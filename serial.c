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
#define WAIT_COUNT 100
#define WAIT_SLEEP_MS 1

#define PORT_BAUD 115200
#define PORT_BITS_DATA 8
#define PORT_PARITY SP_PARITY_NONE
#define PORT_BITS_STOP 0
#define PORT_FLOW_CONTROL SP_FLOWCONTROL_NONE

#define CMD_VERSION_GET "version\n"
#define CMD_VERSION_GET_LEN 8
#define CMD_SP_TEMP_GET "settings get activetemp\n"
#define CMD_SP_TEMP_GET_LEN 24

//  Local Type Definitions

//  Local Structures

//  Local Global Variables
struct sp_port* port_iron = NULL;
GAsyncQueue* priv_cmdQueue = NULL;

//  Truly Global Variables

//  Local Prototype Functions
static inline int priv_read (uint8_t* buffRead, const char* cmdStr, uint16_t length, uint8_t** start);
static inline int priv_read_oneliner (uint8_t* buffRead, const char* cmdStr, uint16_t length, uint8_t** start);
static inline int16_t priv_read_int16_t (uint8_t* buffRead, const char* cmdStr, uint16_t length);
static inline void priv_waitInStart (void);
static inline void priv_waitInComplete (void);
static inline void priv_version_get (void)
static inline void priv_spTemp_get (void)
static inline void priv_spTemp_set (void)

//  *--</Preparations>--*  //



//  *--<Public Code>--*  //


void serial_init (const char* portPath)
{
	//  Open the Serial port.
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
	//  Configure the Serial Port's settings.
	struct sp_port_config* config_iron;
	sp_new_config (&config_iron);
	sp_set_config_baudrate (config_iron, PORT_BAUD);
	sp_set_config_bits (config_iron, PORT_BITS_DATA);
	sp_set_config_parity (config_iron, PORT_PARITY);
	sp_set_config_stopbits (config_iron, PORT_BITS_STOP);
	sp_set_config_flowcontrol (config_iron, PORT_FLOW_CONTROL);
	sp_set_config (port_iron, config_iron);
	sp_free_config (config_iron);
	//  Set up the queue.
	priv_cmdQueue = g_async_queue_new ();
}

void serial_close (void)
{
	//  Close the port & set it to NULL.
	_NULL_RETURN_VOID (port_iron);
	sp_close (port_iron);
	port_iron = NULL;
	//  Clean up the Serial Command queue.
	_NULL_RETURN_VOID (priv_cmdQueue);
	g_async_queue_lock (priv_cmdQueue);
	while (g_async_queue_length_unlocked (priv_cmdQueue))
	{
		ironCommand* ironCmd = g_async_queue_pop_unlocked (priv_cmdQueue);
		free (ironCmd);
	}
	//  `g_async_queue_unref_and_unlock ()` is for some reason deprecated, so we've just got to hope that no-one submits any data between the following two lines.
	g_async_queue_unlock (priv_cmdQueue);
	g_async_queue_unref (priv_cmdQueue);
	priv_cmdQueue = NULL;
}

uint8_t serial_isOpen (void)
{
	return port_iron != NULL;
}


//  Validate the queue & command and add the command to the queue.
void serial_cmd_submit (ironCommand* ironCmd)
{
	_NULL_RETURN_VOID (ironCmd);
	//  Ensure the length is sane.  We can assume the final character will always be a '\0'.
	if (ironCmd -> paramLength >= SERIAL_PARAM_SIZE)
		ironCmd -> paramLength = SERIAL_PARAM_SIZE-1;
	if (priv_cmdQueue != NULL)
		g_async_queue_push (priv_cmdQueue, ironCmd);
}

//  Creates a command from the passed arguments and submits it to the queue.
void serial_cmd_noParams_submit (ironCommandType type)
{
	ironCommand* ironCmd = malloc (sizeof (ironCommand));
	_NULL_EXIT (ironCmd);
	ironCmd -> type = type;
	ironCmd -> params [0] = '\0';
	ironCmd -> paramLength = 0;
	if (priv_cmdQueue != NULL)
		g_async_queue_push (priv_cmdQueue, ironCmd);
}


