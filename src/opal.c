/// @file opal.c

#include <argp.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>     /* fclose */
#include <string.h>
#include <unistd.h>

#include "../include/libopal.h"

/// Get build number from compiler
static void
argp_print_version (FILE *stream, struct argp_state *state)
{
  fprintf (stream, "OPaL Compiler version: %.2f\n", __VERSION_NUM);
}

/// Hook for printing build version
void
(*argp_program_version_hook) (FILE *stream, struct argp_state *state) =
argp_print_version;

/// Link for reporting bugs
const char *argp_program_bug_address =
    "https://github.com/mckerracher/OPaL/issues";

/// Program documentation
static char doc[] = "opal - OPaL Compiler";
static char args_doc[] = "FILE";            ///< Arguments we accept
static struct argp_option options[] =       ///< The options we understand
  {
    { "debug", 'd', 0, 0, "Log debug messages" },
    { "quiet", 'q', 0, 0, "Quiet; do not write anything to standard output."},
    { "log", 'l', "FILE", 0, "Save log to FILE instead of 'log/oc_log'" },
    { "output", 'o', "FILE", 0, "Output to FILE instead of 'a.out'" },
    { "report", 'r', "FILE", 0,
        "Save report to FILE instead of 'report/oc_report.html'" },
    { 0 }
  };

/// Struct to hold Command Line arguments
struct arguments
{
  char *args[1];     ///< Source file
  char *logfile;     ///< filename for logger
  char *destfile;    ///< filename for destination file
  char *report;      ///< filename for html report
  bool quiet;        ///< Print messages to standard output during execution
};

/**
 * @brief Get the input argument from argp_parse, which we know is a pointer to
 * our arguments structure.
 * @param [in] key An integer specifying which option this is
 * @param [in] arg For an option KEY, the string value of its argument, or NULL
 * @param [in] state A pointer to a struct argp_state
 * @return
 */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'd':
      LOG_LEVEL = DEBUG;
      break;

    case 'q':
      arguments->quiet = true;
      break;

    case 'l':
      arguments->logfile = arg;
      break;

    case 'o':
      arguments->destfile = arg;
      break;

    case 'r':
      arguments->report = arg;
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num >= 1)      // Too many arguments
        argp_usage (state);
      arguments->args[state->arg_num] = arg;
      break;

    case ARGP_KEY_END:
      if (state->arg_num < 1)       // Not enough arguments
        argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return EXIT_SUCCESS;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

/**
 * @brief       Main function for opal - OPaL compiler
 * @details
 * 1. Calls the remove_comments() and proc_includes() to process source file.
 * 2. Calls the build_symbol_table() to build symbol table.
 * 3. Calls build_syntax_tree() to build the abstract syntax tree.
 * 4. Calls gen_asm() to build the assembly code table and write to destination.
 * 5. Calls gen_obj() to assemble object file using NASM.
 * 6. Calls gen_bin() to link binary file using ld.
 *
 * @param[in]   argc    Number of command line arguments
 * @param[in]   argv    Vector of individual command line argument strings
 * @return      The error return code of the function.
 *
 * @retval      EXIT_SUCCESS    On success
 * @retval      EXIT_FAILURE    On error
 * @retval      errno           On system call failure
 */

