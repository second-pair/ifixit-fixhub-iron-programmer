/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 Maybe we workin', maybe we no.

 -=-  Part Of  -=-
 the_project

 -=-  Credit where Credit is Due  -=-
 -=>  Library / code snippet etc.

//  *--</Preface>--*  */



//  *--<Preparations>--*  //

//  Includes
//#include <pthread.h>  //  Must be first - https://www.ibm.com/docs/en/aix/7.2?topic=p-pthread-mutex-initializer-macro
#include "serial.h"

//  Defines

//  Local Type Definitions

//  Local Structures

//  Local Global Variables
struct sp_port* port_iron = NULL;

//  Truly Global Variables

//  Local Prototype Functions
//uint8_t subLocal (void);

//  *--</Preparations>--*  //



//  *--<Main Code>--*  //

void serial_init (const char* portPath)
{
	int retCode;
	retCode = sp_get_port_by_name (portPath, &port_iron);
	if (retCode < 0)
	{
		printf ("sp_get_port_by_name:  %d\n", retCode);
		return;
	}
	retCode = sp_open (port_iron, SP_MODE_READ_WRITE);
	if (retCode < 0)
	{
		printf ("sp_open:  %d\n", retCode);
		return;
	}
	struct sp_port_config* config_iron;
	sp_new_config (&config_iron);
	sp_set_config_baudrate (config_iron, 115200);
	sp_set_config_bits (config_iron, 8);
	sp_set_config_parity (config_iron, SP_PARITY_NONE);
	sp_set_config_stopbits (config_iron, 0);
	sp_set_config_flowcontrol (config_iron, SP_FLOWCONTROL_NONE);
	sp_set_config (port_iron, config_iron);
	sp_free_config (config_iron);
}

void serial_close (void)
{
	sp_close (port_iron);
	port_iron = NULL;
}

uint8_t serial_isOpen (void)
{
	return port_iron != NULL;
}

void serial_version_get (void)
{
	sp_nonblocking_write (port_iron, "version\n", 8);
	_SLEEP_MS (100);
	uint8_t buffRead [256];
	int amount = sp_nonblocking_read (port_iron, buffRead, 256);
	printf ("Read %d:  %s\n", amount, buffRead);
}

void serial_spTemp_get (void)
{
	sp_nonblocking_write (port_iron, "settings get activetemp\n", 24);
	_SLEEP_MS (100);
	uint8_t buffRead [256];
	int amount = sp_nonblocking_read (port_iron, buffRead, 256);
	printf ("Read %d:  %s\n", amount, buffRead);
}

//  *--</Main Code>--*  //



//  *--<Callbacks>--*  //

//  *--</Callbacks>--*  //
