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

//  Global Type Definitions

//  Global Structures

//  Truly Global Variables

//  Global Prototype Functions
//  Attempts to open the given string as a Serial Port.
void serial_init (const char* portPath);
//  Closes the Serial Port if open.
void serial_close (void);
//  Returns whether the Serial Port is open.
uint8_t serial_isOpen ();
//  Getter functions.
void serial_version_get (void);
void serial_spTemp_get (void);
//  Setter functions.
//void serial_spTemp_set (uint16_t newSp);

//  *--</Definitions>--*  //



#endif
