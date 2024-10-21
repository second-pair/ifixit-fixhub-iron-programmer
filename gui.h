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

//  Global Type Definitions

//  Global Structures

//  Truly Global Variables

//  Global Prototype Functions
void cb_app_main_activate (GtkApplication* theApp, gpointer data);
void gui_spTemp_update (uint16_t newSp);
void gui_maxTemp_update (uint16_t newSp);

//  *--</Definitions>--*  //



#endif
