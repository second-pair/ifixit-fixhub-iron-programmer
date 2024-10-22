/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 My own work for my owned device.

 -=-  Part Of  -=-
 ifixit-fixhub-iron-programmer

 -=-  Description  -=-
 GUI functions.

 -=-  Credit where Credit is Due  -=-
 -=>  Library / code snippet etc.

//  *--</Preface>--*  */

#ifndef GUI_H
#define GUI_H



//  *--<Definitions>--*  //

//  Includes
#include "includes.h"

//  Defines
#if defined _WIN32 || defined _WINDOWS || defined __CYGWIN__ || defined __NT__
	#define GUI_PORT_DEFAULT "COM0"
#else
	#define GUI_PORT_DEFAULT "/dev/ttyACM0"
#endif
//  Defaults for the Config section.
#define GUI_SP_TEMP_DEFAULT 350
#define GUI_MAX_TEMP_DEFAULT 420
#define GUI_IDLE_ENABLE_DEFAULT 1
#define GUI_IDLE_TIMER_DEFAULT 30
#define GUI_IDLE_TEMP_DEFAULT 100
#define GUI_SLEEP_ENABLE_DEFAULT 1
#define GUI_SLEEP_ENABLE_TIMER 60
#define GUI_UNIT_CELSIUS_DEFAULT 1
#define GUI_CAL_TEMP_DEFAULT 0

//  Global Type Definitions

//  Global Enumerations
typedef enum ironState
{
	ironState_switchOff,
	ironState_noTip,
	ironState_sleep,
	ironState_idle,
	ironState_heating,
	ironState_cooling,
	ironState_atTarget,
	ironState_unknown
} ironState;

//  Global Structures

//  Truly Global Variables

//  Global Prototype Functions
void cb_app_main_activate (GtkApplication* theApp, gpointer data);
void gui_btnPort_forceState (uint8_t openNClosed);
void gui_sw_idleEnable_forceState (uint8_t onNOff);
void gui_sw_sleepEnable_forceState (uint8_t onNOff);
void gui_sw_units_forceState (uint8_t onNOff);

void gui_liveTemp_update (float newValue);
void gui_livePower_update (uint16_t newValue);
void gui_liveDutyCycle_update (uint16_t newValue);
void gui_state_update (ironState newValue);
void gui_spTemp_update (uint16_t newValue);
void gui_maxTemp_update (uint16_t newValue);
void gui_idleEnable_update (uint16_t newValue);
void gui_idleTimer_update (uint16_t newValue);
void gui_idleTemp_update (uint16_t newValue);
void gui_sleepEnable_update (uint16_t newValue);
void gui_sleepTimer_update (uint16_t newValue);
void gui_units_update (uint16_t newValue);
void gui_calTemp_update (int16_t newValue);

//  *--</Definitions>--*  //



#endif
