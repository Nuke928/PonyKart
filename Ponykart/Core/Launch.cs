﻿using System;
using System.Diagnostics;
using System.Windows.Forms;
using Mogre;

namespace Ponykart {
	public static class Launch {
	
		[STAThread]
		public static void Main() {
			//#if !DEBUG
			AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(UnhandledException);
			//#endif

			LKernel.Initialise();

			LKernel.Get<Main>().Go();

		}

		/// <summary>
		/// Fired whan an unhandled exception bubbles up to the AppDomain
		/// </summary>
		static void UnhandledException(object sender, UnhandledExceptionEventArgs e)
		{
			if (OgreException.IsThrown)
				MessageBox.Show(OgreException.LastException.FullDescription, "An Ogre exception has occurred!");
			else
			{
				var ex = e.ExceptionObject as Exception;
				if (ex != null)
					MessageBox.Show(ex.Message, ex.GetType().ToString());
			}

			ProcessStartInfo p = new ProcessStartInfo("notepad.exe Ogre.log");
			Process proc = new Process();
			proc.StartInfo = p;
			proc.Start();
			proc.WaitForExit();
		}

		/// <summary>
		/// Writes something to the console and also sticks it in ogre's log file
		/// </summary>
		/// <param name="message">The message to log</param>
		[DebuggerHidden]
		public static void Log(string message) {
			Debug.WriteLine(message);
			if (LogManager.Singleton != null)
				LogManager.Singleton.LogMessage(message);
		}
	}
}