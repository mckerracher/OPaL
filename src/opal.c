/// @file opal.c
/// @authors Damle Kedar, Mckerracher Joshua, Leon Sarah Louise
///

#include "../include/opal.h"

#include <assert.h>             /* assert() */
#include <ctype.h>              /* isspace(), isalnum() */
#include <errno.h>              /* errno macros and codes */
#include <regex.h> 				/* ReGex functions */
#include <stdarg.h>             /* variadic functions */
#include <stdio.h>
#include <stdlib.h>             /* fopen, fclose, exit() */
#include <string.h>             /* memset() */
#include <strings.h>
#include <unistd.h>
#include <libgen.h>             /* basename(), dirname() */

/*
 * ==================================
 * START COMMON FUNCTION DEFINITIONS
 * ==================================
 */

/**
 * @brief       Print formatted message to log file
 *
 * @details     Helper function to log messages. Function writes to global
 * variable log_fp. Usually called by a macro logger. Eg:
 *
 * ```
 * logger (ERROR, "Cannot read file: %s", file_name);
 * logger (DEBUG, "access('%s', F_OK)", source_fn);
 * ```
 *
 * @param[in]   tag     Log level of message
 * @param[in]   file    Source file name
 * @param[in]   line    Source file line number
 * @param[in]   func    Source file function
 * @param[in]   fmt     Formatted message to log
 *
 * @return      None
 *
 */
