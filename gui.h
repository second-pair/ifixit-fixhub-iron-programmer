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
#define GUI_PORT_DEFAULT "/dev/ttyACM0"
#define GUI_SP_TEMP_DEFAULT "350"
#define GUI_MAX_TEMP_DEFAULT "420"
#define GUI_IDLE_ENABLE_DEFAULT 1
#define GUI_IDLE_TIMER_DEFAULT "30"
#define GUI_IDLE_TEMP_DEFAULT "100"
#define GUI_SLEEP_ENABLE_DEFAULT 1
#define GUI_SLEEP_ENABLE_TIMER "60"
#define GUI_UNIT_CELSIUS_DEFAULT 1
#define GUI_CAL_TEMP_DEFAULT "0"

//#  Defaults for the Config section.

//  Global Type Definitions

//  Global Structures

//  Truly Global Variables

//  Global Prototype Functions
void cb_app_main_activate (GtkApplication* theApp, gpointer data);
void gui_btnPort_forceState (uint8_t openNClosed);
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
