/// @file opal.c
/// @authors Damle Kedar, Mckerracher Joshua, Leon Sarah Louise
///
#include <stdio.h>
#include <stdarg.h>             /* variadic functions */
#include <assert.h>             /* assert() */
#include <stdlib.h>             /* fopen, fclose, exit() */
#include <string.h>             /* memset() */
#include <errno.h>              /* errno macros and codes */
#include <stdbool.h>            /* boolean datatypes */

#include "../include/opal.h"

/*
 * ==================================
 * START COMMON FUNCTION DEFINITIONS
 * ==================================
 */

/**
 * @brief       Print formatted message to log file
 *
 * @details     Helper function to log messages. Function writes to global
 * variable log_fd. Usually called by a macro logger. Eg:
 *
 * ```
 * logger (ERROR, "Cannot read file: %s", file_name);
 * logger (DEBUG, "access('%s', F_OK)", source_fn);
 * ```
 *
 * @param[in]   tag     Log level of message
 * @param[in]   file    Source file name
 * @param[in]   line    Source file line number
 * @param[in]   line    Source file function
 * @param[in]   fmt     Formatted message to log
 *
 * @return      None
 *
 */

void
opal_log (log_level_e tag, const char *file, int line, const char *func,
          const char *fmt, ...)
{

  /// 1. Assert log file descriptor is not null
  assert (log_fd != NULL);

  /// 2. Allocate buffer to hold message to log
  char buf[1024] = { 0 };

  /// 3. Read formatted user message string into the buffer
  va_list ap;
  va_start(ap, fmt);
  vsprintf (buf, fmt, ap);
  va_end(ap);

  /**
   * 4. If tag is a result of a system call and current log level is more
   * than DEBUG, print the message and return. Eg - PASS / FAIL etc
   */
  if (tag == RESULT && LOG_LEVEL >= DEBUG)
    {
      fprintf (log_fd, "%s", buf);
      fflush (log_fd);
      return;
    }

  /**
   * 5. If log message is less than current log level, print message with
   * current date, time to log file descriptor. Eg.
   *
   * ```
   * [05/02/2021 20:57:58] [DEBUG]   main() [source_fd] access('input/hello.opl', R_OK) - PASS
   * ```
   */
  if (tag <= LOG_LEVEL)
    {
      fprintf (log_fd, "\n[%s %s] %10s:%4d %14s() %s", __DATE__, __TIME__,
               file, line, func, buf);
    }

  /// 6. Flush message to log file
  fflush (log_fd);
}

/**
 * @brief       Print banner with stars above and below given string in DEBUG mode
 *
 * @details     Helper function to show something prominently in the logs Eg:
 *
 *  ```
 *  [02/04/2021 20:57:58] [DEBUG]         banner()
 *  [02/04/2021 20:57:58] [DEBUG]         banner() ***************************************************************
 *  [02/04/2021 20:57:58] [DEBUG]         banner() MARC start.
 *  [02/04/2021 20:57:58] [DEBUG]         banner() ***************************************************************
 *  [02/04/2021 20:57:58] [DEBUG]         banner()
 *  ```
 *
 * @param[in]   msg     String to print
 *
 * @return      None
 *
 */

void
banner (const char *msg)
{
  /// Create buffer of 64 characters size and fill with 63 stars
  char stars[64] = { 0 };
  memset (stars, '*', 63 * sizeof(char));

  /// Call logger macro to print newline, 63 stars, string, 63 stars & newline
  logger (DEBUG, "");
  logger (DEBUG, "%s", stars);
  logger (DEBUG, msg);
  logger (DEBUG, "%s", stars);
  logger (DEBUG, "");
}

/**
 * @brief       Function to close all open resources before program exit
 *
 * @param[in]   code     Exit code to return
 *
 * @return      The error return code of the function.
 *
 * @retval      code
 * @retval      errno           On system call failure
 *
 */