void
opal_log (log_level_e tag, const char *file, int line, const char *func,
          const char *fmt, ...)
{

  /// Assert log file pointer is not null
  assert(log_fp);

  /// Allocate buffer to hold message to log
  char buf[4096] = { 0 };

  /// Read formatted user message string into the buffer
  va_list ap;
  va_start(ap, fmt);
  vsprintf (buf, fmt, ap);
  va_end(ap);

  /**
   * If tag is a result of a system call and current log level is more
   * than DEBUG, print the message and return. Eg - PASS / FAIL etc
   */
  if (tag == RESULT && LOG_LEVEL >= DEBUG)
    {
      retVal = fprintf (log_fp, "%s", buf);
      if (retVal < 0)
        opal_exit(retVal);

      if (fflush (log_fp) != EXIT_SUCCESS)
        {
          perror("fflush (log_fp)");
          opal_exit(errno);
        }
      return;
    }

  /**
   * If log message is less than current log level, print message with
   * current date, time to log file pointer. Eg.
   *
   * ```
   * [05/02/2021 20:57:58] [DEBUG]   main() [source_fp] access('input/hello.opl', R_OK) - PASS
   * ```
   */
  if (tag <= LOG_LEVEL)
    {
      fprintf (log_fp, "\n[%10s:%4d] %24s() %s", file, line, func, buf);
    }

  /// Flush message to log file
  if (fflush (log_fp) != EXIT_SUCCESS)
    {
      perror("fflush (log_fp)");
      opal_exit(errno);
    }
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
  logger(DEBUG, "");
  logger(DEBUG, "%s", stars);
  logger(DEBUG, msg);
  logger(DEBUG, "%s", stars);
  logger(DEBUG, "");
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

  logger(DEBUG, "=== START ===");
  logger(DEBUG, "Exit program with code: %d", code);

  /// Flush stdout
  sprintf (perror_msg, "fflush(stdout)");
  logger(DEBUG, perror_msg);
  if (fflush (stdout) == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Close source file
  if (source_fp && source_fp != stdin)
    {
      sprintf (perror_msg, "fclose(source_fp)");
      logger(DEBUG, perror_msg);
      if (fclose (source_fp) == EXIT_SUCCESS)
        {
          _PASS;
          source_fp = NULL;
        }
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  if (source_fn)
    {
      logger(DEBUG, "free (source_fn)");
      free (source_fn);
      source_fn = NULL;
    }

  /// Flush and close destination file
  if (dest_fp && dest_fp != stdout)
    {
      sprintf (perror_msg, "fflush(dest_fp)");
      logger(DEBUG, perror_msg);
      if (fflush (dest_fp) == EXIT_SUCCESS)
        _PASS;
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }

      sprintf (perror_msg, "fclose(dest_fp)");
      logger(DEBUG, perror_msg);
      if (fclose (dest_fp) == EXIT_SUCCESS)
        {
          _PASS;
          dest_fp = NULL;
        }
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  if (dest_fn)
    {
      logger(DEBUG, "free (dest_fn)");
      free (dest_fn);
      dest_fn = NULL;
    }

  /// Flush and close report file
  if (report_fp)
    {
      sprintf (perror_msg, "fflush(report_fp)");
      logger(DEBUG, perror_msg);
      if (fflush (report_fp) == EXIT_SUCCESS)
        _PASS;
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }

      sprintf (perror_msg, "fclose(report_fp)");
      logger(DEBUG, perror_msg);
      if (fclose (report_fp) == EXIT_SUCCESS)
        {
          _PASS;
          report_fp = NULL;
        }
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  if (report_fn)
    {
      logger(DEBUG, "free(report_fn)");
      free (report_fn);
      report_fn = NULL;
    }

  /// Flush and close log file
  if (log_fp && log_fp != stdout)
    {

      sprintf (perror_msg, "fflush(log_fp)");
      logger(DEBUG, perror_msg);
      if (fflush (log_fp) == EXIT_SUCCESS)
        _PASS;
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }

      sprintf (perror_msg, "fclose(log_fp)");
      logger(DEBUG, perror_msg);
      logger(DEBUG, "=== END ===");
      logger(DEBUG, "\n");
      if (fclose (log_fp) != EXIT_SUCCESS)
        {
          perror (perror_msg);
          return (errno);
        }
      log_fp = NULL;
    }
  else
    logger(DEBUG, "=== END ===\n\n");

  if (log_fn)
    {
      free (log_fn);
      log_fn = NULL;
    }

  return (code);
}

/**
 * @brief       Function to call opal_exit and logger functions.
 *
 * @param[in]   exit_code       Exit code to return
 * @param[in]   *log_msg        Logging message
 * @param[in]   fmt_option      Must be 1 if a formatted string is used
 * @param[in]   fmt             Formatted message to log
 *
 * @return      Calls opal_exit with exit_code
 *
 * @retval      Function call to opal_exit
 */
short
opal_error (short exit_code, char *log_msg, int fmt_option, char *fmt, ...)
{
    if (fmt_option == 1)
    {
        /// Allocate buffer to hold message to log
        char buf[4096] = { 0 };

        /// Read formatted user message string into the buffer
        va_list ap;
        va_start(ap, fmt);
        vsprintf (buf, fmt, ap);
        va_end(ap);

        logger (ERROR, buf);
    }
    else
    {
        logger (ERROR, log_msg);
    }
    fprintf(stderr, "%s", log_msg);
    return opal_exit(exit_code);
}

/**
 * @brief       Function to read next character from the source file pointer
 *
 * @return      Character read
 *
 * @retval      Next character read from FILE *stream source_fp
 * @retval      errno           On system call failure
 *
 */
int
read_next_char (void)
{
  /// Read character from source file pointer
  errno = EXIT_SUCCESS;
  next_char = getc (source_fp);

  /// getc() sets the errno in the event of an error
  if (errno != EXIT_SUCCESS)
    {
      perror (perror_msg);
      exit (opal_exit (errno));
    }

  /// Increment the column number of the character
  ++char_col;

  /// If character is a newline, increment line number and reset column number
  if (next_char == '\n')
    {
      ++char_line;
      char_col = 0;
    }

  /// Return the character read
  return next_char;
}

/**
 * @brief   Initialize HTML report file
 *
 * @param[in,out] report_fp    Report file pointer
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
init_report (FILE *report_fp)
{

  logger(DEBUG, "=== START ===");

  /// Assert report file pointer is not NULL
  assert(report_fp);

  /// Write HTML head tag to the report
  logger(DEBUG, "Writing HTML head tag to report");
  fprintf (report_fp, "<!DOCTYPE html>\n"
           "<html>\n"
           "<head>\n"
           "<title>OPaL compilation report</title>\n"
           "<style>\n");

  /// Open res/styles.css in read-only mode
  sprintf (perror_msg, "css_fp = fopen ('%s', 'r')", css_fn);
  logger (DEBUG, perror_msg);

  errno = EXIT_SUCCESS;
  FILE *css_fp = fopen (css_fn, "r");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      exit (opal_exit(errno));
    }

  /// Copy CSS to HTML report
  logger (DEBUG, "Copying CSS to HTML report");
  char ch = 0;
  while ((ch = fgetc (css_fp)) != EOF)
    fputc (ch, report_fp);
  _DONE;

  /// Close res/styles.css file
  sprintf (perror_msg, "fclose(css_fp)");
  logger (DEBUG, perror_msg);
  if (fclose (css_fp) == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      exit (opal_exit (errno));
    }

  fprintf(report_fp,"</style>\n"
           "</head>\n");

  /// Start HTML body tag
  fprintf (report_fp, "<body>\n");

  /// Open textarea tag for source file
  fprintf (report_fp, "<h2>Compilation steps report </h2>\n"
           "<h3>Original source file: <code>%s</code></h3>\n<hr>\n"
           "<textarea style='resize: none;' readonly rows='25' cols='80'>\n",
           source_fn);

  /// Append source file to HTML report and close textarea tag
  ch = 0;
  logger(DEBUG, "Copying source file to HTML report");

  while ((ch = fgetc (source_fp)) != EOF)
    fputc (ch, report_fp);

  _DONE;

  fprintf (report_fp, "\n</textarea>\n");
  fflush (report_fp);

  /// Rewind source file pointer
  sprintf (perror_msg, "rewind('%s')", source_fn);
  logger(DEBUG, perror_msg);
  rewind (source_fp);

  /// If current value of source file position not 0, print error and exit
  if (ftell (source_fp) == 0)
      _DONE;
  else
    {
      perror (perror_msg);
      _FAIL;
      exit (opal_exit(errno));
    }

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief   Close HTML report file
 *
 * @param[in,out] report_fp    Report file pointer
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
close_report (FILE *report_fp)
{

  logger(DEBUG, "=== START ===");

  /// Assert report file pointer is not NULL
  assert(report_fp);

  /// Write HTML closing tags to the report
  logger(DEBUG, "Writing HTML closing tags to report");
  fprintf (report_fp, "\n</body></html>");
  fflush (report_fp);

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/*
 * ==================================
 * END COMMON FUNCTION DEFINITIONS
 * ==================================
 */

/*
 * ==================================
 * START MARC FUNCTION DEFINITIONS
 * ==================================
 */

/**
 * @brief       Function to read from source, remove comments, and write to destination
 *
 * @param[in]   source_fp     Source to be read from
 * @param[in]   dest_fp       Destination to written to
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
rem_comments (FILE *source_fp, FILE *dest_fp)
{
  logger(DEBUG, "=== START ===");

  /// Check if source file pointer is not NULL
  logger(DEBUG, "assert(source_fp)");
  assert(source_fp);
  _PASS;

  /// Check if destination file pointer is not NULL
  logger(DEBUG, "assert(dest_fp)");
  assert(dest_fp);
  _PASS;

  char ch = 0;
  char charNext = 0;
  bool isComment = false;
  int numComments = 0;

  /// Start reading characters from file
  while ((ch = fgetc (source_fp)) != EOF)
    {
      /// If character is not a /, line is not a comment
      if (ch != '/')
        {
          fputc (ch, dest_fp);
          continue;
        }
      /// If character is a /, read next character
      else
        charNext = fgetc (source_fp);

      /// If next character is / or *, line is a comment, set flag
      if (charNext == '/' || charNext == '*')
        {
          isComment = true;
          logger(DEBUG, "Start of comment");
        }
      /// else, not a comment, write both characters to file
      else
        {
          fputc (ch, dest_fp);
          fputc (charNext, dest_fp);
        }

      /// If comment flag is set, process comment
      while (isComment)
        {
          /// Read next character from file
          ch = fgetc (source_fp);

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
              ch = fgetc (source_fp);
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
              fprintf (stderr, "Invalid end of file in comment");
              return EXIT_FAILURE;
            }

          /// If char is a newline, write to file to preserve line numbers
          if (ch == '\n')
              fputc (ch, dest_fp);

        }
    }

  logger(DEBUG, "Removed %d comment lines", numComments);
  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief       Read source, process includes, write to destination
 *
 * @param[in]   source_fp     Source to be read from
 * @param[in]   dest_fp       Destination to written to
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
proc_includes (FILE *source_fp, FILE *dest_fp)
{
  logger(DEBUG, "=== START ===");

  /// Assert source file pointer is not NULL
  logger(DEBUG, "assert(source_fp)");
  assert(source_fp);
  _PASS;

  /// Assert destination file pointer is not NULL
  logger(DEBUG, "assert(dest_fp)");
  assert(dest_fp);
  _PASS;

  /// Move source_fp to beginning of file.
  sprintf (perror_msg, "fseek (source_fp, 0, SEEK_SET)");
  logger(DEBUG, perror_msg);
  fseek (source_fp, 0, SEEK_SET);

  /// If source file position not 0, print error and exit
  if (ftell (source_fp) == 0)
    _DONE;
  else
    {
      perror (perror_msg);
      _FAIL;
      exit (opal_exit(errno));
    }

  /// Copy each character to the destination file, while checking for include files.
  logger(DEBUG, "Reading file.");
  char ch = fgetc (source_fp);
  while (ch != EOF)
    {
      switch (ch)
        {
        case EOF:
          {
            _DONE;
            break;
          }
        case ('#'):
          {
            logger(DEBUG, "Found hashtag symbol.");

            ///Reads in 8 chars to check if they are "include ".
            char include_buffer[9] = { 0 };
            ssize_t sz = fread (include_buffer, sizeof(char), sizeof(char) * 8,
                                source_fp);

            /// Rewinds the file pointer.
            fseek (source_fp, -sz, SEEK_CUR);

            /// If include is found, process the included file.
            if (strcasecmp (include_buffer, "include ") == 0)
              {
                logger(DEBUG, "Include keyword has been found.");

                /// Move file pointer to the point after "include "
                fseek (source_fp, sz, SEEK_CUR);
                char filename_buffer[256] = { 0 };
                int filename_len = 0;

                /// Get the filename for the include file.
                ch = fgetc (source_fp);
                while (ch != '\n' && filename_len < 256)
                  {
                    if (ch != '"')
                      filename_buffer[filename_len++] = ch;
                    ch = fgetc (source_fp);
                  }
                logger(DEBUG, "Finished reading in the filename.");

                char *include_basename = basename (filename_buffer);
                char include_fn[512] = { 0 };
                FILE *include_fp = NULL;

                /// If given file name is relative path, prefix source file dir
                if (strcmp (filename_buffer, include_basename) == 0)
                  {
                    /// Get source file directory
                    char *source_dir = dirname (source_fn);
                    logger(DEBUG, "source_dir: %s", source_dir);
                    sprintf (include_fn, "%s/%s", source_dir, include_basename);
                  }
                else
                  sprintf (include_fn, "%s", filename_buffer);

                logger(DEBUG, "include_fn: %s", include_fn);

                /// If include file does not exist, print error and exit
                sprintf (perror_msg, "access('%s', F_OK)", include_fn);
                logger(DEBUG, perror_msg);
                if (access (include_fn, F_OK) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    perror (perror_msg);
                    _FAIL;
                    return (errno);
                  }

                /// If include file can not be read, print error and exit
                sprintf (perror_msg, "access('%s', R_OK)", include_fn);
                logger(DEBUG, perror_msg);
                if (access (include_fn, R_OK) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    perror (perror_msg);
                    _FAIL;
                    return (errno);
                  }

                /// Open include file in read-only mode
                sprintf (perror_msg, "include_fp = fopen('%s', 'r')",
                         include_fn);
                logger(DEBUG, perror_msg);

                errno = EXIT_SUCCESS;
                include_fp = fopen (include_fn, "r");
                if (errno == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    perror (perror_msg);
                    _FAIL;
                    return (errno);
                  }

                char ch_2 = fgetc (include_fp);

                /// Move contents of include file into destination file
                logger(DEBUG, "Copy contents of %s into destination file",
                       include_fn);
                while (ch_2 != EOF)
                  {
                    fputc (ch_2, dest_fp);
                    ch_2 = fgetc (include_fp);
                  }
                _DONE;

                /// Flush destination file contents to disk
                sprintf (perror_msg, "fflush(dest_fp)");
                logger(DEBUG, perror_msg);
                if (fflush (dest_fp) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    perror (perror_msg);
                    _FAIL;
                    return (errno);
                  }

                /// Close include file pointer
                sprintf (perror_msg, "fclose (include_fp)");
                logger(DEBUG, perror_msg);
                if (fclose (include_fp) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    perror (perror_msg);
                    _FAIL;
                    return (errno);
                  }
              }
            continue;
          }
        default:
          {
            fputc (ch, dest_fp);
          }
        }
      /// Gets the next char for the switch case to evaluate.
      ch = fgetc (source_fp);
    }

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief       Append MARC output to HTML report file
 *
 * @param[in]   source_fp     Source to be read from
 * @param[in]   report_fp       Destination to written to
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
print_marc_html(FILE *source_fp, FILE *report_fp)
{
  logger(DEBUG, "=== START ===");

  /// Assert source file pointer is not NULL
  logger(DEBUG, "assert(source_fp)");
  assert(source_fp);
  _PASS;

  /// Assert destination file pointer is not NULL
  logger(DEBUG, "assert(report_fp)");
  assert(report_fp);
  _PASS;

  /// Open textarea tag in report file for MARC output
  fprintf (report_fp, "<h3>Output by pre-processor <code>MARC</code></h3>\n"
           "<hr>\n"
           "<textarea style='resize: none;' readonly rows='25' cols='80'>\n");

  /// Append MARC output file to report file
  logger (DEBUG, "Copying MARC output to HTML report");
  char ch = 0;
  while ((ch = fgetc (source_fp)) != EOF)
    fputc (ch, report_fp);
  _DONE;

  fprintf (report_fp, "\n</textarea>\n");

  /// Flush contents of report to disk
  sprintf (perror_msg, "fflush(report_fp)");
  logger(DEBUG, perror_msg);
  if (fflush (report_fp) == EXIT_SUCCESS)
    _PASS;
  else
    {
      _FAIL;
      perror (perror_msg);
      return (errno);
    }

  return EXIT_SUCCESS;
}

/*
 * ==================================
 * END MARC FUNCTION DEFINITIONS
 * ==================================
 */

/*
 * ==================================
 * START ALEX FUNCTION DEFINITIONS
 * ==================================
 */

/**
 * @brief       Get lexeme for a string literal
 *
 * @param[in]   char_line      line number of char in source file
 * @param[in]   char_col       column number of char in source file
 *
 * @return      Next lexeme struct with values populated
 *
 * @retval      struct lexeme *
 */
lexeme_s
get_string_literal_lexeme (int char_line, int char_col)
{
  /// Initialize the string
  char string[256] = { 0 };

  int index = 0;

  /// The next char needs to be checked, so get it.
  read_next_char ();

  while (next_char != '"')
    {
      if ((next_char == EOF) || (next_char == '\n'))
        logger(ERROR, "[%d:%d] Illegal End of file.", char_line, char_col);
      else
        string[index++] = next_char;

      read_next_char ();
    }

  read_next_char ();

  lexeme_s retVal =
    {
      .type = lx_String,
      .line = char_line,
      .column = char_col,
      .int_val = 0,
      .char_val = strdup(string)
    };

  return retVal;
}

/**
 * @brief       Get lexeme for binary or unary operator
 *
 * @return      Next lexeme struct with values populated
 *
 * @retval      enum lexeme type
 *
 */
lexeme_type_e
binary_unary (char compound_char, lexeme_type_e compound_type,
              lexeme_type_e simple_type, int char_line, int char_col)
{
  /// Initialize return variable.
  lexeme_type_e retVal = lx_NOP;

  /// The next char needs to be checked, so get it.
  read_next_char ();

  if (next_char == EOF)
    {
      /// Illegal character found.
      logger(ERROR, "[%d:%d] Illegal End of file.", char_line, char_col);
      opal_exit(EXIT_FAILURE);
    }
  else if (next_char == compound_char)
    {
      /// Compound type found, so get the next char and return compound_type.
      read_next_char ();
      retVal = compound_type;
    }
  else
    /// Compound type not found, so return simple_type.
    retVal = simple_type;

  return retVal;
}

/**
 * @brief       Get lexeme for char / integer identifier
 *
 * @return      Lexeme with values populated
 *
 * @retval      struct lexeme
 *
 */
lexeme_s
get_identifier_lexeme (int char_line, int char_col)
{
  lexeme_s retVal = { 0 };
  retVal.line = char_line;
  retVal.column = char_col;
  char identifier_str[1024] = { 0 };
  int str_len = 0;
  bool regex_match = false;

  /// Get string to analyze
  while (isalnum(next_char) || next_char == '_')
    {
      identifier_str[str_len++] = next_char;
      read_next_char ();
    }

  /// Terminate string
  identifier_str[str_len++] = '\0';

  /// Error for unsupported characters
  if (str_len == 1)
    {
      fprintf (stderr, "[%d: %d] Invalid identifier: %c.", char_line, char_col,
               next_char);
      exit (opal_exit(EXIT_FAILURE));
    }

  /// Determine if string is a reserved keyword
  int i = 0;
  for (i = 0; i < (sizeof(keyword_arr) / sizeof(keyword_arr[0])); i++)
    {
      if (strcmp (identifier_str, keyword_arr[i].str) == 0)
        {
          retVal.type = keyword_arr[i].lex_type;
          return retVal;
        }
    }

  /// Determine if the string is an integer via regex
  regex_match = match (identifier_str, int_regex_pattern);
  if (regex_match)
    {
      logger(DEBUG, "strtol (%s, NULL, 0)", identifier_str);

      errno = EXIT_SUCCESS;
      int intVal = strtol (identifier_str, NULL, 0);
      if (errno != EXIT_SUCCESS)
        {
          perror (identifier_str);
          _FAIL;
          exit (opal_exit(EXIT_FAILURE));
        }
      else
        {
          _PASS;
          retVal.type = lx_Integer;
          retVal.int_val = intVal;
        }
    }
  else
    {
      /// String must be an identifier
      retVal.type = lx_Ident;
      retVal.char_val = strdup(identifier_str);
    }

  return retVal;
}

/**
 * @brief       Matches a string against a regular expression pattern.
 *              From pubs.opengroup.org/onlinepubs/007904875/functions/regcomp.html
 *
 * @param[in]   str             String to match
 * @param[in]   pattern         Extended regular expression
 *
 * @return      The result of the pattern match.
 *
 * @retval      true(1)         On match
 * @retval      false(0)        On error
 *
 */
bool
match (const char *str, const char *pattern)
{
  int status;
  regex_t regEx;

  ///Ensure regex is set up properly
  if (regcomp (&regEx, pattern, REG_EXTENDED | REG_NOSUB) != EXIT_SUCCESS)
    return false;

  ///Process string against regex pattern
  status = regexec (&regEx, str, (size_t) 0, NULL, 0);

  regfree (&regEx);

  if (status != EXIT_SUCCESS)
    return false;

  return true;
}

/**
 * @brief       Get the next lexeme based on the next character
 *
 * @return      Next lexeme struct with values populated
 *
 * @retval      struct lexeme
 *
 */
lexeme_s
get_next_lexeme (void)
{

  /// Create a empty struct to populate and return
  lexeme_s retVal = { 0 };

  /// Call read_next_char() to get the next character from source
  while (isspace(next_char))
    read_next_char ();

  /// Populate lexeme line and column number
  retVal.line = char_line;
  retVal.column = char_col;

  /// Get the lexeme type based on the next character
  switch (next_char)
    {
    case '{':
      retVal.type = lx_Lbrace;
      break;
    case '}':
      retVal.type = lx_Rbrace;
      break;
    case '(':
      retVal.type = lx_Lparen;
      break;
    case ')':
      retVal.type = lx_Rparen;
      break;
    case '/':
      retVal.type = lx_Div;
      break;
    case '*':
      retVal.type = lx_Mul;
      break;
    case '%':
      retVal.type = lx_Mod;
      break;
    case ';':
      retVal.type = lx_Semi;
      break;
    case ',':
      retVal.type = lx_Comma;
      break;
    case '+':
      retVal.type = lx_Add;
      break;
    case '-':
      retVal.type = lx_Sub;
      break;
    case '<':
      retVal.type = binary_unary ('=', lx_Leq, lx_Lss, char_line, char_col);
      return retVal;
    case '>':
      retVal.type = binary_unary ('=', lx_Geq, lx_Gtr, char_line, char_col);
      return retVal;
    case '=':
      retVal.type = binary_unary ('=', lx_Eq, lx_Assign, char_line, char_col);
      return retVal;
    case '!':
      retVal.type = binary_unary ('=', lx_Neq, lx_Not, char_line, char_col);
      return retVal;
    case '&':
      retVal.type = binary_unary ('&', lx_And, lx_EOF, char_line, char_col);
      return retVal;
    case '|':
      retVal.type = binary_unary ('|', lx_Or, lx_EOF, char_line, char_col);
      return retVal;
    case '"':
      return get_string_literal_lexeme (char_line, char_col);
    case EOF:
      retVal.type = lx_EOF;
      break;
    default:
      return get_identifier_lexeme (char_line, char_col);
    }

  read_next_char ();
  return retVal;
}

/**
 * @brief       Get lexeme string format
 *
 * @param[in]       lexeme    Lexeme to stringify
 * @param[in,out]   buffer      Buffer to store string value of lexeme
 * @param[in]       buffer_len  Length of buffer to use
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
get_lexeme_str (const lexeme_s *lexeme, char *buffer, const int buffer_len)
{

  /// Assert buffer is not NULL
  assert(buffer);

  /// Empty out the string buffer
  memset (buffer, 0, buffer_len * sizeof(char));

  /// Populate the buffer with values from the struct
  sprintf (
      buffer,
      "line: %3d, column: %3d, type: %16s, int_val: %6d, char_val: '%s'",
      lexeme->line, lexeme->column, op_name[lexeme->type],
      lexeme->int_val, lexeme->char_val ? lexeme->char_val : "");

  return EXIT_SUCCESS;
}

/**
 * @brief       Populate symbol table with lexemes in source file pointer
 *
 * @param[in,out]   *symbol_table    Symbol table linked list to populate
 * @param[in,out]   *symbol_count    Pointer to count of lexemes found
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
build_symbol_table (lexeme_s *symbol_table, int *symbol_count)
{
  logger(DEBUG, "=== START ===");

  /// Assert symbol table pointer is not NULL
  logger(DEBUG, "assert(symbol_table)");
  assert(symbol_table);
  _PASS;

  /// Assert symbol table count pointer is not NULL
  logger(DEBUG, "assert(symbol_count)");
  assert(symbol_count);
  _PASS;

  /// Create symbols at the beginning of the linked list
  lexeme_s *current = symbol_table;

  /// Get lexemes in a loop until we get a EOF lexeme
  do
    {
      /// Call get_next_lexeme() to populate next_lexeme
      next_lexeme = get_next_lexeme ();

      /// Append next_lexeme to symbol table
      lexeme_s *new_symbol = (lexeme_s*) calloc (1, sizeof(lexeme_s));
      new_symbol->line = next_lexeme.line;
      new_symbol->column = next_lexeme.column;
      new_symbol->type = next_lexeme.type;
      new_symbol->int_val = next_lexeme.int_val;

      new_symbol->char_val =
          next_lexeme.char_val ? strdup(next_lexeme.char_val) : NULL;

      /// Call get_lexeme_str() to stringify next_lexeme
      if (get_lexeme_str (new_symbol, lexeme_str,
                          lexeme_str_len) != EXIT_SUCCESS)
        return (EXIT_FAILURE);

      /// Append lexeme to symbol table
      logger(DEBUG, "Append lexeme {%s}", lexeme_str);
      current->next = new_symbol;

      /// Increment symbol count
      *symbol_count = *symbol_count + 1;

      /// Move current to last symbol in linked list
      current = current->next;
    }
  while (next_lexeme.type != lx_EOF);

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief       Print symbol table to destination file pointer
 *
 * @param[in,out]   symbol_table    Symbol table to print
 * @param[in,out]   dest_fp         Destination file pointer
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
print_symbol_table (lexeme_s *symbol_table, FILE *dest_fp)
{
  logger(DEBUG, "=== START ===");

  /// Assert symbol table pointer is not NULL
  logger(DEBUG, "assert(symbol_table)");
  assert(symbol_table);
  _PASS;

  /// Assert destination file pointer is not NULL
  logger(DEBUG, "assert(dest_fp)");
  assert(dest_fp);
  _PASS;

  /// Write ALEX to destination file
  logger (DEBUG, "Writing ALEX output to destination file.");

  lexeme_s *current = symbol_table;
  while (current->next)
    {
      /// Call get_lexeme_str() to stringify next_lexeme
      retVal = get_lexeme_str (current, lexeme_str,
                               lexeme_str_len);
      if (retVal != EXIT_SUCCESS)
        return (EXIT_FAILURE);

      /// Append lexeme to symbol table
      retVal = fprintf (dest_fp, "%s\n", lexeme_str);
      if (retVal < 0)
        {
          perror ("fprintf (dest_fp, next_lexeme_str)");
          exit (opal_exit (retVal));
        }

      current = current->next;
    }
  _DONE;

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief       Print symbol table HTML report to report file pointer
 *
 * @param[in,out]   symbol_table    Symbol table to print
 * @param[in,out]   report_fp       Report file pointer
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
print_symbol_table_html (lexeme_s *symbol_table, FILE *report_fp)
{
  logger(DEBUG, "=== START ===");

  /// Assert symbol table pointer is not NULL
  logger(DEBUG, "assert(symbol_table)");
  assert(symbol_table);
  _PASS;

  /// Assert destination file pointer is not NULL
  logger(DEBUG, "assert(report_fp)");
  assert(report_fp);
  _PASS;

  /// Walk the symbol table & print the HTML report to destination file pointer
  logger(DEBUG, "Walk symbol table and print lexemes to HTML report");

  fprintf (
      report_fp,
      "<h3>Symbol table by Lexical analyzer <code>ALEX</code></h3>\n<hr>\n");
  fprintf (report_fp,
           "<div class='scroll'><table>\n" "<tr>\n" "<th>Line No.</th>\n"
           "<th>Column No.</th>\n" "<th>Type</th>\n" "<th>Value</th>\n"
           "</tr>");

  /// Append symbol table to report file
  logger (DEBUG, "Copying ALEX output to HTML report");

  lexeme_s *current = symbol_table;
  while (current->next)
    {
      fprintf (report_fp, "<tr>");
      fprintf (report_fp, "<td>%d</td>\n"
               "<td>%d</td>\n"
               "<td>%s</td>\n",
               current->line, current->column,
               op_name[current->type]);

      if (current->type == lx_Integer)
        {
          fprintf (report_fp, "<td>%d</td>\n", current->int_val);
        }
      else if (current->type == lx_Ident)
        {
          fprintf (report_fp, "<td>%s</td>\n", current->char_val);
        }
      else if (current->type == lx_String)
        {
          fprintf (report_fp, "<td>\"%s\"</td>\n", current->char_val);
        }
      else
        {
          fprintf (report_fp, "<td></td>\n");
        }
      fprintf (report_fp, "</tr>\n");
      current = current->next;
    }

  fprintf (report_fp, "</table></div>\n");
  fflush (report_fp);

  _DONE;

  /// Flush contents of report to disk
  sprintf (perror_msg, "fflush(report_fp)");
  logger(DEBUG, perror_msg);
  if (fflush (report_fp) == EXIT_SUCCESS)
    _PASS;
  else
    {
      _FAIL;
      perror (perror_msg);
      return (errno);
    }

  _DONE;

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief       Free memory allocated for symbol table linked list
 *
 * @param[in,out]   symbol_table    Symbol table to deallocate
 *
 * @return      NULL
 *
 */
void
free_symbol_table (lexeme_s *symbol_table)
{
  logger(DEBUG, "=== START ===");
  /// Walk symbol table and free individual lexemes

  lexeme_s *next_symbol;
  while (symbol_table)
    {
      next_symbol = symbol_table;
      symbol_table = symbol_table->next;

      get_lexeme_str (next_symbol, lexeme_str,lexeme_str_len);
      logger(DEBUG, "Free symbol: %s", lexeme_str);
      free (next_symbol);
    }

  logger(DEBUG, "=== END ===");
}

/*
 * ==================================
 * END ALEX FUNCTION DEFINITIONS
 * ==================================
 */

/*
 * ==================================
 * START ASTRO FUNCTION DEFINITIONS
 * ==================================
 */

/**
 * @brief       Return syntax tree node with given left and right child nodes
 *
 * @param[in]   type            Node type to create
 * @param[in]   left_child      Left child node pointer
 * @param[in]   right_child     Right child node pointer
 *
 * @return      Abstract syntax tree node pointer
 *
 * @retval      node_s*     On success
 */
node_s*
make_ast_node(ast_node_type_e type, node_s *left_child, node_s *right_child)
{

  /// Create node with given children and return
  node_s *tree = calloc (1, sizeof(node_s));
  tree->left = left_child;
  tree->right = right_child;
  tree->node_type = type;

  /// Create buffer for logging
  char buffer[1024] = { 0 };

  /// Append left child node type to log buffer
  strcat (buffer, "left->");
  if (left_child)
    strcat (buffer, node_name[left_child->node_type]);
  else
    strcat (buffer, "NULL");

  /// Append current node type to log buffer
  strcat (buffer, "\tnode->");
  strcat (buffer, node_name[type]);

  /// Append right child node type to log buffer
  strcat (buffer, "\tright->");
  if (right_child)
    strcat (buffer, node_name[right_child->node_type]);
  else
    strcat (buffer, "NULL");

  /// Log node message
  logger (DEBUG, buffer);

  return tree;
}

/**
 * @brief       Build abstract syntax tree from symbol table
 *
 * @param       symbol_table       Lexeme symbol table
 *
 * @return      Abstract syntax tree built from the symbol table
 *
 * @retval      Tree root node pointer      On success
 * @retval      NULL                        On error
 *
 */
node_s*
build_syntax_tree (lexeme_s *symbol_table)
{
  logger(DEBUG, "=== START ===");

  /// Check if symbol table pointer is not NULL
  logger(DEBUG, "assert(symbol_table)");
  assert(symbol_table);
  _PASS;

  /// Create syntax tree node NULL pointer to return
  node_s *tree = NULL;

  /// Start reading lexemes from the symbol table
  ast_curr_lexeme = symbol_table;

  /// Call make_ast_node() until lexeme with lx_EOF is seen
  do {
      tree = make_ast_node(nd_Sequence, tree, make_statement_node());
  } while (tree != NULL && ast_curr_lexeme->type != lx_EOF);

  logger(DEBUG, "=== END ===");
  return tree;
}

/**
 * @brief       Check if lexeme is of expected type, else print error and exit
 *
 * @param[in]   expected_type   Expected lexeme type
 *
 * @return      NULL
 */
void
expect_lexeme (lexeme_type_e expected_type)
{
  /// If ast_curr_lexeme is of expected type
  if (ast_curr_lexeme->type == expected_type)
    {
      /// ... read next lexeme and return
      ast_curr_lexeme = ast_curr_lexeme->next;
      return;
    }

  /// ... else print error and exit
  fprintf(stderr, "%s expected but %s found.", grammar[expected_type].text,
         grammar[ast_curr_lexeme->type].text);
  exit (opal_exit (EXIT_FAILURE));
}

/**
 * @brief       Build and return expression inside parantheses
 *
 * @return      Syntax tree node pointer
 *
 * @retval      node_s*     On success
 * @retval      NULL        On error
 *
 */
node_s*
make_parentheses_expression(void)
{
  /// Expect left parantheses before the expression
  expect_lexeme (lx_Lparen);

  ///
  node_s *tree = NULL;

  /// Create tree for expression inside parantheses
  tree = make_expression_node (0);

  /// Expect right parantheses after the expression
  expect_lexeme (lx_Rparen);

  /// return tree
  return tree;
}

/**
 * @brief
 *
 * @param[in]   type            type of node in tree
 * @param[in]   curr_lexeme     lexeme to make leaf with
 *
 * @return      Syntax tree node pointer
 *
 * @retval      node_s*     On success
 * @retval      NULL        On error
 *
 */
node_s*
make_leaf_node (ast_node_type_e type, lexeme_s *curr_lexeme)
{

  logger(DEBUG, "=== START ===");

  /// Create the leaf node to return
  node_s *node = calloc (1, sizeof(node_s));
  logger(DEBUG, "assert(node)");
  assert(node);
  _PASS;

  /// Assign node type to new node
  node->node_type = type;

  /// If lexeme type is a string or an identifier
  if ((type == nd_String) || (type == nd_Ident))
    node->char_val = strdup (curr_lexeme->char_val);

  /// Otherwise the lexeme type is an integer
  else if (type == nd_Integer)
    node->int_val = curr_lexeme->int_val;

  logger(DEBUG, "Returning leaf node with val: '%s'.", node->char_val);

  logger(DEBUG, "=== END ===");
  return node;
}

/**
 * @brief       Build and return expression node
 *
 * @param[in]   precedence    Precedence of mathematical operation
 *
 * @return      Syntax tree node pointer
 *
 * @retval      node_s*     On success
 * @retval      NULL        On error
 */
node_s*
make_expression_node(int precedence)
{
  /// Create the tree node to return
  node_s* tree = NULL;
  node_s* node = NULL;

  lexeme_type_e operator = lx_NOP;

  switch(ast_curr_lexeme->type){

    case lx_Not:
      /// If lexeme type is Not, get next lexeme
      ast_curr_lexeme = ast_curr_lexeme->next;

      /// ...make Not node with the children next_lexeme and NULL
      tree = make_ast_node(nd_Not,make_expression_node(grammar[lx_Not].precedence),NULL);
      break;

    case lx_Add:
    case lx_Sub:
      /// If lexeme type is Add or Sub, save type
      operator = ast_curr_lexeme->type;
      ast_curr_lexeme = ast_curr_lexeme->next;

      /// Get next lexeme and make new expression node with it
      node = make_expression_node(grammar[lx_Negate].precedence);

      /// If original node type was Sub
      if (operator == lx_Sub)

        /// ...make a Negate node with the children new node and NULL
        tree = make_ast_node(nd_Negate, node, NULL);

      /// Else only use the new node
      else
        tree = node;
      break;

    case lx_Integer:
      /// If lexeme type is Integer, make leaf node and get next lexeme
      tree = make_leaf_node(nd_Integer, ast_curr_lexeme);
      ast_curr_lexeme = ast_curr_lexeme->next;
      break;

    case lx_Ident:
      /// If lexeme type is Ident, make leaf node and get next lexeme
      tree = make_leaf_node(nd_Ident, ast_curr_lexeme);
      ast_curr_lexeme = ast_curr_lexeme->next;
      break;

    case lx_Input:
      /// If lexeme type is Input, get next lexeme
      ast_curr_lexeme = ast_curr_lexeme->next;

      /// ...expect LParen
      expect_lexeme(lx_Lparen);

      /// ... and make Input node with NULL as one child
      node_s *input_tree = make_ast_node (nd_Input,make_leaf_node(nd_String, ast_curr_lexeme), NULL);

      /// ... and expect String contents as the other
      expect_lexeme(lx_String);
      tree = make_ast_node(nd_Sequence, input_tree, tree);

      /// ... finally expect Rparen to close Input
      expect_lexeme(lx_Rparen);
      break;

    case lx_Lparen:
      /// If lexeme type is Lparen, make tree from contents within
      tree = make_parentheses_expression ();
      break;

    default:
      /// Expressions cannot start with any other type of lexeme
      fprintf (stderr, "[%d:%d] Unexpected lexeme type found: %s\n",
               ast_curr_lexeme->line, ast_curr_lexeme->column,
               op_name[ast_curr_lexeme->type]);
      exit (opal_exit (EXIT_FAILURE));
  }

    /// While the next lexeme is binary and its precedence is at least as high as the current lexeme
    while (grammar[ast_curr_lexeme->type].is_binary && grammar[ast_curr_lexeme->type].precedence >= precedence)
      {
        /// Save lexeme type and get next lexeme
        lexeme_type_e orig_op = ast_curr_lexeme->type;
        ast_curr_lexeme = ast_curr_lexeme->next;

         /// Search for higher precedence in a later lexeme
         int precedence_ctr = grammar[orig_op].precedence;

         /// and increment counter to work up precedence hierarchy
         if(!grammar[orig_op].right_associative)
             precedence_ctr++;

         /// Recursively make new expression node with incremented precedence
         node = make_expression_node(precedence_ctr);

         /// ...and add it to a working tree
         tree = make_ast_node(grammar[orig_op].node_type, tree, node);

      }/// ...until all higher precedented lexemes in expression are processed

  return tree;
}

/**
 * @brief       Build and return syntax tree node for a statement
 *
 * @return      Syntax tree node pointer
 *
 * @retval      node_s*     On success
 * @retval      NULL        On error
 */
node_s*
make_statement_node (void)
{
  node_s *tree = NULL;                  ///< Syntax tree node to return
  node_s *value = NULL;                 ///< Leaf node with int/string value
  node_s *expression = NULL;            ///< Node for expression
  node_s *condition_statement = NULL;   ///< if/while condition statement node
  node_s *else_statement = NULL;        ///< else condition statement node

  switch (ast_curr_lexeme->type)
    {
    case lx_If:
      /// If next lexeme is if statement, read next lexeme
      ast_curr_lexeme = ast_curr_lexeme->next;

      /// ... get expression inside left parentheses
      expression = make_parentheses_expression ();

      /// ... get condition statement node
      condition_statement = make_statement_node ();

      /// ... and create else statement node as NULL
      else_statement = NULL;

      /// If next lexeme is an else
      if (ast_curr_lexeme->type == lx_Else)
        {
          /// ... read next lexeme
          ast_curr_lexeme = ast_curr_lexeme->next;

          /// ... and make else statement node
          else_statement = make_statement_node ();
        }

      /// Build and return the tree with left child as the expression node &
      /// right child as the code block to execute
      tree = make_ast_node (
          nd_If, expression,
          make_ast_node (nd_If, condition_statement, else_statement));
      break;

    case lx_Print:             // print '(' expr {',' expr} ')'
      /// If next lexeme is print, read next lexeme
      ast_curr_lexeme = ast_curr_lexeme->next;

      /// Loop over lexemes inside the left and right parantheses of print
      /// statement, incrementing with every comma lexeme found
      for (expect_lexeme (lx_Lparen);; expect_lexeme (lx_Comma))
        {
          /// For string inside print statement ...
          if (ast_curr_lexeme->type == lx_String)
            {
              /// Build tree with left child as op-code to print string &
              /// right child as the leaf node representing the string
              expression = make_ast_node (
                  nd_Prts, make_leaf_node (nd_String, ast_curr_lexeme), NULL);

              /// ... and read next lexeme
              ast_curr_lexeme = ast_curr_lexeme->next;
            }
          /// For integer inside print statement ...
          else
            {
              /// Build tree with left child as op-code to print integer &
              /// right child as the expression node representing the integer
              expression = make_ast_node (
                  nd_Prti, make_expression_node (0), NULL);

              /// make_expression_node() will read next lexeme
            }

          /// Build tree for statement till this comma
          tree = make_ast_node (nd_Sequence, tree, expression);

          /// If no more commas in print statement, return tree
          if (ast_curr_lexeme->type != lx_Comma)
            break;
        }

      /// Expect a ')' & a ';' after print, else print error and exit
      expect_lexeme (lx_Rparen);
      expect_lexeme (lx_Semi);
      break;

    case lx_Semi:
      /// If next lexeme is semicolon, read next lexeme & return tree
      ast_curr_lexeme = ast_curr_lexeme->next;
      break;

    case lx_NOP:
      /// If next lexeme is no operation, read next lexeme & return tree
      ast_curr_lexeme = ast_curr_lexeme->next;
      break;

    case lx_Ident:
      /// If next lexeme is an identifier create leaf node for it
      value = make_leaf_node (nd_Ident, ast_curr_lexeme);

      /// ... and read next lexeme
      ast_curr_lexeme = ast_curr_lexeme->next;

      /// Expect an '=' operator after an identifier, else print error and exit
      expect_lexeme (lx_Assign);

      /// Build expression tree whose result we will assign to the identifier
      expression = make_expression_node (0);

      /// Build tree with left child as identifier & right child as expression
      tree = make_ast_node (nd_Assign, value, expression);

      /// Expect a semi colon after expression, else print error and exit
      expect_lexeme (lx_Semi);
      break;

    case lx_While:
      /// If next lexeme is while, read next lexeme
      ast_curr_lexeme = ast_curr_lexeme->next;

      /// ... build expression node inside parantheses
      expression = make_parentheses_expression ();

      /// ... build tree node to execute if condition is true
      condition_statement = make_statement_node ();

      /// ... return while tree with left child as expression & right child as
      /// code block to execute
      tree = make_ast_node (nd_While, expression, condition_statement);
      break;

    case lx_Lbrace:
      /// If next lexeme is left brace, build tree for code block until
      /// right brace lexeme is found
      /*for ( expect (lx_Lbrace); ast_curr_lexeme->type != lx_Rbrace && ast_curr_lexeme->type != lx_EOF; )
        {
          tree = make_ast_node (nd_Sequence, tree, make_statement_node ());
        }
        */
      expect_lexeme (lx_Lbrace);
      while (ast_curr_lexeme->type != lx_Rbrace
          && ast_curr_lexeme->type != lx_EOF)
        {
          tree = make_ast_node (nd_Sequence, tree, make_statement_node ());
        }

      /// Expect a right brace after code block and return tree, else print
      /// error and exit
      expect_lexeme (lx_Rbrace);
      break;

    case lx_EOF:
      /// If next lexeme is end of file, break and return tree
      break;

    default:
      /// Statements cannot start with any other type of lexeme
      fprintf(stderr, "[%d:%d] Cannot start statement with '%s': %s\n",
             ast_curr_lexeme->line, ast_curr_lexeme->column,
             grammar[ast_curr_lexeme->type].text, ast_curr_lexeme->char_val);
      exit(opal_exit(EXIT_FAILURE));
    }

  return tree;
}

/**
 * @brief       Optimize the abstract syntax tree
 * @param[in]   tree
 *
 * @return      Optimized abstract syntax tree root pointer
 *
 * @retval      node_s*     On success
 * @retval      NULL        On error
 */
node_s*
optimize_syntax_tree(node_s *tree)
{

  /// Return NULL if no node
  if (!tree)
    return NULL;

  /// If no left or right child nodes
  if (!tree->left && !tree->right)
    {
      if (tree->node_type == nd_Sequence)
        {
          /// Empty code block with no child nodes
          return NULL;
        }
      else
        {
          /// Leaf node
          return tree;
        }
    }

  /// If no left node, return address of right
  if (!tree->left
      && (tree->node_type == nd_Sequence || tree->node_type == nd_If))
    return optimize_syntax_tree (tree->right);

  /// If no right node, return address of left
  else if (!tree->right
      && (tree->node_type == nd_Sequence || tree->node_type == nd_If))
    return optimize_syntax_tree (tree->left);

  /// If node has left and right nodes, optimize them
  tree->left = optimize_syntax_tree (tree->left);
  tree->right = optimize_syntax_tree (tree->right);

  return tree;
}

/**
 * @brief           Print abstract syntax tree to destination file
 *
 * @param[in]       syntax_tree       Abstract syntax tree
 * @param[in,out]   dest_fp           Destination file pointer
 *
 * @return      The error return code of the function.
 *
 * @retval          EXIT_SUCCESS    On success
 * @retval          EXIT_FAILURE    On error
 * @retval          errno           On system call failure
 *
 */
short
print_ast (node_s *syntax_tree, FILE *dest_fp)
{
  logger(DEBUG, "=== START ===");

  /// Check if syntax tree pointer is not NULL
  logger(DEBUG, "assert(syntax_tree)");
  assert(syntax_tree);
  _PASS;

  /// Print syntax tree in pre-traversal mode to destination file pointer
  traverse_ast (syntax_tree, dest_fp);

  logger(DEBUG, "=== END ===");
  return (EXIT_SUCCESS);
}

/**
 * @brief           Traverse abstract syntax tree pre-order
 * @param[in]    node       Abstract syntax tree node to print
 * @param[in]    report_fp  Destination report file pointer
 * @param[in]    level      Level in tree
 */
void
traversePreOrder_graph (node_s *node, FILE *report_fp, int level)
{
  /// If node to print is null, return
  if (!node)
    return;

  /// If node is string, print char_val
  if (node->node_type == nd_String)
    fprintf (report_fp, "%d[\"'%s'\"]:::%s\n", level, node->char_val,
             node_name[node->node_type]);

  /// If node is identifier, print name
  else if (node->node_type == nd_Ident)
      fprintf (report_fp, "%d[%s]:::%s\n", level, node->char_val,
               node_name[node->node_type]);

  /// ... if node is integer, print the int_val
  else if (node->node_type == nd_Integer)
    fprintf (report_fp, "%d[%d]:::%s\n", level, node->int_val,
             node_name[node->node_type]);

  /// ... else, print node type name
  else
    fprintf (report_fp, "%d[%s]:::%s\n", level, node_name[node->node_type],
             node_name[node->node_type]);

  /// If node has left/right child nodes, create connection with array index
  if (node->left)
    fprintf (report_fp,"%d --> %d\n", level, level*2 + 1);

  if (node->right)
    fprintf (report_fp,"%d --> %d\n", level, level*2 + 2);

  /// Print child nodes of tree recursively for left and right child nodes
  if (node->left || node->right)
    {
      traversePreOrder_graph (node->left, report_fp, level*2 + 1);
      traversePreOrder_graph (node->right, report_fp, level*2 + 2);
    }
}

/**
 * @brief           Print abstract syntax tree tree to HTML report file
 *
 * @param[in]       syntax_tree       Abstract syntax tree
 * @param[in,out]   report_fp         Report file pointer
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
print_ast_html (node_s *syntax_tree, FILE *report_fp)
{
  logger(DEBUG, "=== START ===");

  /// Check if syntax tree pointer is not NULL
  logger(DEBUG, "assert(syntax_tree)");
  assert(syntax_tree);
  _PASS;

  /// Write mermaid graph header
  fprintf(report_fp, "<div class='mermaid'>\ngraph TD;\n");

  /// Open res/mermaid.styles in read-only mode
  char *mermaid_fn = "res/mermaid.styles";
  sprintf (perror_msg, "css_fp = fopen ('%s', 'r')", mermaid_fn);
  logger (DEBUG, perror_msg);

  errno = EXIT_SUCCESS;
  FILE *mermaid_fp = fopen (mermaid_fn, "r");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      exit (opal_exit(errno));
    }

  /// Copy CSS to HTML report
  logger (DEBUG, "Copying Mermaid styles to HTML report");
  char ch = 0;
  while ((ch = fgetc (mermaid_fp)) != EOF)
    fputc (ch, report_fp);
  _DONE;
  fprintf(report_fp, "\n");

  /// Close res/styles.css file
  sprintf (perror_msg, "fclose(mermaid_fp)");
  logger (DEBUG, perror_msg);
  if (fclose (mermaid_fp) == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      exit (opal_exit (errno));
    }

  /// Print abstract syntax tree to report
  traversePreOrder_graph (syntax_tree, report_fp, 0);

  /// Write mermaid graph footer
  fprintf(report_fp, "</div>\n"
          "<script src='https://cdn.jsdelivr.net/npm/mermaid/dist/mermaid.min.js'></script>\n"
          "<script>mermaid.initialize({startOnLoad:true, flowchart: {curve:'cardinal', useMaxWidth:false, }, });</script>\n");

  sprintf (perror_msg, "fflush(graph_fp)");
  logger (DEBUG, perror_msg);

  errno = EXIT_SUCCESS;
  fflush (report_fp);
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror(perror_msg);
      _FAIL;
      return (errno);
    }

  logger(DEBUG, "=== END ===");
  return (EXIT_SUCCESS);
}

/**
 * @brief       Free memory allocated for syntax tree
 *
 * @param[in,out]   syntax_tree    Syntax tree to deallocate
 *
 * @return      NULL
 *
 */
void
free_syntax_tree (node_s *syntax_tree)
{
  logger(DEBUG, "=== START ===");

  /// Walk the tree free each node starting with the leaf nodes
  logger(DEBUG, "TODO: Replace stub implementation.");

  logger(DEBUG, "=== START ===");
}

/**
 * @brief                   Traverses the syntax tree while printing the contents to dest_fp.
 *
 * @param[in]   node        The syntax tree node to be processed
 * @param[in]   dest_fp     The file to which the node's content will be printed
 *
 * @return                  NULL
 */
void
traverse_ast(node_s *node, FILE *dest_fp)
{
    /// We have reached a NULL leaf
    if (!node)
        return;

    /// If node is identifier or string, print char_val
    if (node->node_type == nd_Ident || node->node_type == nd_String)
        fprintf (dest_fp, "%s\n", node->char_val);

    /// ... if node is integer, print the int_val
    else if (node->node_type == nd_Integer)
        fprintf (dest_fp, "%d\n", node->int_val);

    /// ... else, print node type name
    else
        fprintf (dest_fp, "%s\n", node_name[node->node_type]);

    /// Recursive calls for further tree traversal
    traverse_ast (node->left, dest_fp);
    traverse_ast (node->right, dest_fp);
}

/*
 * ==================================
 * START GENIE FUNCTION DEFINITIONS
 * ==================================
 */

/**
 * @brief Append ASM code to array
 * @param code      ASM code
 * @param intval    Integer value
 * @param label     String value
 */
void
add_asm_code (asm_code_e code, int intval, char *label)
{
  /// Create struct with given intval and code
  asm_cmd_e asm_cmd = { 0 };
  asm_cmd.intval = intval;
  asm_cmd.cmd = code;

  /// Add the asm_code label if there is one
  if (label)
  {
      asm_cmd.label = strdup(label);
  }

  logger(DEBUG, "Added command - cmd: %s, label: %s", asm_cmds[asm_cmd.cmd], asm_cmd.label ? asm_cmd.label : "NULL");

    /// Adds the asm_cmd
  asm_cmd_list[asm_cmd_list_len++] = asm_cmd;
}

/**
 * @brief Generate assembly command list from given abstract syntax tree
 * @param       ast   Abstract syntax tree
 *
 * @return      NULL
 */
void
gen_asm_code (node_s *ast)
{
  // int location_offset = 0;
  // int int_val = 0;
  char start_label[64] = { 0 };
  char else_label[64] = { 0 };
  char end_label[64] = { 0 };

  if (!ast)
    return;

  switch (ast->node_type)
    {
    case nd_Sequence:
      gen_asm_code (ast->left);
      gen_asm_code (ast->right);
      break;
    case nd_While:
      sprintf (start_label, "_while_loop_%d", asm_cmd_list_len);
      sprintf (end_label, "_while_end_%d", asm_cmd_list_len);

      add_asm_code (asm_Label, 0, start_label);     // while block start
      gen_asm_code (ast->left);                     // check condition
      add_asm_code (asm_Jz, 0, end_label);          // if false, end
      gen_asm_code (ast->right);                    // body
      add_asm_code (asm_Jmp, 0, start_label);       // loop back
      add_asm_code (asm_Label, 0, end_label);       // while block end

      break;
    case nd_If:
      sprintf (start_label, "_if_%d", asm_cmd_list_len);
      sprintf (else_label, "_else_%d", asm_cmd_list_len);
      sprintf (end_label, "_fi_%d", asm_cmd_list_len);

      add_asm_code (asm_Label, 0, start_label);    // start if
      gen_asm_code (ast->left);                    // check condition
      add_asm_code (asm_Jz, 0, else_label);        // false, jump to else block
      gen_asm_code (ast->right->left);             // true, execute body ..
      add_asm_code (asm_Jmp, 0, end_label);        // .. and exit
      add_asm_code (asm_Label, 0, else_label);     // start else
      gen_asm_code (ast->right->right);            // execute else body and exit
      add_asm_code (asm_Label, 0, end_label);      // if/else end
      break;
    default:
      fprintf(stderr, "Unexpected operator: %s\n", node_name[ast->node_type]);
      exit (opal_exit (EXIT_FAILURE));
    }

  return;
}

/**
 * @brief Print assembly command list
 *
 * @param       cmd_list    Assembly command list to print
 * @param       dest_fp     Destination file pointer
 *
 * @return      Function exit code
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 */
short print_asm_code(asm_cmd_e cmd_list[], FILE *dest_fp)
{
  /*
   * Traverse and print the assembly code
   * Steps:
   * 1. Print macro header
   * 2. Print user code
   * 3. Print footer
   * 4. Create strings and their lengths
   *    4a. Read each string character and print ASCII value for newline
   *    4b. NULL terminate string
   *    4c. Print string length
   *    4d. Print strings and lengths array
   * 5. Create variables array
   */

  /// Copy NASM header file with macros to dest_fp


  /// Print user code
  int i = 0;
  logger (DEBUG, "Print ASM user code");
  for (i = 0; i < asm_cmd_list_len; i++)
    {
      switch (asm_cmd_list[i].cmd)
        {
        case asm_Fetch:
        case asm_Store:
        case asm_Push:
          fprintf (dest_fp, "  %s\t%d\n", asm_cmds[asm_cmd_list[i].cmd],
                   asm_cmd_list[i].intval);
          break;
        case asm_Add:
        case asm_Sub:
        case asm_Negate:
        case asm_Mul:
        case asm_Div:
        case asm_Mod:
        case asm_Eq:
        case asm_Neq:
        case asm_Lss:
        case asm_Gtr:
        case asm_Leq:
        case asm_Geq:
        case asm_And:
        case asm_Or:
        case asm_Not:
        case asm_Prts:
        case asm_Input:
        case asm_Prti:
        case asm_HALT:
          fprintf (dest_fp, "  %s\n", asm_cmds[asm_cmd_list[i].cmd]);
          break;
        case asm_Label:
          fprintf (dest_fp, "%s:\n", asm_cmd_list[i].label);
          break;
        case asm_Jz:
        case asm_Jmp:
          fprintf (dest_fp, "  %s\t\t%s\n", asm_cmds[asm_cmd_list[i].cmd],
                   asm_cmd_list[i].label);
          break;
        default:
          logger (ERROR, "Unknown opcode %d\n", asm_cmd_list[i].cmd);
          exit (opal_exit (EXIT_FAILURE));
        }
    }
  _DONE;

  /// Copy NASM footer file to dest_fp

  /// Create strings and their lengths

  /// Create integers array

  return EXIT_SUCCESS;
}

/**
 * @brief Print assembly code list to HTML report file
 *
 * @param       cmd_list    Assembly command list to print
 * @param       report_fp   Destination HTML reportfile pointer
 *
 * @return      Function exit code
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 */
short
print_asm_code_html (asm_cmd_e cmd_list[], FILE *dest_fp)
{

  fprintf (dest_fp,
           "<textarea style='resize: none;' readonly rows='25' cols='80'>");

  int i = 0;
  logger(DEBUG, "Print ASM user code to HTML");
  for (i = 0; i < asm_cmd_list_len; i++)
    {
      switch (asm_cmd_list[i].cmd)
        {
        case asm_Fetch:
        case asm_Store:
        case asm_Push:
          fprintf (dest_fp, "  %s\t%d\n", asm_cmds[asm_cmd_list[i].cmd],
                   asm_cmd_list[i].intval);
          break;
        case asm_Add:
        case asm_Sub:
        case asm_Negate:
        case asm_Mul:
        case asm_Div:
        case asm_Mod:
        case asm_Eq:
        case asm_Neq:
        case asm_Lss:
        case asm_Gtr:
        case asm_Leq:
        case asm_Geq:
        case asm_And:
        case asm_Or:
        case asm_Not:
        case asm_Prts:
        case asm_Input:
        case asm_Prti:
        case asm_HALT:
          fprintf (dest_fp, "  %s\n", asm_cmds[asm_cmd_list[i].cmd]);
          break;
        case asm_Label:
          fprintf (dest_fp, "%s:\n", asm_cmd_list[i].label);
          break;
        case asm_Jz:
        case asm_Jmp:
          fprintf (dest_fp, "  %s\t\t%s\n", asm_cmds[asm_cmd_list[i].cmd],
                   asm_cmd_list[i].label);
          break;
        default:
          fprintf(stderr, "Unknown opcode %d\n", asm_cmd_list[i].cmd);
          exit (opal_exit (EXIT_FAILURE));
        }
    }
  _DONE;

  fprintf (dest_fp, "</textarea>");
  return EXIT_SUCCESS;
}

/**
 * @brief       Get index of an identifier in array, add if missing
 *
 * @param[in]   ident_curr   identifier to get index for
 *
 * @return      index of identifier in the array
 */
int add_var(char *ident_curr)
{
  /// If identifier array is not empty
  if (vars_len > 0)
    {
      /// Search it for the current identifier
      for (int i = 0; i < vars_len; i++)
        {
          /// and return its index if found
          if (strcmp(ident_curr,vars[i]) == 0)
            {
              logger (DEBUG, "Identifier '%s' found at index %d.", ident_curr, i);
              return i;
            }
        }
    }

    int index = vars_len;
    /// Otherwise append the identifier to the array
    logger (DEBUG, "Created new identifier '%s' at index %d.", ident_curr, index);
    vars[vars_len++] = strdup (ident_curr);

    /// and return its index
    return index;
}

/**
 * @brief       Get index of a string in array, add if missing
 *
 * @param[in]   str_curr   string to get index for
 *
 * @return      index of string in the array
 */
int add_str(char *str_curr)
{
  /// If string array is not empty
  if (strs_len > 0)
    {
      /// Search it for the current string
      for (int i = 0; i < strs_len; i++)
        {
          /// and return its index if found
          if (strcmp(str_curr,strs[i]) == 0)
            {
              logger (DEBUG, "Identifier '%s' found at index %d.", str_curr, i);
              return i;
            }
        }
    }

    int index = strs_len;
    /// Otherwise append the string to the array
    logger (DEBUG, "Created new identifier '%s' at index %d.", str_curr, index);
    strs[strs_len++] = strdup (str_curr);

    /// and return its index
    return index;
}
