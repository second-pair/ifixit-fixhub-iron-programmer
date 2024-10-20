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
void serial_init (const char* portPath);
void serial_close (void);
uint8_t serial_isOpen ();
void serial_version_get (void);
void serial_spTemp_get (void);

//  *--</Definitions>--*  //



#endif
