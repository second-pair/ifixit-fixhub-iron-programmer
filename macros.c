//  *--<Macros>--*  //


//  Compilation Date
#define COMP_YEAR ((__DATE__ [7] - '0') * 1000) + (__DATE__ [8] - '0') * 100 + (__DATE__ [9] - '0') * 10 + __DATE__ [10] - '0'
#define COMP_DAY (__DATE__ [4] == ' ' ? __DATE__ [5] - '0' : (__DATE__ [4] - '0') * 10 + __DATE__ [5] - '0')
#define COMP_MONTH ( \
	__DATE__ [0] == 'J' && __DATE__ [1] == 'a' && __DATE__ [2] == 'n' ? 1 : \
	__DATE__ [0] == 'F' && __DATE__ [1] == 'e' && __DATE__ [2] == 'b' ? 2 : \
	__DATE__ [0] == 'M' && __DATE__ [1] == 'a' && __DATE__ [2] == 'r' ? 3 : \
	__DATE__ [0] == 'A' && __DATE__ [1] == 'p' && __DATE__ [2] == 'r' ? 4 : \
	__DATE__ [0] == 'M' && __DATE__ [1] == 'a' && __DATE__ [2] == 'y' ? 5 : \
	__DATE__ [0] == 'J' && __DATE__ [1] == 'u' && __DATE__ [2] == 'n' ? 6 : \
	__DATE__ [0] == 'J' && __DATE__ [1] == 'u' && __DATE__ [2] == 'l' ? 7 : \
	__DATE__ [0] == 'A' && __DATE__ [1] == 'u' && __DATE__ [2] == 'g' ? 8 : \
	__DATE__ [0] == 'S' && __DATE__ [1] == 'e' && __DATE__ [2] == 'p' ? 9 : \
	__DATE__ [0] == 'O' && __DATE__ [1] == 'c' && __DATE__ [2] == 't' ? 10 : \
	__DATE__ [0] == 'N' && __DATE__ [1] == 'o' && __DATE__ [2] == 'v' ? 11 : \
	__DATE__ [0] == 'D' && __DATE__ [1] == 'e' && __DATE__ [2] == 'c' ? 12 : \
	0 \
)


//  Logging Macros
#ifndef LOG_LEVEL
	#define LOG_LEVEL 0
