#include "pch.h"
#include "MusicSource.h"

#include <string>
#include "Kernel/LKernel.h"
#include "VorbisStream.h"
#include "OpusStream.h"
#include "Misc/direntSearch.h"

using namespace std;
using namespace Ponykart::Sound;
using namespace Extensions;


MusicSource::MusicSource (const string &filename, bool startPaused, float volume)
{
	auto path = LKernel::basePath + "media/music/" + filename;

	auto ext = getFileExtension(filename);
	for (char &c : ext)
		c = tolower(c);

	if (ext == ".ogg")
		stream = new VorbisStream(path);
	else if (ext == ".opus")
		stream = new OpusStream(path);
	else
		throw string("Unsupported music format: " + ext);

	alGenSources(1, &source);
	alGenBuffers(buffers.size(), buffers.data());
	fill();

	alSourcef(source, AL_GAIN, volume);
	if (startPaused)
		alSourcePause(source);
	else
		alSourcePlay(source);
}


Extensions::ALSource MusicSource::getSource () const
{
	return source;
}


void MusicSource::clear ()
{
	alSourceStop(source);
	int bufCount = 0;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &bufCount);
	for (; bufCount > 0; bufCount--) {
		ALBuffer buf;
		alSourceUnqueueBuffers(source, 1, &buf);
	}
}


void MusicSource::fill ()
{
	clear();
	for (auto buf : buffers)
		if (stream->readPCMChunk(buf) > 0)
			alSourceQueueBuffers(source, 1, &buf);
	bufCursor = 0;
}


MusicSource::~MusicSource ()
{
	destroy();
}


void MusicSource::pause ()
{
	if (source < 0)
		return;
	lock_guard<mutex> pumpGuard(pumpLock);

	alSourcePause(source);
}


bool MusicSource::play ()
{
	if (source < 0)
		return false;
	lock_guard<mutex> pumpGuard(pumpLock);

	alSourcePlay(source);
	return true;
}


void MusicSource::stop ()
{
	if (source < 0)
		return;
	lock_guard<mutex> pumpGuard(pumpLock);

	clear();
	stream->reset();
	fill();
}


bool MusicSource::finished ()
{
	if (source < 0)
		return true;

	int state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	if (state == AL_PLAYING || state == AL_PAUSED)
		return false;

	lock_guard<mutex> pumpGuard(pumpLock);
	return stream->finished();
}


void MusicSource::destroy ()
{
	if (source < 0)
		return;
	lock_guard<mutex> pumpGuard(pumpLock);

	clear();
	alDeleteBuffers(buffers.size(), buffers.data());
	alDeleteSources(1, &source);

	delete stream;

	source = -1;
}


void MusicSource::pump ()
{
	lock_guard<mutex> pumpGuard(pumpLock);
	if (source < 0)
		return;

	int processed = 0;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
	bool queuedMore = false;
	for (; processed > 0; processed--) {
		ALBuffer buf;
		alSourceUnqueueBuffers(source, 1, &buf);
		if (stream->readPCMChunk(buf) > 0) {
			alSourceQueueBuffers(source, 1, &buffers[bufCursor++]);
			bufCursor %= buffers.size();
			queuedMore = true;
		}
	}

	if (queuedMore) {
		int state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		if (state == AL_STOPPED)
			alSourcePlay(source);
	}
}
