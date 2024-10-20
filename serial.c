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
#define BUFF_SIZE 256
#define WAIT_COUNT 100
#define WAIT_SLEEP_MS 1

#define CMD_VERSION_GET "version\n"
#define CMD_VERSION_GET_LEN 8
#define CMD_SP_TEMP_GET "settings get activetemp\n"
#define CMD_SP_TEMP_GET_LEN 24

//  Local Type Definitions

//  Local Structures

//  Local Global Variables
struct sp_port* port_iron = NULL;

//  Truly Global Variables

//  Local Prototype Functions
static inline int priv_read (uint8_t* buffRead, const char* cmd, uint16_t length, uint8_t** start);
static inline int priv_read_oneliner (uint8_t* buffRead, const char* cmd, uint16_t length, uint8_t** start);
static inline int16_t priv_read_int16_t (uint8_t* buffRead, const char* cmd, uint16_t length);
static inline void priv_waitInStart (void);
static inline void priv_waitInComplete (void);

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
	_NULL_RETURN_VOID (port_iron);
	sp_close (port_iron);
	port_iron = NULL;
}

uint8_t serial_isOpen (void)
{
	return port_iron != NULL;
}

static inline int priv_read (uint8_t* buffRead, const char* cmd, uint16_t length, uint8_t** start)
{
	_NULL_RETURN (port_iron, -1);
	sp_nonblocking_write (port_iron, cmd, length);
	priv_waitInStart ();
	priv_waitInComplete ();
	int amount = sp_nonblocking_read (port_iron, buffRead, BUFF_SIZE);
	if (amount >= BUFF_SIZE)
	{
		buffRead [BUFF_SIZE-1] = '\0';
		_LOG (1, "Buffer full!\n");
	}
	if (amount < 0)
	{
		buffRead [0] = '\0';
		_LOG (1, "Read failed!  %d\n", amount);
		return amount;
	}
	int stt = 0;
	for (; buffRead [stt] != '\n' && buffRead [stt] != '\0' && stt < amount; (stt) ++) {}
	if (stt < amount-1) (stt) ++;
	*start = buffRead + stt;
	return amount;
}
static inline int priv_read_oneliner (uint8_t* buffRead, const char* cmd, uint16_t length, uint8_t** start)
{
	int amount = priv_read (buffRead, cmd, length, start);
	if (amount < 0) return -1;
	int end = 0;
	for (; (*start) [end] != '\n' && (*start) [end] != '\0' && end < amount; (end) ++) {}
	(*start) [end] = '\0';
	return amount;
}
static inline int16_t priv_read_int16_t (uint8_t* buffRead, const char* cmd, uint16_t length)
{
	uint8_t* start;
	int amount = priv_read_oneliner (buffRead, cmd, length, &start);
	if (amount < 0) return INT16_MAX;
	int decode = strtol ((char*)start, NULL, 10);
	if (decode > INT16_MAX) decode = INT16_MAX;
	if (decode < INT16_MIN) decode = INT16_MIN;
	return decode;
}
static inline void priv_waitInStart (void)
{
	for (uint16_t i = 0; i < WAIT_COUNT; i ++)
	{
		if (sp_input_waiting (port_iron))
			break;
		_SLEEP_MS (WAIT_SLEEP_MS);
	}
}
static inline void priv_waitInComplete (void)
{
	int waitingLast = 0;
	int waiting = 0;
	do
	{
		_SLEEP_MS (WAIT_SLEEP_MS);
		waitingLast = waiting;
		waiting = sp_input_waiting (port_iron);
	}
	while (waiting != waitingLast);
}


void serial_version_get (void)
{
	uint8_t buffRead [BUFF_SIZE];
	uint8_t* start;
	int amount = priv_read (buffRead, CMD_VERSION_GET, CMD_VERSION_GET_LEN, &start);
	if (amount < 0) return;
	printf ("Version:\n%s\n", start);
}

void serial_spTemp_get (void)
{
	uint8_t buffRead [BUFF_SIZE];
	int16_t number = priv_read_int16_t (buffRead, CMD_SP_TEMP_GET, CMD_SP_TEMP_GET_LEN);
	_VALUE_IS_RETURN_VOID (number, INT16_MAX);
	printf ("SP Temp:  %u\n", number);
	//gui_spTemp_update (number);
}

//  *--</Main Code>--*  //



//  *--<Callbacks>--*  //

//  *--</Callbacks>--*  //