short
opal_exit (short code)
{

  logger (DEBUG, "=== START ===");
  logger (DEBUG, "Exit program with code: %d", code);

  /// Flush stdout
  sprintf (perror_msg, "fflush(stdout)");
  logger (DEBUG, perror_msg);
  if (fflush (stdout) == EXIT_SUCCESS)
    _PASS;
  else
    {
      _FAIL;
      perror (perror_msg);
      return (errno);
    }

  /// Close source file
  if (source_fd && source_fd != stdin)
    {
      sprintf (perror_msg, "fclose(source_fd)");
      logger (DEBUG, perror_msg);
      if (fclose (source_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
          return (errno);
        }
    }

  /// Flush and close destination file
  if (dest_fd && dest_fd != stdout)
    {
      sprintf (perror_msg, "fflush(dest_fd)");
      logger (DEBUG, perror_msg);
      if (fflush (dest_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
          return (errno);
        }

      sprintf (perror_msg, "fclose(dest_fd)");
      logger (DEBUG, perror_msg);
      if (fclose (dest_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
          return (errno);
        }
    }

  /// Flush and close log file
  if (log_fd && log_fd != stdout)
    {
      logger (DEBUG, "=== END ===");

      sprintf (perror_msg, "fflush(log_fd)");
      logger (DEBUG, perror_msg);
      if (fflush (log_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
          return (errno);
        }

      sprintf (perror_msg, "fclose(log_fd)");
      logger (DEBUG, perror_msg);
      logger (DEBUG, "\n");
      if (fclose (log_fd) != EXIT_SUCCESS)
        {
          perror (perror_msg);
          return (errno);
        }
    }
  else
    logger (DEBUG, "=== END ===\n\n");

  return (code);
}

/*
 * ==================================
 * END COMMON FUNCTION DEFINITIONS
 * ==================================
 */

/*
 * ==================================
 * MARC FUNCTION DEFINITIONS
 * ==================================
 */
/**
 * @brief       Function to read from source, remove comments, and write to destination
 *
 * @param[in]   source_fd     Source to be read from
 * @param[in]   dest_fd       Destination to written to
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 *
 */
short
rem_comments (FILE *source_fd, FILE *dest_fd)
{
  logger(DEBUG, "=== START ===");

  char ch = 0;
  char charNext = 0;
  bool isComment = false;
  int numComments = 0;

  /// Start reading characters from file
  while ((ch = fgetc (source_fd)) != EOF)
    {
      /// If character is not a /, line is not a comment
      if (ch != '/')
        {
          fputc (ch, dest_fd);
          continue;
        }
      /// If character is a /, read next character
      else
        charNext = fgetc (source_fd);

      /// If next character is / or *, line is a comment, set flag
      if (charNext == '/' || charNext == '*')
        {
          isComment = true;
          logger(DEBUG, "Start of comment");
        }
      /// else, not a comment, write both characters to file
      else
        {
          fputc (ch, dest_fd);
          fputc (charNext, dest_fd);
        }

      /// If comment flag is set, process comment
      while (isComment)
        {
          /// Read next character from file
          ch = fgetc (source_fd);

          /// If next character is /, process single line comment
          if (charNext == '/' && (ch == '\n' || ch == EOF))
            {
              logger(DEBUG, "End of comment (single line)");
              isComment = false;
              numComments++;
              charNext = 0;
            }

          /// If next character is *, process multi-line comment
          if (charNext == '*' && ch == '*')
            {
              ch = fgetc (source_fd);
              if (ch == '\n')
                numComments++;

              if (ch == '/')
                {
                  logger(DEBUG, "End of comment (multi-line)");
                  isComment = false;
                  charNext = 0;
                }
            }

          /// If end of file in multi-line comment return EXIT_FAILURE
          if (isComment && ch == EOF)
            {
              fprintf(stderr, "Invalid end of file in comment");
              return EXIT_FAILURE;
            }
        }
    }

  logger(DEBUG, "Removed %d comment lines", numComments);
  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/// Read source, process includes, write to destination
short
proc_includes(FILE *source_fd, FILE *dest_fd)
{
    ///Move source_fd to beginning of file.
    fseek(source_fd, 0, SEEK_SET);

    ///Copy each character to the destination file, while checking for include files.
    logger(DEBUG, "Reading file.");
    char ch = fgetc(source_fd);
    while (ch != EOF){
        switch(ch) {
            case ('EOF'): {
                logger(DEBUG, "End of file found, done reading file.");
                break;
            }
            case ('#'): {
                logger(DEBUG, "Found hashtag symbol.");

                ///Reads in 8 chars to check if they are "include ".
                char include_buffer[9] = {0};
                ssize_t sz = fread (include_buffer, sizeof(char), sizeof(char) * 8, source_fd);

                ///Rewinds the file pointer.
                fseek (source_fd, -sz, SEEK_CUR);

                ///If include is found, process the included file.
                if (strcasecmp(include_buffer, "include ") == 0) {
                    logger(DEBUG, "Include keyword has been found.");

                    ///Move file pointer to the point after "include "
                    fseek (source_fd, sz, SEEK_CUR);
                    char filename_buffer[256] = {0};
                    int filename_len = 0;

                    ///Get the filename for the include file.
                    while (ch != '\n' && filename_len < 256){
                        if (ch != '"' && ch != '#')
                            filename_buffer[filename_len++] = ch;
                        ch = fgetc(source_fd);
                    }
                    logger(DEBUG, "Finished reading in the filename.");

                    ///Open file and get the first character in it.
                    FILE *include_file = fopen(filename_buffer, "r");
                    char ch_2 = fgetc(include_file);

                    if (include_file != NULL)
                        logger(DEBUG, "Successfully opened the include file.");
                    else
                        logger(DEBUG, "Failed to open the include file.");

                    ///Move contents of include file into destination file
                    while (ch_2 != EOF) {
                        fputc(ch_2, dest_fd);
                        ch_2 = fgetc(include_file);
                    }
                    fclose(include_file);
                }
                continue;
            }
            default: {
                fputc (ch, dest_fd);
            }
        }
        ch = fgetc(source_fd);
    }
    _DONE;

    return EXIT_SUCCESS;
}
