/*  *--<Preface>--*  //

 -=-  Author Details  -=-
 Blair Edwards
 My own work for my owned device.

 -=-  Part Of  -=-
 ifixit-fixhub-iron-programmer

 -=-  Credit where Credit is Due  -=-
 -=>  Library / code snippet etc.

//  *--</Preface>--*  */



//  *--<Preparations>--*  //

//  Includes
#include "serial.h"

//  Defines
#define WAIT_COUNT 100
#define WAIT_SLEEP_MS 1
#define THREAD_SLEEP_MS 1
#define THREAD_QUEUE_WAIT_MS 1

#define PORT_BAUD 115200
#define PORT_BITS_DATA 8
#define PORT_PARITY SP_PARITY_NONE
#define PORT_BITS_STOP 0
#define PORT_FLOW_CONTROL SP_FLOWCONTROL_NONE

#define CMD_VERSION_GET "version\n"
#define CMD_VERSION_GET_LEN 8
#define CMD_SP_TEMP_GET "settings get activetemp\n"
#define CMD_SP_TEMP_GET_LEN 24
#define CMD_SP_TEMP_SET "settings set activetemp"
#define CMD_SP_TEMP_SET_LEN 24
#define CMD_HEATER_DETAILS_GET "heater details\n"
#define CMD_HEATER_DETAILS_GET_LEN 15

//  Local Type Definitions

//  Local Structures

//  Local Global Variables
struct sp_port* port_iron = NULL;
GAsyncQueue* priv_cmdQueue = NULL;
pthread_t thread_serial;
sig_atomic_t thread_run = 0;

//  Truly Global Variables

//  Local Prototype Functions
static inline int priv_read (uint8_t* buffRead, const char* cmdStr, uint16_t length);
static inline int priv_read_skipEchoBack (uint8_t* buffRead, const char* cmdStr, uint16_t length, uint8_t** start);
static inline int priv_read_oneliner (uint8_t* buffRead, const char* cmdStr, uint16_t length, uint8_t** start);
static inline int16_t priv_read_int16_t (uint8_t* buffRead, const char* cmdStr, uint16_t length);
static inline int8_t priv_send_params (const char* baseCmd, uint16_t length, const char* params);
static inline void priv_waitInStart (void);
static inline void priv_waitInComplete (void);
static inline void priv_serCmd_despatch (ironCommand* ironCmd);
static void priv_serRoutine_next (void);
static inline void priv_version_get (void);
static inline void priv_spTemp_get (void);
static inline void priv_spTemp_set (ironCommand* ironCmd);
//  ...  of Which are Callbacks
void* thread_serial_run (void* args);

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

	//  Create the command-handling thread.
	thread_run = 1;
	pthread_create (&thread_serial, NULL, thread_serial_run	, NULL);
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
	while (g_async_queue_length_unlocked (priv_cmdQueue) > 0)
	{
		ironCommand* ironCmd = g_async_queue_pop_unlocked (priv_cmdQueue);
		free (ironCmd);
	}
	//  `g_async_queue_unref_and_unlock ()` is for some reason deprecated, so we've just got to hope that no-one submits any data between the following two lines.
	g_async_queue_unlock (priv_cmdQueue);
	g_async_queue_unref (priv_cmdQueue);
	priv_cmdQueue = NULL;

	//  Stop & collect the command-handling thread.
	thread_run = 0;
	pthread_join (thread_serial, NULL);
}

uint8_t serial_isOpen (void)
{
	return port_iron != NULL;
}


//  Validate the queue & command and add the command to the queue.
void serial_cmd_submit (ironCommand* ironCmd)
{
	_NULL_RETURN_VOID (ironCmd);
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
	if (priv_cmdQueue != NULL)
		g_async_queue_push (priv_cmdQueue, ironCmd);
}


//  *--</Public Code>--*  //



//  *--<Private Code>--*  //


