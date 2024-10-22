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
#include "libserialport/libserialport.h"

//  Defines
#define WAIT_COUNT 100
#define WAIT_SLEEP_MS 1
#define THREAD_SLEEP_MS (333 / SERIAL_IRON_GET_TYPE_MAX)
#define THREAD_QUEUE_WAIT_MS 1

//  Serial port config.
#define PORT_BAUD 115200
#define PORT_BITS_DATA 8
#define PORT_PARITY SP_PARITY_NONE
#define PORT_BITS_STOP 0
#define PORT_FLOW_CONTROL SP_FLOWCONTROL_NONE

//  Command strings.
//  Setter functions.
#define CMD_SP_TEMP_SET "settings set activetemp"
#define CMD_SP_TEMP_SET_LEN 23
#define CMD_MAX_TEMP_SET "settings set maxtemp"
#define CMD_MAX_TEMP_SET_LEN 20
#define CMD_IDLE_ENABLE_SET "settings set idletimerenable"
#define CMD_IDLE_ENABLE_SET_LEN 28
#define CMD_IDLE_TIMER_SET "settings set idletimer"
#define CMD_IDLE_TIMER_SET_LEN 22
#define CMD_IDLE_TEMP_SET "settings set idletemp"
#define CMD_IDLE_TEMP_SET_LEN 21
#define CMD_SLEEP_ENABLE_SET "settings set sleeptimerenable"
#define CMD_SLEEP_ENABLE_SET_LEN 29
#define CMD_SLEEP_TIMER_SET "settings set sleeptimer"
#define CMD_SLEEP_TIMER_SET_LEN 23
#define CMD_UNITS_SET "settings set units"
#define CMD_UNITS_SET_LEN 18
#define CMD_CAL_TEMP_SET "settings set tempcorrection"
#define CMD_CAL_TEMP_SET_LEN 27
//  Getter functions.
#define CMD_VERSION_GET "version\n"
#define CMD_VERSION_GET_LEN 8
#define CMD_HEATER_DETAILS_GET "heater details\n"
#define CMD_HEATER_DETAILS_GET_LEN 15
#define CMD_HEATER_DETAILS_GET "heater details\n"
#define CMD_HEATER_DETAILS_GET_LEN 15
#define CMD_SP_TEMP_GET "settings get activetemp\n"
#define CMD_SP_TEMP_GET_LEN CMD_SP_TEMP_SET_LEN+1
#define CMD_MAX_TEMP_GET "settings get maxtemp\n"
#define CMD_MAX_TEMP_GET_LEN CMD_MAX_TEMP_SET_LEN+1
#define CMD_IDLE_ENABLE_GET "settings get idletimerenable\n"
#define CMD_IDLE_ENABLE_GET_LEN CMD_IDLE_ENABLE_SET_LEN+1
#define CMD_IDLE_TIMER_GET "settings get idletimer\n"
#define CMD_IDLE_TIMER_GET_LEN CMD_IDLE_TIMER_SET_LEN+1
#define CMD_IDLE_TEMP_GET "settings get idletemp\n"
#define CMD_IDLE_TEMP_GET_LEN CMD_IDLE_TEMP_SET_LEN+1
#define CMD_SLEEP_ENABLE_GET "settings get sleeptimerenable\n"
#define CMD_SLEEP_ENABLE_GET_LEN CMD_SLEEP_ENABLE_SET_LEN+1
#define CMD_SLEEP_TIMER_GET "settings get sleeptimer\n"
#define CMD_SLEEP_TIMER_GET_LEN CMD_SLEEP_TIMER_SET_LEN+1
#define CMD_UNITS_GET "settings get units\n"
#define CMD_UNITS_GET_LEN CMD_UNITS_SET_LEN+1
#define CMD_CAL_TEMP_GET "settings get tempcorrection\n"
#define CMD_CAL_TEMP_GET_LEN CMD_CAL_TEMP_SET_LEN+1
//  Operation command functions.
//#define CMD_RESET "reset\n"
//#define CMD_RESET_LEN "6"
#define CMD_REBOOT "reset\n"
#define CMD_REBOOT_LEN 6

