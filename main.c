/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 My own work for my owned device.

 -=-  Project Name  -=-
 ifixit-fixhub-iron-programmer

 -=-  Dates  -=-
 Started 2024-10-20

 -=-  Description  -=-
 This programme provides an GUI interface for programming and monitoring an iFixit FixHub Soldering Iron.
 IO is through interaction with a GTK4 GUI and through Serial Comms.
 Config is through compile-time '#define's.

 -=-  Credit where Credit is Due  -=-
 -=>  Library / code snippet etc.

 -=-  Task  -=-
 -=>  Create a basic GUI with one or two core parameters thrown in and a button to establish Serial comms.
 -=>  Establish Serial comms and read the Version.
 -=>  Write setters/getters for these core parameters & hook them up to the GUI.
 -=>  Lay out the rest of the GUI & their callbacks.
 -=>  Write the remaining setters/getters & hook these up to the GUI.
 -=>  Write a loop that retrieves our readouts and displays them.
 -=>  Update the Serial GUI section to select an arbitrary port.
 -=>  Consider perssistent settings, like a JSON Config file.
 -=>  Make things look nice.
 -=>  Consider multilingual support.

 -=-  Notes  -=-
 -=>  I've developed my own commenting notation for things that "aren't done" one way or another.  Such as:
	 -  //#  TODO
	 -  //?  Not sure / query
	 -  //!  Important note / relevant as technology advances
 -=>  Over time, I intend for my logging to be presented in-app.  For now:
 -=>  Logging with `_LOG ()` takes a 'logLevel' argument, which roughly indicates:
	 -  0:  Critical, major errors, should Always be printed.
	 -  1:	Important info particularly critical functions, minor / user errors.
	 -  2:	Useful info / general programme flow.
	 -  3:	Debug info, steps throughout a function.
	 -  4:  Useful spam - printed often such as in a loop.
	 -  5:  Debug spam - printed often such as in a loop.

//  *--</Preface>--*  */



//  *--<Preparations>--*  //

//  Includes
#include "includes.h"

//  Defines
#define GUI_APP_FLAGS G_APPLICATION_DEFAULT_FLAGS

//  Type Definitions

//  Structures

//  Local Global Variables
static volatile uint8_t gui_keepRunning = 1;

//  Truly Global Variables

//  Prototype Functions
int main (int argc, char** argv);

//  Prototype Callbacks
void cb_signal_terminate (int sigType);

//  *--</Preparations>--*  //



//  *--<Main Code>--*  //

int main (int argc, char** argv)
{
	_LOG (0, "-=-  "TEXT_TL_TITLE"  -=-\n");
	_LOG (0, "Compiled:  %04d-%02d-%02d %s\n", COMP_YEAR, COMP_MONTH, COMP_DAY, __TIME__);
	_LOG (0, "'libserialport' version:  %s\n", sp_get_package_version_string ());

	//  Register signal handlers for signals we want to trap.
	#if (defined _WIN32 || defined _WINDOWS || defined __CYGWIN__ || defined __NT__)
		//  Windows can't speak 'sigaction'.
		signal (SIGINT, cb_signal_terminate);
		signal (SIGQUIT, cb_signal_terminate);
		signal (SIGABRT, cb_signal_terminate);
		signal (SIGTERM, cb_signal_terminate);
	#else
		//  UNIX should use 'sigaction' where possible.
		struct sigaction sigHandler;
		sigHandler .sa_handler = cb_signal_terminate;
	    sigaction (SIGINT, &sigHandler, NULL);
		sigaction (SIGQUIT, &sigHandler, NULL);
		sigaction (SIGABRT, &sigHandler, NULL);
		sigaction (SIGTERM, &sigHandler, NULL);
	#endif

	//  Create and run the application.
	GtkApplication* app_main = gtk_application_new (GUI_URL, GUI_APP_FLAGS);
	g_signal_connect (app_main, "activate", G_CALLBACK (cb_app_main_activate), NULL);
	int status = g_application_run (G_APPLICATION (app_main), argc, argv);

	//  Clean up and head on out.
	g_object_unref (app_main);
	return status;
}

//  *--</Main Code>--*  //



//  *--<Callbacks>--*  //

//  Trap the signals we're interested in.
void cb_signal_terminate (int sigType)
{
	gui_keepRunning = 0;
	if (serial_isOpen ())
		serial_close ();
	exit (0);
}

//  *--</Callbacks>--*  //
