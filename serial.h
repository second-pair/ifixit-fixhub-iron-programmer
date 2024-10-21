/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 Maybe we workin', maybe we no.

 -=-  Part Of  -=-
 the_project

 -=-  Credit where Credit is Due  -=-
 -=>  Library / code snippet etc.

//  *--</Preface>--*  */

#ifndef SERIAL_H
#define SERIAL_H



//  *--<Definitions>--*  //

//  Includes
#include "includes.h"

//  Defines
#define SERIAL_BUFF_SIZE 1024
#define SERIAL_CMD_SIZE 64
#define SERIAL_PARAM_SIZE 32

//  Global Type Definitions

//  Global Enumerations
typedef enum ironCommandType
{
	ironCmdType_version_get,
	ironCmdType_spTemp_get, ironCmdType_spTemp_set
} ironCommandType;

//  Global Structures
typedef struct ironCommand
{
	ironCommandType type;
	char params [SERIAL_PARAM_SIZE];
	//idleFunc
} ironCommand;

//  Truly Global Variables

//  Global Prototype Functions
//  Attempts to open the given string as a Serial Port.
void serial_init (const char* portPath);
//  Closes the Serial Port if open.
void serial_close (void);
//  Returns whether the Serial Port is open.
uint8_t serial_isOpen ();
//  Adds the heap-alloceated 'command' to the Serial Command queue.
void serial_cmd_submit (ironCommand* command);
//  Creates the no-parameter 'command' for the given command type.
void serial_cmd_noParams_submit (ironCommandType type);

//  *--</Definitions>--*  //



#endif