//  Local Type Definitions

//  Local Structures

//  Local Global Variables
struct sp_port* port_iron = NULL;
GAsyncQueue* priv_cmdQueue = NULL;
pthread_t thread_serial;
sig_atomic_t thread_run = 0;

//  Truly Global Variables

//  Local Prototype Functions
//  Sending & Reading functions.
static inline int priv_read (uint8_t* buffRead, const char* cmdStr, uint16_t length);
static inline int priv_read_skipEchoBack (uint8_t* buffRead, const char* cmdStr, uint16_t length, uint8_t** start);
static inline int priv_read_oneliner (uint8_t* buffRead, const char* cmdStr, uint16_t length, uint8_t** start);
static inline int16_t priv_read_int16_t (uint8_t* buffRead, const char* cmdStr, uint16_t length);
static inline uint16_t priv_read_uint16_t (uint8_t* buffRead, const char* cmdStr, uint16_t length);
static inline int8_t priv_send_params (const char* baseCmd, uint16_t length, const char* params);
//  Other Serial helper functions.
static inline void priv_waitInStart (void);
static inline void priv_waitInComplete (void);
//  Serial command managing functions.
static inline void priv_serCmd_despatch (ironCommand* ironCmd);
static void priv_serRoutine_next (void);
//  Buffer manipulation functions.
static inline uint16_t priv_buff_skipLines (uint8_t** start, uint16_t skips, int buffLen);
static inline uint16_t priv_buff_skipChars (uint8_t** start, uint16_t skips, int buffLen);
static inline uint16_t priv_buff_skipLinesAndChars (uint8_t** start, uint16_t lineSkips, uint16_t charSkips, int buffLen);
static inline uint16_t priv_buff_findParamLen (uint8_t** start, int buffLen);
static inline void priv_buff_copyNextParam (uint8_t** start, int buffLen, char* buffDest);
static inline void skipToParamAndExtract_str (uint8_t** start, uint16_t lineSkips, uint16_t charSkips, int* buffLen, char* buffDest);


//  Getter functions.
static inline void priv_version_get (void);
static inline void priv_heaterDetails_get (void);
static inline void priv_spTemp_get (void);
static inline void priv_maxTemp_get (void);
static inline int8_t priv_idleEnable_get (void);
static inline void priv_idleTimer_get (void);
static inline void priv_idleTemp_get (void);
static inline int8_t priv_sleepEnable_get (void);
static inline void priv_sleepTimer_get (void);
static inline int8_t priv_units_get (void);
static inline void priv_calTemp_get (void);
//  Setter functions.
static inline void priv_spTemp_set (ironCommand* ironCmd);
static inline void priv_maxTemp_set (ironCommand* ironCmd);
static inline void priv_idleEnable_set (ironCommand* ironCmd);
static inline void priv_idleTimer_set (ironCommand* ironCmd);
static inline void priv_idleTemp_set (ironCommand* ironCmd);
static inline void priv_sleepEnable_set (ironCommand* ironCmd);
static inline void priv_sleepTimer_set (ironCommand* ironCmd);
static inline void priv_units_set (ironCommand* ironCmd);
static inline void priv_calTemp_set (ironCommand* ironCmd);
//  Operation command functions.
//static inline void priv_reset (void);
static inline void priv_reboot (uint8_t restore);
//  ...  of Which are Callbacks
void* thread_serial_run (void* args);

//  *--</Preparations>--*  //



//  *--<Public Code>--*  //