//  Send a command, wait for the iron to send its response, skip the echo-back and return the rest.
static inline int priv_read (uint8_t* buffRead, const char* cmdStr, uint16_t length)
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
	}
	return amount;
}

static inline int priv_read_skipEchoBack (uint8_t* buffRead, const char* cmdStr, uint16_t length, uint8_t** start)
{
	//  Perform the read.
	int amount = priv_read (buffRead, cmdStr, length);
	if (amount < 0)
		return amount;
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
	int amount = priv_read_skipEchoBack (buffRead, cmdStr, length, start);
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

static inline int8_t priv_send_params (const char* baseCmd, uint16_t length, const char* params)
{
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	//  See if we can shortcut the send.
	if (params [0] == '\0')
		return priv_read (buffRead, baseCmd, length);
	//  Build the full command string.
	char fullCmd [SERIAL_CMD_SIZE];
	int amount = snprintf (fullCmd, SERIAL_CMD_SIZE, "%s %s\n", baseCmd, params);
	if (amount > SERIAL_CMD_SIZE)
	{
		_LOG (1, "Full command too long!  %d > %d\n", amount, SERIAL_CMD_SIZE);
		return -1;
	}
	//  Send it.
	return priv_read (buffRead, fullCmd, amount);
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


//  Serial Command despatcher.
static inline void priv_serCmd_despatch (ironCommand* ironCmd)
{
	switch (ironCmd -> type)
	{
		case ironCmdType_version_get:
			priv_version_get ();
			break;
		case ironCmdType_spTemp_get:
			priv_spTemp_get ();
			break;
		case ironCmdType_spTemp_set:
			priv_spTemp_set (ironCmd);
			break;
		default:
			break;
	}
}
//  Routine data-fetch actioner.
static void priv_serRoutine_next (void)
{
	//  Step through the get-type commands each time this function is called.
	static ironCommandType typeNext = 0;
	switch (typeNext++)
	{
		case ironCmdType_version_get:
			priv_version_get ();
			break;
		case ironCmdType_spTemp_get:
			priv_spTemp_get ();
			break;
		default:
			break;
	}
	//  Prevent "overflowing" into the Set region.
	if (typeNext >= SERIAL_IRON_GET_TYPE_MAX)
		typeNext = 0;
}


//  Getter functions.

static inline void priv_version_get (void)
{
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	uint8_t* start;
	int amount = priv_read_skipEchoBack (buffRead, CMD_VERSION_GET, CMD_VERSION_GET_LEN, &start);
	if (amount < 0) return;
	_LOG (5, "Version:\n%s\n", start);
}

static inline void priv_spTemp_get (void)
{
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	int16_t number = priv_read_int16_t (buffRead, CMD_SP_TEMP_GET, CMD_SP_TEMP_GET_LEN);
	_VALUE_IS_RETURN_VOID (number, INT16_MAX);
	gui_spTemp_update (number);
}


//  Setter functions.

static inline void priv_spTemp_set (ironCommand* ironCmd)
{
	int amount = priv_send_params (CMD_SP_TEMP_SET, CMD_SP_TEMP_SET_LEN, ironCmd -> params);
	if (amount < 0) return;
	priv_spTemp_get ();
}


//  *--</Private Code>--*  //



//  *--<Callbacks>--*  //

void* thread_serial_run (void* args)
{
	while (thread_run)
	{
		//  Handle any queued commands.
		ironCommand* ironCmd = g_async_queue_timeout_pop (priv_cmdQueue, THREAD_QUEUE_WAIT_MS);
		if (ironCmd != NULL)
			priv_serCmd_despatch (ironCmd);
		else
			priv_serRoutine_next ();
		//#  Consider implementing this anyway so we don't bog down the soldering iron.
		//_SLEEP_MS (THREAD_SLEEP_MS); - this gets handled fine by `g_async_queue_timeout_pop ()`.
	}
	return NULL;
}

//  *--</Callbacks>--*  //