//  *--</Public Code>--*  //



//  *--<Private Code>--*  //


//  Send a command, wait for the iron to send its response, skip the echo-back and return the rest.
static inline int priv_read (uint8_t* buffRead, const char* cmdStr, uint16_t length, uint8_t** start)
{
	_NULL_RETURN (port_iron, -1);
	//  Write our command and wait for a suitable response to queue.
	sp_nonblocking_write (port_iron, cmdStr, length);
	priv_waitInStart ();
	priv_waitInComplete ();
	//  Read the response & check for errors.
	int amount = sp_nonblocking_read (port_iron, buffRead, SERIAL_BUFF_SIZE);
	if (amount >= SERIAL_BUFF_SIZE)
	{
		buffRead [SERIAL_BUFF_SIZE-1] = '\0';
		_LOG (1, "Buffer full!\n");
	}
	if (amount < 0)
	{
		buffRead [0] = '\0';
		_LOG (1, "Read failed!  %d\n", amount);
		return amount;
	}
	//  Skip past the echo-back first line response.
	int stt = 0;
	for (; buffRead [stt] != '\n' && buffRead [stt] != '\0' && stt < amount; (stt) ++) {}
	if (stt < amount-1) (stt) ++;
	//  Return
	*start = buffRead + stt;
	return amount;
}

//  `priv_read ()`, but caps the response to the first line after the echo-back.
static inline int priv_read_oneliner (uint8_t* buffRead, const char* cmdStr, uint16_t length, uint8_t** start)
{
	//  Perform a command-send-and-read and check for errors.
	int amount = priv_read (buffRead, cmdStr, length, start);
	if (amount < 0) return -1;
	//  Find the end of the response line and cap the string there.
	int end = 0;
	for (; (*start) [end] != '\n' && (*start) [end] != '\0' && end < amount; (end) ++) {}
	(*start) [end] = '\0';
	//  Sorted.
	return amount;
}

//  `priv_read_oneliner ()`, but parses the remaining line into an 'int16_t'.
static inline int16_t priv_read_int16_t (uint8_t* buffRead, const char* cmdStr, uint16_t length)
{
	//  Perform a one-liner read and check for errors.
	uint8_t* start;
	int amount = priv_read_oneliner (buffRead, cmdStr, length, &start);
	if (amount < 0) return INT16_MAX;
	//  Decode the response.
	int decode = strtol ((char*)start, NULL, 10);
	if (decode > INT16_MAX) decode = INT16_MAX;
	if (decode < INT16_MIN) decode = INT16_MIN;
	//  Return it.
	return decode;
}


//  Wait upto 'WAIT_COUNT' times for Serial data to start gathering.
static inline void priv_waitInStart (void)
{
	for (uint16_t i = 0; i < WAIT_COUNT; i ++)
	{
		if (sp_input_waiting (port_iron))
			break;
		_SLEEP_MS (WAIT_SLEEP_MS);
	}
}

//  Wait for a steady stream of Serial data to finish streaming in.
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


//  Getter functions.

static inline void priv_version_get (void)
{
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	uint8_t* start;
	int amount = priv_read (buffRead, CMD_VERSION_GET, CMD_VERSION_GET_LEN, &start);
	if (amount < 0) return;
	printf ("Version:\n%s\n", start);
}

static inline void priv_spTemp_get (void)
{
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	int16_t number = priv_read_int16_t (buffRead, CMD_SP_TEMP_GET, CMD_SP_TEMP_GET_LEN);
	_VALUE_IS_RETURN_VOID (number, INT16_MAX);
	printf ("SP Temp:  %u\n", number);
	//gui_spTemp_update (number);
}


//  Setter functions.

static inline void priv_spTemp_set (void)
{}


//  *--</Private Code>--*  //



//  *--<Callbacks>--*  //

//  *--</Callbacks>--*  //