int
main (int argc, char **argv)
{

  /// Create structure to process command line arguments
  struct arguments arguments =
    { .destfile = NULL, .logfile = NULL, .report = NULL, .quiet = false };

  /// Parse arguments
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  /// Populate variables for source, destination, log, report files
  source_fn = strdup (arguments.args[0]);
  dest_fn = arguments.destfile ? strdup (arguments.destfile) : strdup ("a.out");
  log_fn =
      arguments.logfile ? strdup (arguments.logfile) : strdup ("log/oc_log");
  report_fn =
      arguments.report ?
          strdup (arguments.report) : strdup ("report/oc_report.html");
  bool quiet = arguments.quiet;

  /// Open log file in append mode, else exit program
  sprintf (perror_msg, "log_fp = fopen(%s, 'a')", log_fn);
  errno = EXIT_SUCCESS;
  log_fp = fopen (log_fn, "a");
  if (errno != EXIT_SUCCESS)
    {
      perror (perror_msg);
      _FAIL;
      return (opal_exit (EXIT_FAILURE));
    }

  banner ("Main start.");
  logger(DEBUG, "Log: %s", log_fn);
  logger(DEBUG, "source_fn: '%s'", source_fn);
  logger(DEBUG, "report_fn: '%s'", report_fn);

  if (!quiet)
    {
      fprintf (stdout,
               "Source file:\t%s\nLog file:\t%s\nTemp directory:\ttmp/\n",
               source_fn, log_fn);
    }

  /// If source file does not exist, print error and exit
  sprintf (perror_msg, "access('%s', F_OK)", source_fn);
  logger(DEBUG, perror_msg);
  if (access (source_fn, F_OK) == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// If source file can not be read, print error and exit
  sprintf (perror_msg, "access('%s', R_OK)", source_fn);
  logger(DEBUG, perror_msg);
  if (access (source_fn, R_OK) == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Check if destination file exists
  sprintf (perror_msg, "access('%s', F_OK)", dest_fn);
  logger(DEBUG, perror_msg);
  if (access (dest_fn, F_OK) == EXIT_SUCCESS)
    {
      /// If destination file exists, delete it
      sprintf (perror_msg, "remove(%s)", dest_fn);
      logger(DEBUG, perror_msg);
      if (remove (dest_fn) == EXIT_SUCCESS)
        _PASS;
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  /// Open source file in read-only mode
  sprintf (perror_msg, "source_fp = fopen('%s', 'r')", source_fn);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  source_fp = fopen (source_fn, "r");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Check if report file exists
  sprintf (perror_msg, "access('%s', F_OK)", report_fn);
  logger(DEBUG, perror_msg);
  if (access (report_fn, F_OK) == EXIT_SUCCESS)
    {
      /// Truncate report file
      sprintf (perror_msg, "ftruncate(%s, EXIT_SUCCESS)", report_fn);
      logger(DEBUG, perror_msg);
      if (truncate (report_fn, 0) == EXIT_SUCCESS)
        _PASS;
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  /// If report file can not be written, print error and exit
  sprintf (perror_msg, "report_fp = fopen('%s', 'a')", report_fn);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  report_fp = fopen (report_fn, "a");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Initialize HTML report file
  retVal = init_report (report_fp);
  if (retVal != EXIT_SUCCESS)
    opal_exit (retVal);

  /// Call MARC functions to pre-process source file
  banner ("MARC start.");

  /// Create and open temp destination file for remove_comments()
  char *rc_tmp = "tmp/marc_rc.tmp";
  logger(DEBUG, "rc_tmp: '%s'", rc_tmp);

  /// If temp file can not be written, print error and exit
  sprintf (perror_msg, "rc_fp = fopen('%s', 'wb')", rc_tmp);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  FILE *rc_fp = fopen (rc_tmp, "wb");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Remove comments from source with rem_comments(), write to rc_tmp
  retVal = rem_comments (source_fp, rc_fp);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  if (!quiet)
    fprintf(stdout, "Removed comments from source file.\n");

  /// Close source file pointer source_fp if not NULL
  sprintf (perror_msg, "fclose(source_fp)");
  logger(DEBUG, perror_msg);
  if (source_fp)
    {
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

  /// Close rem_comments() temp file pointer rc_fp if not NULL
  sprintf (perror_msg, "fclose(rc_fp)");
  logger(DEBUG, perror_msg);
  if (rc_fp)
    {
      if (fclose (rc_fp) == EXIT_SUCCESS)
        {
          _PASS;
          rc_fp = NULL;
        }
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  /// Open rem_comments() temp file in read mode, else print error and exit
  sprintf (perror_msg, "rc_fp = fopen('%s', 'r')", rc_tmp);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  rc_fp = fopen (rc_tmp, "r");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Create and open temp destination file for proc_includes()
  char *pi_tmp = "tmp/marc_pi.tmp";
  logger(DEBUG, "pi_tmp: '%s'", pi_tmp);

  /// If temp file can not be written, print error and exit
  sprintf (perror_msg, "pi_fp = fopen('%s', 'wb')", pi_tmp);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  FILE *pi_fp = fopen (pi_tmp, "wb");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Process #include directives from source with proc_includes()
  retVal = proc_includes (rc_fp, pi_fp);
  if (retVal != EXIT_SUCCESS)
    {
      return (opal_exit (retVal));
    }

  if (!quiet)
    fprintf(stdout, "Processed #include files.\n");

  /// Close rem_comments temp file pointer if not NULL
  if (rc_fp)
    {
      sprintf (perror_msg, "fclose(rc_fp)");
      logger(DEBUG, perror_msg);

      if (fclose (rc_fp) == EXIT_SUCCESS)
        {
          _PASS;
          rc_fp = NULL;
        }
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  /// Close proc_includes() temp file pointer if not NULL
  if (pi_fp)
    {
      sprintf (perror_msg, "fclose(pi_fp)");
      logger(DEBUG, perror_msg);

      if (fclose (pi_fp) == EXIT_SUCCESS)
        {
          _PASS;
          pi_fp = NULL;
        }
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  /// Open proc_includes() temp file in read mode, else print error and exit
  sprintf (perror_msg, "pi_fp = fopen('%s', 'r')", pi_tmp);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  pi_fp = fopen (pi_tmp, "r");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Open rem_comments() temp file in write mode, else print error and exit
  sprintf (perror_msg, "rc_fp = fopen('%s', 'wb')", rc_tmp);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  rc_fp = fopen (rc_tmp, "wb");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }


  if (!quiet)
    fprintf(stdout, "Removed comments from included files.\n");

  /// Remove comments from includes files with rem_comments(), write to rc_tmp
  retVal = rem_comments (pi_fp, rc_fp);
  if (retVal != EXIT_SUCCESS)
    {
      return (opal_exit (retVal));
    }

  /// Close proc_includes() temp file pointer, else print error and exit
  sprintf (perror_msg, "fclose(pi_fp)");
  logger(DEBUG, perror_msg);
  if (fclose (pi_fp) == EXIT_SUCCESS)
    {
      _PASS;
      pi_fp = NULL;
    }
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Close rem_comments() temp file pointer, else print error and exit
  sprintf (perror_msg, "fclose(rc_fp)");
  logger(DEBUG, perror_msg);
  if (fclose (rc_fp) == EXIT_SUCCESS)
    {
      _PASS;
      rc_fp = NULL;
    }
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Open rem_comments() temp file in read mode, else print error and exit
  sprintf (perror_msg, "rc_fp = fopen('%s', 'r')", rc_tmp);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  rc_fp = fopen (rc_tmp, "r");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Append MARC output to HTML report
  retVal = print_marc_html (rc_fp, report_fp);
  if (retVal != EXIT_SUCCESS)
    {
      return (opal_exit (retVal));
    }

  /// Close rem_comments() temp file pointer, else print error and exit
  sprintf (perror_msg, "fclose(rc_fp)");
  logger(DEBUG, perror_msg);
  if (fclose (rc_fp) == EXIT_SUCCESS)
    {
      _PASS;
      rc_fp = NULL;
    }
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Start lexical analyzer code
  banner ("ALEX start.");

  /// Open rem_comments() temp file as source_fp, else print error and exit
  sprintf (perror_msg, "source_fp = fopen('%s', 'r')", rc_tmp);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  source_fp = fopen (rc_tmp, "r");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Create symbol table linked list
  logger(DEBUG, "Create symbol_table linked list node.");
  lexeme_s *symbol_table = (lexeme_s*) calloc (1, sizeof(lexeme_s));

  int symbol_count = 0;                ///< Number of lexemes identified

  if (!quiet)
    fprintf(stdout, "Symbol table of lexemes created.\n");

  /// Build symbol table using rem_comments() temp file as source
  retVal = build_symbol_table (symbol_table, &symbol_count);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  logger(DEBUG, "assert(symbol_ct [%d] > 0)", symbol_count);
  assert(symbol_count > 0);
  _PASS;

  /// Create and open temp destination file for print_symbol_table()
  char *alex_tmp = "tmp/alex.tmp";
  logger(DEBUG, "alex_tmp: '%s'", alex_tmp);

  /// If alex temp file can not be written, print error and exit
  sprintf (perror_msg, "alex_fp = fopen('%s', 'wb')", alex_tmp);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  FILE *alex_fp = fopen (alex_tmp, "wb");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Print symbol table with print_symbol_table() to alex temp file
  retVal = print_symbol_table (symbol_table, alex_fp);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  /// Print symbol table HTML report with print_symbol_table_html()
  retVal = print_symbol_table_html (symbol_table, report_fp);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  if (alex_fp)
    {
      sprintf (perror_msg, "fclose(alex_fp)");
      logger(DEBUG, perror_msg);
      if (fclose (alex_fp) == EXIT_SUCCESS)
        {
          _PASS;
          alex_fp = NULL;
        }
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  /// Start syntax analyzer code
  banner ("ASTRO start.");

  /// Build abstract syntax tree using symbol table
  node_s *syntax_tree = build_syntax_tree (symbol_table);

  logger(DEBUG, "assert(syntax_tree)");
  assert(syntax_tree);
  _PASS;

  if (!quiet)
    fprintf(stdout, "Abstract Syntax Tree created.\n");

  /// Print abstract syntax tree HTML report with print_ast_html()
  fprintf (report_fp, "<h3>Output by syntax analyzer <code>ASTRO</code></h3>\n"
           "<hr>\n");
  retVal = print_ast_html (syntax_tree, report_fp);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  /// Optimize the abstract syntax tree
  node_s *syntax_tree_pass1 = optimize_syntax_tree (syntax_tree);
  node_s *syntax_tree_pass2 = optimize_syntax_tree (syntax_tree_pass1);

  if (!quiet)
    fprintf(stdout, "Abstract Syntax Tree optimization done.\n");

  /// Print optimized syntax tree HTML report with print_ast_html()
  fprintf (report_fp, "<h3>Optimized abstract syntax tree: </h3>\n<hr>\n");
  retVal = print_ast_html (syntax_tree_pass2, report_fp);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  /// Start code generator
  banner ("GENIE start.");

  /// Build assembly code table using
  gen_asm_code (syntax_tree_pass2);
  add_asm_code (asm_HALT, 0, NULL);

  if (!quiet)
    fprintf(stdout, "Assembly code generated.\n");

  /// Create and open temp destination file for print_asm_code()
  char *asm_tmp = "tmp/asm.tmp";
  logger(DEBUG, "asm_tmp: '%s'", asm_tmp);

  /// If asm temp file can not be written, print error and exit
  sprintf (perror_msg, "asm_fp = fopen('%s', 'wb')", asm_tmp);
  logger(DEBUG, perror_msg);
  errno = EXIT_SUCCESS;
  FILE *asm_fp = fopen (asm_tmp, "wb");
  if (errno == EXIT_SUCCESS)
    _PASS;
  else
    {
      perror (perror_msg);
      _FAIL;
      return (errno);
    }

  /// Print symbol table with print_symbol_table() to assembly file
  retVal = print_asm_code (asm_cmd_list, asm_fp);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  /// Close asm temp file pointer asm_fp if not NULL
  sprintf (perror_msg, "fclose(asm_fp)");
  logger(DEBUG, perror_msg);
  if (asm_fp)
    {
      if (fclose (asm_fp) == EXIT_SUCCESS)
        {
          _PASS;
          asm_fp = NULL;
        }
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  /// Print assembly code with print_asm_code_html()
  retVal = print_asm_code_html (asm_cmd_list, report_fp);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  /// Start orchestrator
  banner ("ORCHESTRATOR start.");

  /// If object object file exists, delete it
  char *obj_fn = "tmp/nasm.o";
  sprintf (perror_msg, "access('%s', F_OK)", obj_fn);
  logger(DEBUG, perror_msg);
  if (access (obj_fn, F_OK) == EXIT_SUCCESS)
    {
      sprintf (perror_msg, "remove(%s)", obj_fn);
      logger(DEBUG, perror_msg);
      if (remove (obj_fn) == EXIT_SUCCESS)
        _PASS;
      else
        {
          perror (perror_msg);
          _FAIL;
          return (errno);
        }
    }

  /// Assemble object using NASM
  retVal = gen_obj (asm_tmp, obj_fn);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  if (!quiet)
    fprintf(stdout, "Assemble object file using 'NASM'.\n");

  /// Link object using LD
  retVal = gen_bin (obj_fn, dest_fn);
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  if (!quiet)
    fprintf(stdout, "Link object file using 'ld'.\n");

  /// Close HTML report file
  retVal = close_report (report_fp);
  if (retVal != EXIT_SUCCESS)
    opal_exit (retVal);

  if (!quiet)
    fprintf(stdout, "Output file:\t%s\nCompilation report:\t%s\n",
            dest_fn, report_fn);

  /// Free memory used by symbol_table
  free_symbol_table (symbol_table);
  symbol_table = NULL;

  /// Free memory used by syntax_tree
  free_syntax_tree (syntax_tree);
  syntax_tree = NULL;

  /// Free memory used by ASM array
  retVal = free_asm_arrays();
  if (retVal != EXIT_SUCCESS)
    return (opal_exit (retVal));

  /// source_fp, dest_fp, log_fp & report_fp closed by opal_exit()
  return (opal_exit (EXIT_SUCCESS));
}