//#  Check if the iron's name is "iFixit Iron DFU" and put it back into normal mode if it is.  This, btw, is probably how we would do firmware updates etc.
void serial_init (const char* portPath)
{
	//  Open the Serial port.
	int retCode;
	retCode = sp_get_port_by_name (portPath, &port_iron);
	if (retCode < 0)
	{
		port_iron = NULL;
		_LOG (1, "sp_get_port_by_name:  %d\n", retCode);
		return;
	}
	retCode = sp_open (port_iron, SP_MODE_READ_WRITE);
	if (retCode < 0)
	{
		port_iron = NULL;
		_LOG (1, "sp_open:  %d\n", retCode);
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

	//  Flush the input buffer.
	priv_waitInComplete ();
	sp_flush (port_iron, SP_BUF_INPUT);

	//  Perform initial parameter requests - mainly for switches.
	//#  It's probably worth extending this to all of the variables.
	gui_sw_idleEnable_forceState (priv_idleEnable_get () == 1 ? 1 : 0);
	gui_sw_sleepEnable_forceState (priv_sleepEnable_get () == 1 ? 1 : 0);
	gui_sw_units_forceState (priv_units_get () == 1 ? 1 : 0);

	//  Create the command-handling thread.
	thread_run = 1;
	pthread_create (&thread_serial, NULL, thread_serial_run	, NULL);
}

void serial_close (void)
{
	//  Close the port & set it to NULL.
	_NULL_RETURN_VOID (port_iron);
	sp_close (port_iron);
	sp_free_port (port_iron);
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


//  Sending & Reading functions.

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
	*start = buffRead;
	return amount - priv_buff_skipLines (start, 1, amount);
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

//  `priv_read_oneliner ()`, but parses the remaining line into the associated type.
static inline int16_t priv_read_int16_t (uint8_t* buffRead, const char* cmdStr, uint16_t length)
{
	//  Perform a one-liner read and check for errors.
	uint8_t* start;
	int amount = priv_read_oneliner (buffRead, cmdStr, length, &start);
	if (amount < 0)
		return INT16_MAX;
	//  Decode the response.
	int decode = strtol ((char*)start, NULL, 10);
	if (decode > INT16_MAX) decode = INT16_MAX;
	if (decode < INT16_MIN) decode = INT16_MIN;
	//  Return it.
	return (int16_t)decode;
}
static inline uint16_t priv_read_uint16_t (uint8_t* buffRead, const char* cmdStr, uint16_t length)
{
	//  Perform a one-liner read and check for errors.
	uint8_t* start;
	int amount = priv_read_oneliner (buffRead, cmdStr, length, &start);
	if (amount < 0)
		return UINT16_MAX;
	//  Decode the response.
	int decode = strtol ((char*)start, NULL, 10);
	if (decode > UINT16_MAX) decode = UINT16_MAX;
	if (decode < 0) decode = 0;
	//  Return it.
	return (uint16_t)decode;
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


//  Other Serial helper functions.

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


//  Serial command managing functions.

//  Serial Command despatcher.
static inline void priv_serCmd_despatch (ironCommand* ironCmd)
{
	switch (ironCmd -> type)
	{
		//  Getter functions.
		case ironCmdType_version_get:
			priv_version_get ();
			break;
		case ironCmdType_heaterDetails_get:
			priv_heaterDetails_get ();
			break;
		case ironCmdType_spTemp_get:
			priv_spTemp_get ();
			break;
		case ironCmdType_maxTemp_get:
			priv_maxTemp_get ();
			break;
		case ironCmdType_idleEnable_get:
			priv_idleEnable_get ();
			break;
		case ironCmdType_idleTimer_get:
			priv_idleTimer_get ();
			break;
		case ironCmdType_idleTemp_get:
			priv_idleTemp_get ();
			break;
		case ironCmdType_sleepEnable_get:
			priv_sleepEnable_get ();
			break;
		case ironCmdType_sleepTimer_get:
			priv_sleepTimer_get ();
			break;
		case ironCmdType_units_get:
			priv_units_get ();
			break;
		//  Setter functions.
		case ironCmdType_calTemp_get:
			priv_calTemp_get ();
			break;
		case ironCmdType_spTemp_set:
			priv_spTemp_set (ironCmd);
			break;
		case ironCmdType_maxTemp_set:
			priv_maxTemp_set (ironCmd);
			break;
		case ironCmdType_idleEnable_set:
			priv_idleEnable_set (ironCmd);
			break;
		case ironCmdType_idleTimer_set:
			priv_idleTimer_set (ironCmd);
			break;
		case ironCmdType_idleTemp_set:
			priv_idleTemp_set (ironCmd);
			break;
		case ironCmdType_sleepEnable_set:
			priv_sleepEnable_set (ironCmd);
			break;
		case ironCmdType_sleepTimer_set:
			priv_sleepTimer_set (ironCmd);
			break;
		case ironCmdType_units_set:
			priv_units_set (ironCmd);
			break;
		case ironCmdType_calTemp_set:
			priv_calTemp_set (ironCmd);
			break;
		//  Operation command functions.
		//case ironCmdType_reset:
		//	priv_reset ();
		//	break;
		case ironCmdType_reboot:
			priv_reboot (SERIAL_REBOOT_RESTORE);
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
		case ironCmdType_heaterDetails_get:
			priv_heaterDetails_get ();
			break;
		case ironCmdType_spTemp_get:
			priv_spTemp_get ();
			break;
		case ironCmdType_maxTemp_get:
			priv_maxTemp_get ();
			break;
		case ironCmdType_idleEnable_get:
			priv_idleEnable_get ();
			break;
		case ironCmdType_idleTimer_get:
			priv_idleTimer_get ();
			break;
		case ironCmdType_idleTemp_get:
			priv_idleTemp_get ();
			break;
		case ironCmdType_sleepEnable_get:
			priv_sleepEnable_get ();
			break;
		case ironCmdType_sleepTimer_get:
			priv_sleepTimer_get ();
			break;
		case ironCmdType_units_get:
			priv_units_get ();
			break;
		case ironCmdType_calTemp_get:
			priv_calTemp_get ();
			break;
		default:
			break;
	}
	//  Prevent "overflowing" into the Set region.
	if (typeNext >= SERIAL_IRON_GET_TYPE_MAX)
		typeNext = 0;
}


//  Buffer manipulation functions.

//  Skip past 'skips' newline sections in the provided buffer pointer.
static inline uint16_t priv_buff_skipLines (uint8_t** start, uint16_t skips, int buffLen)
{
	uint16_t stt = 0;
	for (uint16_t i = 0; i < skips; i ++)
	{
		//  Find the next newline.
		for (; (*start) [stt] != '\r' && (*start) [stt] != '\n' && (*start) [stt] != '\0' && stt < buffLen; stt ++) {}
		//  Skip the rest of the newline block.
		for (; ((*start) [stt] == '\r' || (*start) [stt] == '\n') && stt < buffLen; (stt) ++) {}
	}
	//  Return
	(*start) += stt;
	return stt;
}
static inline uint16_t priv_buff_skipChars (uint8_t** start, uint16_t skips, int buffLen)
{
	uint16_t stt = 0;
	//  Skip for as long as it is safe to do so.
	for (; stt < skips && (*start) [stt] != '\0' && stt < buffLen; stt ++) {}
	//  Return
	(*start) += stt;
	return stt;
}
static inline uint16_t priv_buff_skipLinesAndChars (uint8_t** start, uint16_t lineSkips, uint16_t charSkips, int buffLen)
{
	uint16_t total = 0;
	total += priv_buff_skipLines (start, lineSkips, buffLen);
	total += priv_buff_skipChars (start, charSkips, buffLen);
	return total;
}
static inline uint16_t priv_buff_findParamLen (uint8_t** start, int buffLen)
{
	uint16_t stt = 0;
	//  Find the next newline.
	for (; (*start) [stt] != '\r' && (*start) [stt] != '\n' && (*start) [stt] != '\0' && stt < buffLen; stt ++) {}
	return stt;
}
static inline void priv_buff_copyNextParam (uint8_t** start, int buffLen, char* buffDest)
{
	uint16_t paramLen = priv_buff_findParamLen (start, buffLen);
	memcpy (buffDest, *start, paramLen);
	buffDest [paramLen] = '\0';
}
static inline void skipToParamAndExtract_str (uint8_t** start, uint16_t lineSkips, uint16_t charSkips, int* buffLen, char* buffDest)
{
	*buffLen -= priv_buff_skipLinesAndChars (start, lineSkips, charSkips, *buffLen);
	priv_buff_copyNextParam (start, *buffLen, buffDest);
}
#define skipToParamAndExtract_float(start, lineSkips, charSkips, buffLen, type, capMin, capMax) \
({ \
	buffLen -= priv_buff_skipLinesAndChars (&start, lineSkips, charSkips, buffLen); \
	type value = strtof ((char*)start, NULL); \
	_CAP_RANGE (1, "%f", value, capMin, capMax); \
	value; \
})
#define skipToParamAndExtract_int(start, lineSkips, charSkips, buffLen, type, capMin, capMax) \
({ \
	buffLen -= priv_buff_skipLinesAndChars (&start, lineSkips, charSkips, buffLen); \
	type value = strtol ((char*)start, NULL, 10); \
	_CAP_RANGE (1, "%d", value, capMin, capMax); \
	value; \
})


//  Getter functions.

#define priv_get(nameLower, nameUpper, type, maxBound) \
({ \
	uint8_t buffRead [SERIAL_BUFF_SIZE]; \
	type number = priv_read_##type (buffRead, CMD_##nameUpper##_GET, CMD_##nameUpper##_GET_LEN); \
	_VALUE_IS_RETURN_VOID (number, maxBound); \
	gui_##nameLower##_update (number); \
})

static inline void priv_spTemp_get (void)
	{  priv_get (spTemp, SP_TEMP, uint16_t, UINT16_MAX);  }
static inline void priv_maxTemp_get (void)
	{  priv_get (maxTemp, MAX_TEMP, uint16_t, UINT16_MAX);  }
static inline void priv_idleTimer_get (void)
	{  priv_get (idleTimer, IDLE_TIMER, uint16_t, UINT16_MAX);  }
static inline void priv_idleTemp_get (void)
	{  priv_get (idleTemp, IDLE_TEMP, uint16_t, UINT16_MAX);  }
static inline void priv_sleepTimer_get (void)
	{  priv_get (sleepTimer, SLEEP_TIMER, uint16_t, UINT16_MAX);  }
static inline void priv_calTemp_get (void)
	{  priv_get (calTemp, CAL_TEMP, int16_t, INT16_MAX);  }

static inline void priv_version_get (void)
{
	//  Get the information.
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	uint8_t* start;
	int amount = priv_read_skipEchoBack (buffRead, CMD_VERSION_GET, CMD_VERSION_GET_LEN, &start);
	if (amount < 0)
	{
		_LOG (1, "No data received!  Returning...\n");
		return;
	}
	//  Parse the information.
	_LOG (5, "Version:\n%s\n", start);
}

static inline void priv_heaterDetails_get (void)
{
	//  Get the information.
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	uint8_t* start;
	int amount = priv_read_skipEchoBack (buffRead, CMD_HEATER_DETAILS_GET, CMD_HEATER_DETAILS_GET_LEN, &start);
	if (amount < 0)
	{
		_LOG (1, "No data received!  Returning...\n");
		return;
	}
	_LOG (5, "Heater Details:\n\n%s\n", start);

	/*  Parse the information.
	06:  | current filtered temp: 22.28 C|
	10:  |          tip power mW: 2250|
	11:  |  pid output desired W: 0|
	12:  |        pwm duty cycle: 100|
	13:  |            switch on?: No|Yes|
	14:  |        tip installed?: No|Yes|
	15:  |                 state: Off|Active|
	No Yes Off  //  Switch Off
	Yes Yes Off  //  Switch On, no Heat - "Sleep"
	Yes Yes Active  //  Heating - "Active"
	*/

	//  Live Temp
	gui_liveTemp_update (skipToParamAndExtract_float
		(start, 5, 24, amount, float, 0.0, 10000.0));
	//  Live Power
	gui_livePower_update (skipToParamAndExtract_int
		(start, 5, 24, amount, uint16_t, 0, UINT16_MAX));
	//  Live DC
	gui_liveDutyCycle_update (skipToParamAndExtract_int
		(start, 1, 24, amount, uint16_t, 0, UINT16_MAX));

	char buffState [SERIAL_PARAM_SIZE];
	//  State - Switch Off
	skipToParamAndExtract_str (&start, 1, 24, &amount, buffState);
	if (_STRING_COMPARE (buffState, "No"))
	{
		gui_state_update (ironState_switchOff);
		return;
	}
	else if (! _STRING_COMPARE (buffState, "Yes"))
	{
		_LOG (0, "Unknown State:  |%s|\n", start);
		gui_state_update (ironState_unknown);
		return;
	}
	//  State - No Tip
	skipToParamAndExtract_str (&start, 1, 24, &amount, buffState);
	if (_STRING_COMPARE (buffState, "No"))
	{
		gui_state_update (ironState_noTip);
		return;
	}
	else if (! _STRING_COMPARE (buffState, "Yes"))
	{
		_LOG (0, "Unknown State:  |%s|\n", start);
		gui_state_update (ironState_unknown);
		return;
	}
	//  State - State
	skipToParamAndExtract_str (&start, 1, 24, &amount, buffState);
	if (_STRING_COMPARE (buffState, "Off"))
	{
		gui_state_update (ironState_sleep);
		return;
	}
	else if (_STRING_COMPARE (buffState, "Active"))
	{
		gui_state_update (ironState_heating);
		return;
	}
	else if (_STRING_COMPARE (buffState, "Idle"))
	{
		gui_state_update (ironState_idle);
		return;
	}
	else if (_STRING_COMPARE (buffState, "Sleep"))
	{
		gui_state_update (ironState_sleep);
		return;
	}
	else
	{
		_LOG (0, "Unknown State:  |%s|\n", start);
		gui_state_update (ironState_unknown);
		return;
	}
}

static inline int8_t priv_idleEnable_get (void)
{
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	uint8_t* start;
	int amount = priv_read_oneliner (buffRead, CMD_IDLE_ENABLE_GET, CMD_IDLE_ENABLE_GET_LEN, &start);
	int8_t retCode = -1;
	if (amount < 0)
	{
		_LOG (1, "No data received!  Returning...\n");
		return retCode;
	}
	else if (_STRING_COMPARE (start, "Enabled\r"))
	{
		retCode = 1;
		gui_idleEnable_update (1);
	}
	else if (_STRING_COMPARE (start, "Disabled\r"))
	{
		retCode = 0;
		gui_idleEnable_update (0);
	}
	else
	{
		_LOG (1, "Bad Idle Enable received!  Returning...\n");
		return retCode;
	}
	_LOG (5, "Idle Enable:  %s\n", start);
	return retCode;
}
static inline int8_t priv_sleepEnable_get (void)
{
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	uint8_t* start;
	int amount = priv_read_oneliner (buffRead, CMD_SLEEP_ENABLE_GET, CMD_SLEEP_ENABLE_GET_LEN, &start);
	int8_t retCode = -1;
	if (amount < 0)
	{
		_LOG (1, "No data received!  Returning...\n");
		return retCode;
	}
	else if (_STRING_COMPARE (start, "Enabled\r"))
	{
		retCode = 1;
		gui_sleepEnable_update (1);
	}
	else if (_STRING_COMPARE (start, "Disabled\r"))
	{
		retCode = 0;
		gui_sleepEnable_update (0);
	}
	else
	{
		_LOG (1, "Bad Sleep Enable received!  Returning...\n");
		return retCode;
	}
	_LOG (5, "Sleep Enable:  %s\n", start);
	return retCode;
}
static inline int8_t priv_units_get (void)
{
	uint8_t buffRead [SERIAL_BUFF_SIZE];
	uint8_t* start;
	int amount = priv_read_oneliner (buffRead, CMD_UNITS_GET, CMD_UNITS_GET_LEN, &start);
	int8_t retCode = -1;
	if (amount < 0)
	{
		_LOG (1, "No data received!  Returning...\n");
		return retCode;
	}
	else if (_STRING_COMPARE (start, "C\r"))
	{
		retCode = 1;
		gui_units_update (1);
	}
	else if (_STRING_COMPARE (start, "F\r"))
	{
		retCode = 0;
		gui_units_update (0);
	}
	else
	{
		_LOG (1, "Bad Unit received!  Returning...\n");
		return retCode;
	}
	_LOG (5, "Units:  %s\n", start);
	return retCode;
}


//  Setter functions.
#define priv_set(cmd, cmdLen, readbackFunc) \
({ \
	int amount = priv_send_params (cmd, cmdLen, ironCmd -> params); \
	if (amount < 0) \
		return; \
	readbackFunc (); \
})
static inline void priv_spTemp_set (ironCommand* ironCmd)
	{  priv_set (CMD_SP_TEMP_SET, CMD_SP_TEMP_SET_LEN, priv_spTemp_get);  }
static inline void priv_maxTemp_set (ironCommand* ironCmd)
	{  priv_set (CMD_MAX_TEMP_SET, CMD_MAX_TEMP_SET_LEN, priv_maxTemp_get);  }
static inline void priv_idleEnable_set (ironCommand* ironCmd)
	{  priv_set (CMD_IDLE_ENABLE_SET, CMD_IDLE_ENABLE_SET_LEN, priv_idleEnable_get);  }
static inline void priv_idleTimer_set (ironCommand* ironCmd)
	{  priv_set (CMD_IDLE_TIMER_SET, CMD_IDLE_TIMER_SET_LEN, priv_idleTimer_get);  }
static inline void priv_idleTemp_set (ironCommand* ironCmd)
	{  priv_set (CMD_IDLE_TEMP_SET, CMD_IDLE_TEMP_SET_LEN, priv_idleTemp_get);  }
static inline void priv_sleepEnable_set (ironCommand* ironCmd)
	{  priv_set (CMD_SLEEP_ENABLE_SET, CMD_SLEEP_ENABLE_SET_LEN, priv_sleepEnable_get);  }
static inline void priv_sleepTimer_set (ironCommand* ironCmd)
	{  priv_set (CMD_SLEEP_TIMER_SET, CMD_SLEEP_TIMER_SET_LEN, priv_sleepTimer_get);  }
static inline void priv_units_set (ironCommand* ironCmd)
	{  priv_set (CMD_UNITS_SET, CMD_UNITS_SET_LEN, priv_units_get);  }
static inline void priv_calTemp_set (ironCommand* ironCmd)
	{  priv_set (CMD_CAL_TEMP_SET, CMD_CAL_TEMP_SET_LEN, priv_calTemp_get);  }

//  Operation command functions.
//  I couldn't find a firmware-tied reset command, so I'll be implementing my own in 'gui.c'.
//static inline void priv_reset (void)
//{
//	_LOG (1, "Reset not yet implemented.\n");
//}
static inline void priv_reboot (uint8_t restore)
{
	_LOG (1, "Performing Iron reboot sequence.\n");
	//  I'll implement a one-off custom for this, as the iron should reboot once this has been sent.
	_NULL_RETURN_VOID (port_iron);
	//  Retrieve the port path for re-opening later.
	const char* _portPath = sp_get_port_name (port_iron);
	char portPath [SERIAL_PORT_PATH_LEN];
	snprintf (portPath, SERIAL_PORT_PATH_LEN, "%s", _portPath);
	//  Write the reboot command.
	sp_nonblocking_write (port_iron, CMD_REBOOT, CMD_REBOOT_LEN);
	//  Close the port.
	serial_close ();
	//  Wait for a wee bit.
	_SLEEP_MS (SERIAL_REBOOT_DELAY_MS);
	//  Re-open it.
	if (restore)
		serial_init (portPath);
	else
		gui_btnPort_forceState (0);
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
		//  Implementing this so we don't bog down the soldering iron; which seems to perform its core-loop at around 3Hz.
		//#  Implement a "Wait Until" instead of a "Wait For".
		_SLEEP_MS (THREAD_SLEEP_MS);
	}
	return NULL;
}

//  *--</Callbacks>--*  //
