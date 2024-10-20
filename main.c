/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 My own work for my owned device.

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
//#include <pthread.h>  //  Must be first - https://www.ibm.com/docs/en/aix/7.2?topic=p-pthread-mutex-initializer-macro
#include "includes.h"

//  Defines
#define GUI_TITLE "C GTK4 Template"
#define GUI_URL "uk.second-pair.templates.c-gtk4"
#define GUI_APP_FLAGS G_APPLICATION_DEFAULT_FLAGS
/*  Window Sizing
0:  No target size - as allocated.
1:  Fixed target size.
2:  Fullscreen.
*/
#define GUI_WINDOW_MODE 1
#define GUI_WINDOW_SIZE_W 640
#define GUI_WINDOW_SIZE_H 480

//  Type Definitions

//  Structures

//  Local Global Variables
static volatile uint8_t gui_keepRunning = 1;

//  Truly Global Variables

//  Prototype Functions
int main (int argc, char** argv);
GtkWidget* gui_layout_create (void);

//  Prototype Callbacks
void cb_app_main_activate (GtkApplication* theApp, gpointer data);
void cb_signal_terminate (int sigType);

//  *--</Preparations>--*  //



//  *--<Main Code>--*  //

int main (int argc, char** argv)
{
	_LOG (0, "-=-  "TEXT_TL_TITLE"  -=-\n");
	_LOG (0, "Compiled:  %04d-%02d-%02d %s\n", COMP_YEAR, COMP_MONTH, COMP_DAY, __TIME__);

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

GtkWidget* gui_layout_create (void)
{
	//  Create the top-level container widget.
	GtkWidget* box_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign (box_main, GTK_ALIGN_CENTER);
	gtk_widget_set_valign (box_main, GTK_ALIGN_CENTER);

	//  Add a test child widget.
	gtk_box_append (GTK_BOX (box_main), gtk_label_new ("Hello, world!\n"));

	return box_main;
}

//  *--</Main Code>--*  //



//  *--<Callbacks>--*  //

void cb_app_main_activate (GtkApplication* theApp, gpointer data)
{
	//  Create the window.
	GtkWidget* window_main = gtk_application_window_new (theApp);
	gtk_window_set_title (GTK_WINDOW (window_main), GUI_TITLE);

	//  Determine how we want to size the window.
	#if (GUI_WINDOW_MODE == 1)
		gtk_window_set_default_size (GTK_WINDOW (window_main), GUI_WINDOW_SIZE_W, GUI_WINDOW_SIZE_H);
	#elif(GUI_WINDOW_MODE == 2)
		gtk_window_fullscreen (GTK_WINDOW (window_main));
	#endif

	//  Create its child and present it.
	gtk_window_set_child (GTK_WINDOW (window_main), gui_layout_create ());
	gtk_window_present (GTK_WINDOW (window_main));
}

//  Trap the signals we're interested in.
void cb_signal_terminate (int sigType)
{
	gui_keepRunning = 0;
	exit (0);
}

//  *--</Callbacks>--*  //
