/*
 *  Created on: Feb 25, 2011
 *      Author: Paul
 *		Source: github.com/paulrehkugler/ExtensionSearch
 */

#ifdef _WIN32
	#include <io.h>
	#include "WindowsCompat/dirent.h"
#else
	#include <unistd.h>
	#include <dirent.h>
#endif
#include <iostream>
#include <vector>
#include "direntSearch.h"

using namespace std;

namespace Extensions
{
    std::string getFilename(const std::string& path)
	{
		std::string result;
		auto pos = path.rfind('/');
		if (pos != std::string::npos)	result = std::string(path, pos+1);
		else							result = path;
		pos = result.rfind('\\');
		if (pos != std::string::npos)	result = std::string(result, pos+1);
		return result;
	}

    std::string getFilenameWithoutExtension(const std::string& path) // TODO: Check if this is correct (getFilenameWithoutExtension)
	{
		std::string result;
		auto pos = path.rfind('/');
		if (pos != std::string::npos)	result = std::string(path, pos+1);
		else							result = path;
		pos = result.rfind('\\');
		if (pos != std::string::npos)	result = std::string(result, pos+1);
		pos = result.rfind('.');
		if (pos != std::string::npos)	result = std::string(result, 0, pos);
		return result;
	}


    std::string getFileExtension(const std::string& path) // TODO: Check if this is correct (getFileExtension)
	{
		auto pos = path.rfind('.');
		if (pos != std::string::npos)
			return std::string(path, pos);
		else
			return std::string();
	}

    std::vector<std::string> direntSearch(const std::string& directory, const std::string& extension) // recursive search algorithm
	{
		std::vector<std::string> results;
		DIR* dir_point = opendir(directory.c_str());
		if (!dir_point)
			throw std::string("direntSearch: Failed to open " + directory + ", opendir() returned nullptr");
		dirent* entry = readdir(dir_point);
		while (entry){									// if !entry then end of directory
			if (entry->d_type == DT_REG){		// if entry is a regular file
				std::string fname = entry->d_name;	// filename
													// if filename's last characters are extension
				if (fname.find(extension, (fname.length() - extension.length())) != std::string::npos)
					results.push_back(directory+'/'+fname);		// add filename to results vector
			}
			entry = readdir(dir_point);
		}
		return results;
	}

    std::vector<std::string> direntSearchRec(const std::string& directory, const std::string& extension) // recursive search algorithm
	{
		std::vector<std::string> results;
		DIR* dir_point = opendir(directory.c_str());
		if (!dir_point)
			throw std::string("direntSearchRec: Failed to open " + directory +", opendir() returned nullptr");
		dirent* entry = readdir(dir_point);
		while (entry){									// if !entry then end of directory
			if (entry->d_type == DT_DIR){				// if entry is a directory
				std::string fname = entry->d_name;
				if (fname != "." && fname != "..")
				{
					std::vector<std::string> tmp=direntSearchRec(directory + "/" + entry->d_name, extension); // search through it
					results.insert(end(results), begin(tmp), end(tmp));
				}
			}
			else if (entry->d_type == DT_REG){		// if entry is a regular file
				std::string fname = entry->d_name;	// filename
													// if filename's last characters are extension
				if (fname.find(extension, (fname.length() - extension.length())) != std::string::npos)
					results.push_back(directory+'/'+fname);		// add filename to results vector
			}
			entry = readdir(dir_point);
		}
		return results;
	}
} // Extensions
