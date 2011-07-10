﻿using System;
using System.Collections;
using LuaInterface;
using LuaNetInterface;
using Ponykart.Levels;

namespace Ponykart.Lua
{
	public delegate void LuaEventHandler();

	// PackageAttributes are kinda like namespaces... I think? Anyway if you set this to null then it's treated as if it's in the global namespace
	// obviously if you want it to be in the global namespace, adding documentation for that namespace is pointless
	[LuaPackage(null, null)]
	public class LuaMain
	{
		public LuaVirtualMachine LuaVM { get; private set; }

		public event LuaEventHandler OnRegister;

		public LuaMain() {
			LuaVM = new LuaVirtualMachine();

			// for an implementation of a lua console, see the LuaNetInterface example
			// obviously I'd have to hook it up to the UI instead of it just being in the console...

			// we don't have to register them on level load because this is a global singleton, not a level one
			RegisterLuaFunctions(this);
			// though we do have to restart lua when we change levels
			LKernel.Get<LevelManager>().OnLevelLoad += (e) => Restart();


			LuaVM.Lua.DoString("print(\"All working over here!\")");
			Launch.Log("[Loading] Scripting engine successfully started!");
		}

		/// <summary>
		/// If you have a class that needs to have some of its functions added to lua, add this to the constructor:
		/// LKernel.Get&lt;LuaMain&gt;().RegisterLuaFunctions(this);
		/// It will subscribe that object to the OnRegister event.
		/// </summary>
		/// <param name="o">The class whose functions you want to add to the Lua VM</param>
		/// <remarks>Shorthand</remarks>
		public void RegisterLuaFunctions(object o) {
			Launch.Log("[Lua Main] Registering lua functions from " + o.GetType());

			OnRegister += () => LuaVM.RegisterLuaFunctions(o);
		}

		/// <summary>
		/// make lua parse and execute a string of code
		/// </summary>
		/// <param name="s">the string to execute</param>
		public void DoString(string s) {
			if (LKernel.Get<LevelManager>().IsValidLevel) {
				try {
					LuaVM.Lua.DoString(s);
					Console.WriteLine();
				} catch (Exception ex) {
					Launch.Log("[Lua] *** EXCEPTION *** at " + ex.Source + ": " + ex.Message + "\n");
					LKernel.Get<UI.LuaConsoleManager>().AddLabel("ERROR: " + ex.Message);
					Launch.Log(ex.StackTrace);
				}
			}
		}

		/// <summary>
		/// make lua parse and execute a file
		/// </summary>
		/// <param name="file">the filename of the file to execute</param>
		[LuaFunction("doFile", "Runs a lua script.", "string file - The filename. If it doesn't start with media/scripts/, it's added automagically.")]
		public void DoFile(string file) {
			if (LKernel.Get<LevelManager>().IsValidLevel) {
				// adding this in case you try to run a script but forget the file path
				if (!file.StartsWith("media/scripts/") && !file.StartsWith("/media/scripts/"))
					file = "media/scripts/" + file;

				try {
					LuaVM.Lua.DoFile(file);
					Console.WriteLine();
				} catch (Exception ex) {
					Launch.Log("[Lua] *** EXCEPTION *** at " + ex.Source + ": " + ex.Message + "\n");
					LKernel.Get<UI.LuaConsoleManager>().AddLabel("ERROR: " + ex.Message);
					Launch.Log(ex.StackTrace);
				}
			}
		}

		/// <summary>
		/// Gets all of the wrappers to (re-)register their functions
		/// </summary>
		public void RunRegisterEvent() {
			if (OnRegister != null)
				OnRegister();
		}

		#region basic lua stuff

		/// <summary>
		/// Quits the Lua VM. Does not dispose it - use Restart() if you want to do that.
		/// </summary>
		[LuaFunction("quit", "Quits the lua VM.")]
		public void Quit() {
			LuaVM.Stop();
		}

		/// <summary>
		/// Quits, disposes, and creates a new Lua VM, then runs the re-registration event.
		/// </summary>
		[LuaFunction("restart", "Shuts down the Lua VM and starts it again.")]
		public void Restart() {
			Quit();
			LuaVM.Lua.Dispose();
			LuaVM = new LuaVirtualMachine();
			RunRegisterEvent();
		}

		/// <summary>
		/// This overwrites lua's built-in print function so it prints to our console
		/// </summary>
		/// <param name="s">The string to print</param>
		[LuaFunction("print", "Prints something.", "string s - the string to print")]
		public void Print(string s) {
			Launch.Log("[Lua] " + s);
			LKernel.Get<UI.LuaConsoleManager>().AddLabel(s);
		}

		/// <summary>
		/// Gets help for a command
		/// </summary>
		/// <param name="command">Package to get help of.</param>
		[LuaFunction("helpcmd", "Show help for a given command or package", "string command - Package to get help of.")]
		public void GetCommandHelp(string command) {
			LuaPackageDescriptor packageDesc;

			if (LuaVM.Functions.ContainsKey(command)) {
				LuaFunctionDescriptor descriptor = (LuaFunctionDescriptor)LuaVM.Functions[command];
				Console.WriteLine(descriptor.FullDocumentationString);
				LKernel.Get<UI.LuaConsoleManager>().AddLabel(descriptor.FullDocumentationString);

				return;
			}

			if (command.IndexOf(".") == -1) {
				if (LuaVM.Packages.ContainsKey(command)) {
					LuaPackageDescriptor descriptor = (LuaPackageDescriptor)LuaVM.Packages[command];
					descriptor.ShowHelp();

					return;
				} else {
					Console.WriteLine("No such function or package: " + command);
					LKernel.Get<UI.LuaConsoleManager>().AddLabel("No such function or package: " + command);

					return;
				}
			}

			string[] parts = command.Split('.');

			if (!LuaVM.Packages.ContainsKey(parts[0])) {
				Console.WriteLine("No such function or package: " + command);
				LKernel.Get<UI.LuaConsoleManager>().AddLabel("No such function or package: " + command);
				return;
			}

			packageDesc = (LuaPackageDescriptor)LuaVM.Packages[parts[0]];

			if (!packageDesc.HasFunction(parts[1])) {
				Console.WriteLine("Package " + parts[0] + " doesn't have a " + parts[1] + " function.");
				LKernel.Get<UI.LuaConsoleManager>().AddLabel("Package " + parts[0] + " doesn't have a " + parts[1] + " function.");
				return;
			}

			packageDesc.ShowHelp(parts[1]);
		}

		/// <summary>
		/// Lists available commands
		/// </summary>
		[LuaFunction("help", "List available commands.")]
		public void GetHelp() {
			Console.WriteLine("Available commands: ");
			Console.WriteLine();

			IDictionaryEnumerator functions = LuaVM.Functions.GetEnumerator();

			while (functions.MoveNext()) {
				Console.WriteLine(((LuaFunctionDescriptor)functions.Value).GetFunctionHeader());
			}

			if (LuaVM.Packages.Count > 0) {
				Console.WriteLine();
				Console.WriteLine("Available packages: ");

				IDictionaryEnumerator packages = LuaVM.Packages.GetEnumerator();

				while (packages.MoveNext()) {
					Console.WriteLine((string)packages.Key + " - " +
						((LuaPackageDescriptor)packages.Value).GetPackageHeader());
				}
			}
		}
		#endregion
	}
}
