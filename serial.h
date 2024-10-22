/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 My own work for my owned device.

 -=-  Part Of  -=-
 ifixit-fixhub-iron-programmer

 -=-  Description  -=-
 Serial functions.

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
#define SERIAL_PORT_PATH_LEN 128
#define SERIAL_RESET_DELAY_MS 10
#define SERIAL_RESET_RESTORE 0  //  Haven't yet gotten this to re-sync correctly.
#define SERIAL_IRON_GET_TYPE_MAX 11

//  Global Type Definitions

//  Global Enumerations
typedef enum ironCommandType
{
	//  Getters
	ironCmdType_version_get, ironCmdType_heaterDetails_get,
	ironCmdType_spTemp_get, ironCmdType_maxTemp_get,
	ironCmdType_idleEnable_get, ironCmdType_idleTimer_get, ironCmdType_idleTemp_get,
	ironCmdType_sleepEnable_get, ironCmdType_sleepTimer_get,
	ironCmdType_units_get, ironCmdType_calTemp_get,
	//  Setters
	ironCmdType_spTemp_set, ironCmdType_maxTemp_set,
	ironCmdType_idleEnable_set, ironCmdType_idleTimer_set, ironCmdType_idleTemp_set,
	ironCmdType_sleepEnable_set, ironCmdType_sleepTimer_set,
	ironCmdType_units_set, ironCmdType_calTemp_set,
	//  Operation Commands
	ironCmdType_reset, ironCmdType_reboot
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
