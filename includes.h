/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 My own work for my owned device.

 -=-  Part Of  -=-
 ifixit-fixhub-iron-programmer

 -=-  Description  -=-
 Global include file, so that each part of the project only needs to worry about including this single file.

 -=-  Credit where Credit is Due  -=-
 -=>  Library / code snippet etc.

//  *--</Preface>--*  */

#ifndef INCLUDES_H
#define INCLUDES_H



//  *--<Defines>--*  //

//  Things
#define STUFF 0

//  *--</Defines>--*  //



//  *--<Enumerations>--*  //

//  *--</Enumerations>--*  //



//  *--<Includes>--*  //

//  Library Includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
	#include <pthread.h>  //  Must be first - https://www.ibm.com/docs/en/aix/7.2?topic=p-pthread-mutex-initializer-macro
	#include <stdio.h>
	#include <stdint.h>
	#include <gtk/gtk.h>
#pragma GCC diagnostic pop

//  Third-Party Submodules
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
	//#include "cJSON/cJSON.h"
	#include "libserialport/libserialport.h"
#pragma GCC diagnostic pop

//  Internal Includes
#include "text.h"
#include "macros.c"
#include "serial.h"

//  *--</Includes>--*  //



//  *--<Error Checks>--*  //

#if 0
	#error "Error!"
#endif

//  *--</Error Checks>--*  //



//  *--<Macros>--*  //

//  *--</Macros>--*  //



#endif