#endif
//  Log to STDOUT, provided we meet the verbosity requirements.
#define _LOG(logLevel, logString, ...) \
({ \
	if (logLevel <= LOG_LEVEL) \
		printf ("LOG%d:  `%s ()`:  " logString, logLevel, __FUNCTION__, ##__VA_ARGS__); \
})
//  As above, but only perform a carriage return, not a line feed.
//  To allow for more verbose logging within an"_LOG_STREAM block", we only do this for the same 'logLevel' as our current verbosity - else we just perform a normal `_LOG ()`.
#define _LOG_STREAM(logLevel, logString, ...) \
({ \
	if (logLevel == LOG_LEVEL) \
	{ \
		printf ("LOG%d:  `%s ()`:  " logString "\r", logLevel, __FUNCTION__, ##__VA_ARGS__);  fflush (stdout); \
	} \
	else \
		_LOG (logLevel + 1, logString "\n", ##__VA_ARGS__); \
})
//  Close out a previous `_LOG_STREAM ()` with a carriage return, if appropriate.
#define _LOG_STREAM_END(logLevel) \
({ \
	if (logLevel == LOG_LEVEL) \
		putchar ('\n'); \
})


//  Check if a variable is NULL, `return retCode;` if it does.
#define _NULL_RETURN(variable, retCode) \
({ \
	if (variable == NULL) \
	{ \
		_LOG (0, ""#variable" == NULL!  Returning...\n"); \
		return retCode; \
	} \
})
//  As above, but simply return in the case of a 'void ()' function type.
#define _NULL_RETURN_VOID(variable) \
({ \
	if (variable == NULL) \
	{ \
		_LOG (0, ""#variable" == NULL!  Returning...\n"); \
		return; \
	} \
})
//  As `_NULL_RETURN ()`, but calls `exit (1)` instead.
#define _NULL_EXIT(variable) \
({ \
	if (variable == NULL) \
	{ \
		_LOG (0, ""#variable" == NULL!  Exiting...\n"); \
		exit (1); \
	} \
})

//  Similar to above.  Check if a variable is a given value, `return retCode;` if it does Not.
#define _VALUE_NOT_RETURN(variable, value, retCode) \
({ \
	if (variable != value) \
	{ \
		_LOG (0, ""#variable" != "#value"!  Returning...\n"); \
		return retCode; \
	} \
})
//  As above, but simply return in the case of a 'void ()' function type.
#define _VALUE_NOT_RETURN_VOID(variable, value) \
({ \
	if (variable != value) \
	{ \
		_LOG (0, ""#variable" != "#value"!  Returning...\n"); \
		return; \
	} \
})
//  As `_VALUE_NOT_RETURN ()`, but calls `exit (1)` instead.
#define _VALUE_NOT_EXIT(variable, value) \
({ \
	if (variable != value) \
	{ \
		_LOG (0, ""#variable" != "#value"!  Exiting...\n"); \
		exit (1); \
	} \
})
//  As `_VALUE_NOT_RETURN ()`, but `return retCode;` if it Does.
#define _VALUE_IS_RETURN(variable, value, retCode) \
({ \
	if (variable == value) \
	{ \
		_LOG (0, ""#variable" == "#value"!  Returning...\n"); \
		return retCode; \
	} \
})
//  As above, but simply return in the case of a 'void ()' function type.
#define _VALUE_IS_RETURN_VOID(variable, value) \
({ \
	if (variable == value) \
	{ \
		_LOG (0, ""#variable" == "#value"!  Returning...\n"); \
		return; \
	} \
})
//  As `_VALUE_IS_RETURN ()`, but calls `exit (1)` instead.
#define _VALUE_IS_EXIT(variable, value) \
({ \
	if (variable == value) \
	{ \
		_LOG (0, ""#variable" == "#value"!  Exiting...\n"); \
		exit (1); \
	} \
})


//  Macros to cap a value.
#define _CAP_HIGH(logLevel, format, variable, limit) \
({ \
	uint8_t retCode = 0; \
	if (variable > limit) \
	{ \
		_LOG (logLevel, "Capping "#variable":  "#format" > "#format".\n", variable, limit); \
		variable = limit; \
		retCode = 1; \
	} \
	retCode; \
})
#define _CAP_LOW(logLevel, format, variable, limit) \
({ \
	uint8_t retCode = 0; \
	if (variable < limit) \
	{ \
		_LOG (logLevel, "Capping "#variable":  "#format" < "#format".\n", variable, limit); \
		variable = limit; \
		retCode = 1; \
	} \
	retCode; \
})
#define _CAP_RANGE(logLevel, format, variable, limitLow, limitHigh) \
({ \
	uint8_t retCode = 0; \
	retCode |= _CAP_LOW (logLevel, format, variable, limitLow); \
	retCode |= _CAP_HIGH (logLevel, format, variable, limitHigh); \
	retCode; \
})

//  Macro to extract the contents of a GTK+ textbox.
#define _TEXTBOX_EXTRACT(textBox, destString, maxLength) \
({ \
	uint8_t didWork = 0; \
	if (textBox != NULL) \
	{ \
		GtkTextBuffer* theBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textBox)); \
		if (theBuffer != NULL) \
		{ \
			GtkTextIter start, end; \
			gtk_text_buffer_get_start_iter (theBuffer, &start); \
			gtk_text_buffer_get_end_iter (theBuffer, &end); \
			if (&start != NULL && &end != NULL) \
			{ \
				snprintf (destString, maxLength, "%s", gtk_text_buffer_get_text (theBuffer, &start, &end, 0)); \
				didWork = 1; \
			} \
		} \
	} \
	if (! didWork) \
		destString [0] = '\0'; \
})

//  Macro to compare two strings.  It's easy to forget something while writing one of these, so here's a neat one-liner.
#define _STRING_COMPARE(string1, string2) \
({ \
	uint8_t didMatch = 1; \
	if (string1 == NULL || string2 == NULL) \
		didMatch = 0; \
	else \
	{ \
		uint16_t currChar = 0; \
		for (; string1 [currChar] != '\0' && string2 [currChar] != '\0' && currChar < UINT16_MAX; currChar ++) \
		{ \
			if (string1 [currChar] != string2 [currChar]) \
			{ \
				didMatch = 0; \
				break; \
			} \
		} \
		if (string1 [currChar] != '\0' || string2 [currChar] != '\0') \
			didMatch = 0; \
	} \
	didMatch; \
})


//  Macros for stripping illegal characters out of strings.
//  This can be altered to optimise for systems that don't need to handle nearly infinitely-long strings.
#define STRIP_COUNT_TYPE uintmax_t
#define STRIP_COUNT_MAX UINTMAX_MAX
/*  Function to check if all the characters in a string are legal.  If they are not, they are removed.
 -  Check if the first character is a sign ('-') and if we allow that, skip it.
 -  Loop through all characters in the string.  For each one:
	1.  Check if we allow floats and if we already have a float marker accounted for.
	2.  Run it through the passed rule macro to check if it is a legal character.
	3.  If it is not, search ahead for the next legal character.
	4.  Shift down all remaining characters to close the gap between these legal characters.
*/
#define _STRIP_HANDLER(stringToStrip, ruleSet, allowSign, allowFloat) \
({ \
	uint8_t hasFloat = ! allowFloat; \
	STRIP_COUNT_TYPE ch = 0; \
	for (; stringToStrip [ch] != '\0' && ch < STRIP_COUNT_MAX; ch ++) \
	{ \
		if (ch == 0 && allowSign && stringToStrip [0] == '-') \
			continue; \
		else if (! hasFloat && (stringToStrip [ch] == '.' || stringToStrip [ch] == ',')) \
			 hasFloat = 1; \
		else if (! ruleSet (stringToStrip [ch])) \
		{ \
			STRIP_COUNT_TYPE nextCh = ch; \
			while (! ruleSet (stringToStrip [nextCh]) && stringToStrip [nextCh] != '\0' && nextCh < STRIP_COUNT_MAX) \
				nextCh ++; \
			STRIP_COUNT_TYPE shiftCh = ch; \
			while (stringToStrip [nextCh] != '\0' && nextCh < STRIP_COUNT_MAX) \
			{ \
				stringToStrip [shiftCh] = stringToStrip [nextCh]; \
				nextCh ++;  shiftCh ++; \
			} \
			stringToStrip [shiftCh] = '\0'; \
		} \
	} \
})

//  Check if a character is not a newline character [\r\n].
#define _STRIP_CHECK_IS_NEWLINE(charToTest) \
	(charToTest == '\r' || charToTest == '\n')
//  Check if a character is a legal number [0-9].
#define _STRIP_CHECK_IS_NUMBER(charToTest) \
	(charToTest >= '0' && charToTest <= '9')
//  Check if a character is a legal float [0-9.].
#define _STRIP_CHECK_IS_FLOAT(charToTest) \
( \
	_STRIP_CHECK_IS_NUMBER (charToTest) || \
	charToTest == '.' || charToTest == ',' \
)
//  Check if a character is a legal hex character [0-9a-fA-F].
#define _STRIP_CHECK_IS_HEX(charToTest) \
( \
	_STRIP_CHECK_IS_NUMBER (charToTest) || \
	(charToTest >= 'A' && charToTest <= 'F') || \
	(charToTest >= 'a' && charToTest <= 'f' ) \
)
//  Check if a character is a legal letter [a-zA-Z].
#define _STRIP_CHECK_IS_ALPHA(charToTest) \
( \
	(charToTest >= 'A' && charToTest <= 'Z') || \
	(charToTest >= 'a' && charToTest <= 'z' ) \
)
//  Check if a character is a legal letter or number [0-9a-zA-Z].
#define _STRIP_CHECK_IS_ALPHANUMERIC(charToTest) \
( \
	_STRIP_CHECK_IS_NUMBER (charToTest) || \
	_STRIP_CHECK_IS_ALPHA (charToTest) \
)
//  Check if a character is a legal POSIX filename character [0-9a-zA-Z.-_ ].
#define _STRIP_CHECK_IS_FN_POSIX(charToTest) \
( \
	_STRIP_CHECK_IS_ALPHANUMERIC (charToTest) || \
	charToTest == '.' || charToTest == '-' || charToTest == '_' || charToTest == ' ' \
)
//  Check if a character is a legal POSIX filepath character [0-9a-zA-Z.-_ /].
#define _STRIP_CHECK_IS_FP_POSIX(charToTest) \
( \
	_STRIP_CHECK_IS_FN_POSIX (charToTest) || \
	charToTest == '/' \
)
//  Check if a character is UTF-8 and not a control character.
#define _STRIP_CHECK_IS_UTF8(charToTest) \
	(charToTest >= ' ' && charToTest <= '~')
//  Check if a character is a legal larger-scope filepath - IE UTF-8 except ["*:<>?\|].
#define _STRIP_CHECK_IS_FILEPATH(charToTest) \
( \
	_STRIP_CHECK_IS_UTF8 (charToTest) && ! \
	(charToTest == '"' || charToTest == '*' || charToTest == ':' || charToTest == '<' || \
	charToTest == '>' || charToTest == '?' || charToTest == '\\' || charToTest == '|') \
)
//  Check if a character is a legal larger-scope filename - IE UTF-8 except ["*/:<>?\|].
#define _STRIP_CHECK_IS_FILENAME(charToTest) \
( \
	_STRIP_CHECK_IS_FILEPATH (charToTest) && \
	charToTest != '/' \
)
//  As above, but including newlines.
#define _STRIP_CHECK_IS_NL_NUMBER(charToTest) \
( \
	_STRIP_CHECK_IS_NUMBER (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_SIGNED(charToTest) \
( \
	_STRIP_CHECK_IS_SIGNED (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_FLOAT(charToTest) \
( \
	_STRIP_CHECK_IS_FLOAT (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_HEX(charToTest) \
( \
	_STRIP_CHECK_IS_HEX (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_ALPHA(charToTest) \
( \
	_STRIP_CHECK_IS_ALPHA (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_ALPHANUMERIC(charToTest) \
( \
	_STRIP_CHECK_IS_ALPHANUMERIC (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_FP_POSIX(charToTest) \
( \
	_STRIP_CHECK_IS_FP_POSIX (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_FN_POSIX(charToTest) \
( \
	_STRIP_CHECK_IS_FN_POSIX (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_UTF8(charToTest) \
( \
	_STRIP_CHECK_IS_UTF8 (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_FILEPATH(charToTest) \
( \
	_STRIP_CHECK_IS_FILEPATH (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)
#define _STRIP_CHECK_IS_NL_FILENAME(charToTest) \
( \
	_STRIP_CHECK_IS_FILENAME (charToTest) || \
	_STRIP_CHECK_IS_NEWLINE (charToTest) \
)

//  Wrappers to hook up the correct character checker macro with the strip-handling function.  'stringToStrip' Must be null-terminated!
#define _STRIP_NEWLINE(stringToStrip) _STRIP_HANDLER (stringToStrip, ! _STRIP_CHECK_IS_NEWLINE, 0, 0);
#define _STRIP_NUMBER(stringToStrip, allowSign) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NUMBER, allowSign, 0);
#define _STRIP_FLOAT(stringToStrip, allowSign) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_FLOAT, allowSign, 1);
#define _STRIP_HEX(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_HEX, 0, 0);
#define _STRIP_ALPHA(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_ALPHA, 0, 0);
#define _STRIP_ALPHANUMERIC(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_ALPHANUMERIC, 0, 0);
#define _STRIP_FP_POSIX(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_FP_POSIX, 0, 0);
#define _STRIP_FN_POSIX(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_FN_POSIX, 0, 0);
#define _STRIP_UTF8(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_UTF8, 0, 0);
#define _STRIP_FILEPATH(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_FILEPATH, 0, 0);
#define _STRIP_FILENAME(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_FILENAME, 0, 0);
#define _STRIP_NL_NUMBER(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_NUMBER, 0, 0);
#define _STRIP_NL_FLOAT(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_FLOAT, 0, 0);
#define _STRIP_NL_HEX(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_HEX, 0, 0);
#define _STRIP_NL_ALPHA(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_ALPHA, 0, 0);
#define _STRIP_NL_ALPHANUMERIC(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_ALPHANUMERIC, 0, 0);
#define _STRIP_NL_FP_POSIX(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_FP_POSIX, 0, 0);
#define _STRIP_NL_FN_POSIX(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_FN_POSIX, 0, 0);
#define _STRIP_NL_UTF8(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_UTF8, 0, 0);
#define _STRIP_NL_FILEPATH(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_FILEPATH, 0, 0);
#define _STRIP_NL_FILENAME(stringToStrip) _STRIP_HANDLER (stringToStrip, _STRIP_CHECK_IS_NL_FILENAME, 0, 0);


//  Macro to implement a somewhat platform-neutral sleep function.
//!  If threading, try to avoid using this where synchronisation may be affected.
//!  If using a library with its own timing systems, try to use those instead of this.
//!  Windows requires a copy of 'libwinpthread-1.dll' to be present in the PWD, or on the PATH.
//  The idea here is to provide a few options to the implemeter - efficient but less accurate sleeps, vs accurate but less efficient sleeps.
//  The `_SLEEP_* ()` functions do waiting by repeadedly sleeping the thread and checking if we've made it to our wait point.
//  The `_SLEEP_**_SPIN ()` functions sleep the thread until we're close to our destination, then perform a "spin-wait" (AKA "busy-wait") for the remainder.
//  In practice, the spin-waits seem to be accurate to +/- 200ns, but waste a lot of CPU time, particularly for waits < 100ms.
#include <time.h>
#define _SLEEP_MS(delayMs) \
({ \
	struct timespec amount; \
	amount .tv_sec = delayMs / 1000; \
	amount .tv_nsec = delayMs % 1000000000; \
	nanosleep (&amount, NULL); \
})
//  If we've got more than 100ms waiting to do, wait until there's that much left.
//  Spin-sleep 'till seconds are equal.
//  Spin-sleep 'till nanoseconds are equal.
#define _SLEEP_SPIN(theTarget) \
({ \
	struct timespec timeCurr; \
	clock_gettime (CLOCK_MONOTONIC, &timeCurr); \
	if (theTarget .tv_sec > (timeCurr .tv_sec + 1) || theTarget .tv_nsec > (timeCurr .tv_nsec + 100000000)) \
		_SLEEP_MS ((long unsigned int)((theTarget .tv_sec - timeCurr .tv_sec) * 1000 + (theTarget .tv_nsec - timeCurr .tv_nsec) / 1000000 - 100)); \
	while (theTarget .tv_sec > timeCurr .tv_sec) \
			clock_gettime (CLOCK_MONOTONIC, &timeCurr); \
	while (theTarget .tv_nsec > timeCurr .tv_nsec) \
		clock_gettime (CLOCK_MONOTONIC, &timeCurr); \
})
//  Normal-sleep for milliseconds.
//  Spin-sleep 'till nanoseconds are equal.  This is unavoidable for nanoseconds.
#define _SLEEP_NS(delayNs) \
({ \
	struct timespec timeCurr, timeTarget; \
	clock_gettime (CLOCK_MONOTONIC, &timeCurr); \
	timeTarget .tv_sec = timeCurr .tv_sec + (timeCurr .tv_nsec + delayNs) / 1000000000; \
	timeTarget .tv_nsec = (timeCurr .tv_nsec + delayNs) % 1000000000; \
	_SLEEP_MS ((long unsigned int)((timeTarget .tv_sec - timeCurr .tv_sec) * 1000 + (timeTarget .tv_nsec - timeCurr .tv_nsec) / 1000000)); \
	while (timeTarget .tv_nsec > timeCurr .tv_nsec) \
		clock_gettime (CLOCK_MONOTONIC, &timeCurr); \
})
//  Calculate the difference and call `_SLEEP_SPIN ()`.
#define _SLEEP_NS_SPIN(delayNs) \
({ \
	struct timespec timeTarget; \
	clock_gettime (CLOCK_MONOTONIC, &timeTarget); \
	timeTarget .tv_sec += (timeTarget .tv_nsec + delayNs) / 1000000000; \
	timeTarget .tv_nsec = (timeTarget .tv_nsec + delayNs) % 1000000000; \
	_SLEEP_SPIN (timeTarget); \
})
//  Calculate the difference and call `_SLEEP_SPIN ()`.
#define _SLEEP_MS_SPIN(delayMs) \
({ \
	struct timespec timeTarget; \
	clock_gettime (CLOCK_MONOTONIC, &timeTarget); \
	timeTarget .tv_sec += delayMs / 1000 + (timeTarget .tv_nsec + ((delayMs % 1000) * 1000000)) / 1000000000; \
		timeTarget .tv_nsec = (timeTarget .tv_nsec + ((delayMs % 1000) * 1000000)) % 1000000000; \
	_SLEEP_SPIN (timeTarget); \
})

//  Macros to facilitate simple timing.
#include <time.h>
static struct timespec __attribute__ ((__unused__)) _TIMER_STRUCT;
#define _TIMER_START() \
({ \
	clock_gettime (CLOCK_MONOTONIC, &_TIMER_STRUCT); \
})
#define _TIMER_END(logLevel) \
({ \
	struct timespec endTimer; \
	clock_gettime (CLOCK_MONOTONIC, &endTimer); \
	_LOG (logLevel, "Timer:  %lfs.\n", (double)(endTimer .tv_sec - _TIMER_STRUCT .tv_sec) + (double)(endTimer .tv_nsec - _TIMER_STRUCT .tv_nsec) / 1000000000.0); \
})


//  *--</Macros>--*  //
