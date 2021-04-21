/// @file opal.c
/// @authors Damle Kedar, Mckerracher Joshua, Leon Sarah Louise
///

#include "../include/opal.h"

#include <assert.h>             /* assert() */
#include <bits/types/FILE.h>
#include <ctype.h>              /* isspace(), isalnum() */
#include <errno.h>              /* errno macros and codes */
#include <regex.h> 				/* ReGex functions */
#include <stdarg.h>             /* variadic functions */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>             /* fopen, fclose, exit() */
#include <string.h>             /* memset() */
#include <strings.h>
#include <unistd.h>

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

  /// 1. Assert log file pointer is not null
  assert(log_fp);

  /// 2. Allocate buffer to hold message to log
  char buf[4096] =
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
      fprintf (log_fp, "%s", buf);
      fflush (log_fp);
      return;
    }

  /**
   * 5. If log message is less than current log level, print message with
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

  /// 6. Flush message to log file
  fflush (log_fp);
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
 * @param[int/out] report_fp    Report file pointer
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
  sprintf (perror_msg, "css_fp = fopen ('res/styles.css', 'r')");
  logger (DEBUG, perror_msg);
  FILE *css_fp = fopen ("res/styles.css", "r");
  if (css_fp != NULL)
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
  fseek (source_fp, 0, SEEK_SET);

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
                char filename_buffer[256] =
                  { 0 };
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

                /// If include file does not exist, print error and exit
                sprintf (perror_msg, "access('%s', F_OK)", filename_buffer);
                logger(DEBUG, perror_msg);
                if (access (filename_buffer, F_OK) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    perror (perror_msg);
                    _FAIL;
                    return (errno);
                  }

                /// If include file can not be read, print error and exit
                sprintf (perror_msg, "access('%s', R_OK)", filename_buffer);
                logger(DEBUG, perror_msg);
                if (access (filename_buffer, R_OK) == EXIT_SUCCESS)
                  _PASS;
                else
                  {
                    perror (perror_msg);
                    _FAIL;
                    return (errno);
                  }

                /// Open include file in read-only mode
                sprintf (perror_msg, "include_fp = fopen('%s', 'r')",
                         filename_buffer);
                logger(DEBUG, perror_msg);
                FILE *include_fp = fopen (filename_buffer, "r");
                if (include_fp != NULL)
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
                       filename_buffer);
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
 * @return      Next lexeme struct with values populated
 *
 * @retval      struct lexeme
 *
 */
lexeme_s
get_string_literal_lexeme (int char_line, int char_col)
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

  /// Determine if string is a reserved keyword
  for (int i = 0; i < (sizeof(keyword_arr) / sizeof(keyword_arr[0])); i++)
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
      retVal.char_val = strdup (identifier_str);
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
 * @param[in]       lexeme_s    Lexeme to stringify
 * @param[in/out]   buffer      Buffer to store string value of lexeme
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
get_lexeme_str (lexeme_s *lexeme, char *buffer, const int buffer_len)
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
 * @param[in/out]   symbol_table    Symbol table linked list to populate
 * @param[in/out]   symbol_count    Pointer to count of lexemes found
 * @param[in/out]   source_fp       Source file pointer
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

  lexeme_s *symbol_table_tail = NULL;

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
          next_lexeme.char_val ? strdup (next_lexeme.char_val) : NULL;

      /// Get the current tail of the symbol table
      symbol_table_tail = symbol_table;
      while (symbol_table_tail->next)
        {
          symbol_table_tail = symbol_table_tail->next;
        }

      /// Call get_lexeme_str() to stringify next_lexeme
      if (get_lexeme_str (new_symbol, lexeme_str,
                          lexeme_str_len) != EXIT_SUCCESS)
        return (EXIT_FAILURE);

      /// Append lexeme to symbol table
      logger(DEBUG, "Append lexeme {%s}", lexeme_str);
      symbol_table_tail->next = new_symbol;

      /// Increment symbol count
      *symbol_count = *symbol_count + 1;

    }
  while (next_lexeme.type != lx_EOF);

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief       Print symbol table to destination file pointer
 *
 * @param[in/out]   symbol_table    Symbol table to print
 * @param[in/out]   dest_fp         Destination file pointer
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

  lexeme_s *symbol_table_tail = symbol_table;
  while (symbol_table_tail->next)
    {
      /// Call get_lexeme_str() to stringify next_lexeme
      retVal = get_lexeme_str (symbol_table_tail, lexeme_str,
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

      symbol_table_tail = symbol_table_tail->next;
    }
  _DONE;

  logger(DEBUG, "=== END ===");
  return EXIT_SUCCESS;
}

/**
 * @brief       Print symbol table HTML report to report file pointer
 *
 * @param[in/out]   symbol_table    Symbol table to print
 * @param[in/out]   report_fp       Report file pointer
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
 * @param[in/out]   symbol_table    Symbol table to print
 *
 * @return      NULL
 *
 */
void
free_symbol_table (lexeme_s *symbol_table)
{
  lexeme_s *next_symbol;

  while (symbol_table)
    {
      next_symbol = symbol_table;

      if (next_symbol->char_val)
        {
          free (next_symbol->char_val);
          next_symbol->char_val = NULL;
        }

      symbol_table = symbol_table->next;
      free (next_symbol);
    }
}

/*
 * ==================================
 * END ALEX FUNCTION DEFINITIONS
 * ==================================
 */
