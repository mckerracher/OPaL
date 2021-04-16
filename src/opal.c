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
#include <unistd.h>
#include <ctype.h>              /* isspace(), isalnum() */

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
  assert(log_fd);

  /// 2. Allocate buffer to hold message to log
  char buf[1024] =
    { 0 };

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
      fprintf (log_fd, "\n[%10s:%4d] %20s() %s", file, line, func, buf);
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
  char stars[64] =
    { 0 };
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
      _FAIL;
      perror (perror_msg);
      return (errno);
    }

  /// Close source file
  if (source_fd && source_fd != stdin)
    {
      sprintf (perror_msg, "fclose(source_fd)");
      logger(DEBUG, perror_msg);
      if (fclose (source_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
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
  if (dest_fd && dest_fd != stdout)
    {
      sprintf (perror_msg, "fflush(dest_fd)");
      logger(DEBUG, perror_msg);
      if (fflush (dest_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
          return (errno);
        }

      sprintf (perror_msg, "fclose(dest_fd)");
      logger(DEBUG, perror_msg);
      if (fclose (dest_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
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
  if (report_fd)
    {
      sprintf (perror_msg, "fflush(report_fd)");
      logger(DEBUG, perror_msg);
      if (fflush (report_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
          return (errno);
        }

      sprintf (perror_msg, "fclose(report_fd)");
      logger(DEBUG, perror_msg);
      if (fclose (report_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
          return (errno);
        }
    }

  if (report_fn)
    {
      logger(DEBUG, "free(report_fn)");
      free(report_fn);
      report_fn = NULL;
    }

  /// Flush and close log file
  if (log_fd && log_fd != stdout)
    {
      logger(DEBUG, "=== END ===");

      sprintf (perror_msg, "fflush(log_fd)");
      logger(DEBUG, perror_msg);
      if (fflush (log_fd) == EXIT_SUCCESS)
        _PASS;
      else
        {
          _FAIL;
          perror (perror_msg);
          return (errno);
        }

      sprintf (perror_msg, "fclose(log_fd)");
      logger(DEBUG, perror_msg);
      logger(DEBUG, "\n");
      if (fclose (log_fd) != EXIT_SUCCESS)
        {
          perror (perror_msg);
          return (errno);
        }
    }
  else
    logger(DEBUG, "=== END ===\n\n");

  if (log_fn)
    {
      free(log_fn);
      log_fn = NULL;
    }

  return (code);
}

/**
 * @brief       Function to read next character from the source file descriptor
 *
 * @return      Character read
 *
 * @retval      Next character read from FILE *stream source_fd
 * @retval      errno           On system call failure
 *
 */

int
read_next_char (void)
{
  /// Read character from source file descriptor
  next_char = getc (source_fd);

  /// getc() sets the errno in the event of an error
  if (errno != 0)
    {
      perror (perror_msg);
      opal_exit (errno);
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
 * @param[int/out] report_fd    Report file descriptor
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */

short
init_report (FILE *report_fd)
{

  logger(DEBUG, "=== START ===");

  /// Assert report file descriptor is not NULL
  assert(report_fd);

  /// Write HTML head tag to the report
  logger (DEBUG, "Writing HTML head tag to report");
  fprintf (report_fd, "<!DOCTYPE html>\n"
           "<html>\n"
           "<head>\n"
           "<title>OPaL compilation report</title>\n"
           "<style>\n"
           "</style>\n"
           "</head>\n");

  /// Start HTML body tag
  fprintf (report_fd, "<body>\n");

  /// Open textarea tag for source file
  fprintf (report_fd, "<h2>Compilation steps report </h2>\n"
           "<h3>Original source file: <code>%s</code></h3>\n<hr>\n"
           "<textarea style='resize: none;' readonly rows='25' cols='80'>\n",
           source_fn);

  /// Append source file to HTML report and close textarea tag
  char ch = 0;
  logger (DEBUG, "Copying source file to HTML report");
  while ((ch = fgetc (source_fd)) != EOF)
    fputc (ch, report_fd);
  _DONE;

  fprintf (report_fd, "\n</textarea>\n");
  fflush (report_fd);

  /// Rewind source file descriptor
  sprintf (perror_msg, "rewind('%s')", source_fn);
  logger(DEBUG, perror_msg);
  rewind (source_fd);

  /// If current value of source file position not 0, print error and exit
  if (ftell (source_fd) == 0)
    {
      _DONE;
    }
  else
    {
      _FAIL;
      perror (perror_msg);
      return (errno);
    }

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
 * @param[in]   source_fd     Source to be read from
 * @param[in]   dest_fd       Destination to written to
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */

short
rem_comments (FILE *source_fd, FILE *dest_fd)
{
  logger(DEBUG, "=== START ===");

  /// Check if source file descriptor is not NULL
  logger(DEBUG, "assert(source_fd)");
  assert(source_fd);
  _PASS;

  /// Check if destination file descriptor is not NULL
  logger(DEBUG, "assert(dest_fd)");
  assert(dest_fd);
  _PASS;

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
              fprintf (stderr, "Invalid end of file in comment");
              return EXIT_FAILURE;
            }

          /// If char is a newline, write to file to preserve line numbers
          if (ch == '\n')
            {
              fputc (ch, dest_fd);
            }
        }
    }

  logger(DEBUG, "Removed %d comment lines", numComments);
  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief       Read source, process includes, write to destination
 *
 * @param[in]   source_fd     Source to be read from
 * @param[in]   dest_fd       Destination to written to
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */

short
proc_includes (FILE *source_fd, FILE *dest_fd)
{
  logger(DEBUG, "=== START ===");

  /// Assert source file descriptor is not NULL
  logger(DEBUG, "assert(source_fd)");
  assert(source_fd);
  _PASS;

  /// Assert destination file descriptor is not NULL
  logger(DEBUG, "assert(dest_fd)");
  assert(dest_fd);
  _PASS;

  /// Move source_fd to beginning of file.
  fseek (source_fd, 0, SEEK_SET);

  /// Copy each character to the destination file, while checking for include files.
  logger(DEBUG, "Reading file.");
  char ch = fgetc (source_fd);
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
            char include_buffer[9] =
              { 0 };
            ssize_t sz = fread (include_buffer, sizeof(char), sizeof(char) * 8,
                                source_fd);

            /// Rewinds the file pointer.
            fseek (source_fd, -sz, SEEK_CUR);

            /// If include is found, process the included file.
            if (strcasecmp (include_buffer, "include ") == 0)
              {
                logger(DEBUG, "Include keyword has been found.");

                /// Move file pointer to the point after "include "
                fseek (source_fd, sz, SEEK_CUR);
                char filename_buffer[256] =
                  { 0 };
                int filename_len = 0;

                /// Get the filename for the include file.
                ch = fgetc (source_fd);
                while (ch != '\n' && filename_len < 256)
                  {
                    if (ch != '"')
                      filename_buffer[filename_len++] = ch;
                    ch = fgetc (source_fd);
                  }
                logger(DEBUG, "Finished reading in the filename.");

                /// If include file does not exist, print error and exit
                sprintf (perror_msg, "access('%s', F_OK)", filename_buffer);
                logger(DEBUG, perror_msg);
                if (access (filename_buffer, F_OK) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    _FAIL;
                    perror (perror_msg);
                    return (errno);
                  }

                /// If include file can not be read, print error and exit
                sprintf (perror_msg, "access('%s', R_OK)", filename_buffer);
                logger(DEBUG, perror_msg);
                if (access (filename_buffer, R_OK) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    _FAIL;
                    perror (perror_msg);
                    return (errno);
                  }

                /// Open include file in read-only mode
                sprintf (perror_msg, "include_fd = fopen('%s', 'r')",
                         filename_buffer);
                logger(DEBUG, perror_msg);
                FILE *include_fd = fopen (filename_buffer, "r");
                if (include_fd != NULL)
                  _PASS;
                else
                  {
                    _FAIL;
                    perror (perror_msg);
                    return (errno);
                  }

                char ch_2 = fgetc (include_fd);

                /// Move contents of include file into destination file
                logger(DEBUG, "Copy contents of %s into destination file",
                       filename_buffer);
                while (ch_2 != EOF)
                  {
                    fputc (ch_2, dest_fd);
                    ch_2 = fgetc (include_fd);
                  }
                _DONE;

                /// Flush destination file contents to disk
                sprintf (perror_msg, "fflush(dest_fd)");
                logger(DEBUG, perror_msg);
                if (fflush (dest_fd) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    _FAIL;
                    perror (perror_msg);
                    return (errno);
                  }

                /// Close include file descriptor
                sprintf (perror_msg, "fclose (include_fd)");
                logger(DEBUG, perror_msg);
                if (fclose (include_fd) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    _FAIL;
                    perror (perror_msg);
                    return (errno);
                  }
              }
            continue;
          }
        default:
          {
            fputc (ch, dest_fd);
          }
        }
      /// Gets the next char for the switch case to evaluate.
      ch = fgetc (source_fd);
    }

  logger(DEBUG, "=== END ===");
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
 * @return      Next lexeme struct with values populated
 *
 * @retval      struct lexeme
 *
 */

lexeme_s
get_string_literal_lexeme(int char_line, int char_col)
{

  // TODO: Replace stub implementation
  lexeme_s retVal = { 0 };
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

  // TODO: Replace stub implementation
  lexeme_type_e retVal = lx_NOP;
  return retVal;
}

lexeme_s
get_identifier_lexeme (int char_line, int char_col)
{

  // TODO: Replace stub implementation
  lexeme_s retVal =
      { 0 };
  return retVal;
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
  lexeme_s retVal =
    { 0 };

  /// Call read_next_char() to get the next character from source
  while (isspace (next_char))
    {
      read_next_char ();
    }

  /// Popoulate lexeme line and column number
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
 * @param[in]       lexeme_s    Lexeme to stringify
 * @param[in/out]   buffer      Buffer to store string value of lexeme
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */
short
get_lexeme_str (lexeme_s lexeme, char *buffer)
{

  /// Assert buffer is not NULL
  assert(buffer);

  /// Empty out the string buffer
  memset (buffer, 0, 1024 * sizeof(char));

  /// Populate the buffer with values from the struct
  sprintf (buffer, "{line: % 3d, col: % 3d, lx_type: %s, val: '%s'}",
           lexeme.line, lexeme.column, op_name[lexeme.type],
           lexeme.char_val ? lexeme.char_val : "");

  return EXIT_SUCCESS;
}

/**
 * @brief       Populate symbol table with lexemes in source file descriptor
 *
 * @param[in/out]   symbol_table    Symbol table linked list to populate
 * @param[in/out]   symbol_count    Pointer to count of lexemes found
 * @param[in/out]   source_fd       Source file descriptor
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

  /// Get lexemes in a loop until we get a EOF lexeme
  logger(DEBUG, "Get lexemes and print to standard out");

  do
    {
      /// Call get_next_lexeme() to populate next_lexeme
      next_lexeme = get_next_lexeme ();

      /// Call get_lexeme_str() to stringify next_lexeme
      if (get_lexeme_str (next_lexeme, next_lexeme_str) != EXIT_SUCCESS)
        return (EXIT_FAILURE);

      /// Print lexeme to standard out
      logger(DEBUG, "Append lexeme %s", next_lexeme_str);

    }
  while (next_lexeme.type != lx_EOF);

  /// STUB IMPLEMENTATION: Increment symbol count to non-zero for now
  *symbol_count = 1;

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief       Print symbol table to destination file descriptor
 *
 * @param[in/out]   symbol_table    Symbol table to print
 * @param[in/out]   dest_fd         Destination file descriptor
 *
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 *
 */

short
print_symbol_table (lexeme_s *symbol_table, FILE *dest_fd)
{
  logger(DEBUG, "=== START ===");

  /// Assert symbol table pointer is not NULL
  logger(DEBUG, "assert(symbol_table)");
  assert(symbol_table);
  _PASS;

  /// Assert destination file descriptor is not NULL
  logger(DEBUG, "assert(dest_fd)");
  assert(dest_fd);
  _PASS;

  // TODO: Replace stub implementation next
  logger(DEBUG, "STUB IMPLEMENTATION: Printing symbol table");
  _DONE;

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/*
 * ==================================
 * END ALEX FUNCTION DEFINITIONS
 * ==================================
 */
