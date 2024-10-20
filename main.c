/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 My own work for my owned device.

 -=-  Dates  -=-
 Started 2024-10-20

 -=-  Description  -=-
 This programme provides an GUI interface for programming and monitoring an iFixit FixHub Soldering Iron.
 IO is through GUI interaction and through Serial Comms.
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

//  Type Definitions

//  Structures

//  Global Variables

//  Prototype Functions
int main (int argc, char** argv);

//  *--</Preparations>--*  //



//  *--<Main Code>--*  //

int main (int argc, char** argv)
{
	_LOG (0, "-=-  "TEXT_TL_TITLE"  -=-\n");
	_LOG (0, "Compiled:  %04d-%02d-%02d %s\n", COMP_YEAR, COMP_MONTH, COMP_DAY, __TIME__);
	return 0;
}

//  *--</Main Code>--*  //



//  *--<Callbacks>--*  //

//  *--</Callbacks>--*  //
