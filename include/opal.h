/// @file opal.h
#ifndef OPAL_H_
#define OPAL_H_

/// __BUILD_NUM macro defined during build
#ifndef __BUILD_NUMBER
#define __BUILD_NUMBER 0
#endif  /* __BUILD_NUMBER */

#define TRUE 1          ///< Alias for better code readability
#define FALSE 0         ///< Alias for better code readability

/// Log level name enum for opal_log function
typedef enum log_level
{
  NONE, ERROR, INFO, DEBUG, RESULT
} log_level_e;
short LOG_LEVEL = ERROR;        ///< Current log level

#define _PASS (logger(RESULT, " - PASS"))   ///< Macro function to log PASS
#define _FAIL (logger(RESULT, " - FAIL"))   ///< Macro function to log FAIL
#define _DONE (logger(RESULT, " .. DONE"))  ///< Macro function to log DONE

/**
 * Macro used to print log messages with caller file name, line number and
 * function, followed the formatted string
 */
#define logger(tag, ...) opal_log(tag, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Common data structures and variables used
 */

char *source_fn = NULL;         ///< Input source file name
char *dest_fn = NULL;           ///< Destination file name
char *log_fn = NULL;            ///< Log file name

FILE *source_fd = NULL;         ///< Source file pointer
FILE *dest_fd = NULL;           ///< Destination file pointer
FILE *log_fd = NULL;            ///< Log file pointer

short retVal = 0;               ///< Function return value

/// Buffer used to populate error message string for perror()
char perror_msg[1024] = { 0 };

/*
 * ==================================
 * COMMON FUNCTION DECLARATIONS
 * ==================================
 */
/// Print formatted message to log file
void opal_log (log_level_e, const char*, int, const char*, const char*, ...);
/// Print a banner with stars above and below given string
void banner (const char*);
/// Close open files, flush buffers and exit
short opal_exit (short);

/*
 * ==================================
 * MARC FUNCTION DECLARATIONS
 * ==================================
 */
/// Read source, remove comments, write to destination
short rem_comments(FILE*, FILE*);
short proc_includes(FILE*, FILE*);

#endif /* OPAL_H_ */
