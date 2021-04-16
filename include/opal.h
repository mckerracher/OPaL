/// @file opal.h
#ifndef OPAL_H_
#define OPAL_H_

/// __VERSION_NUM for program
#ifndef __VERSION_NUM
#define __VERSION_NUM 1.0
#endif  /* __VERSION_NUM */

#define TRUE 1          ///< Alias for better code readability
#define FALSE 0         ///< Alias for better code readability

/*
 * ==================================
 * Macros used to print messages with caller file name, line number and
 * function, followed the formatted string & status like PASS, FAIL etc
 * ==================================
 */
#define logger(tag, ...) \
  opal_log(tag, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define _PASS (logger(RESULT, " - PASS"))   ///< Macro function to log PASS
#define _FAIL (logger(RESULT, " - FAIL"))   ///< Macro function to log FAIL
#define _DONE (logger(RESULT, " .. DONE"))  ///< Macro function to log DONE

/*
 * ==================================
 * Common data structures and variables used
 * ==================================
 */

char *source_fn = NULL;         ///< Input source file name
char *dest_fn = NULL;           ///< Destination file name
char *log_fn = NULL;            ///< Log file name

FILE *source_fd = NULL;         ///< Source file pointer
FILE *dest_fd = NULL;           ///< Destination file pointer
FILE *log_fd = NULL;            ///< Log file pointer

short retVal = 0;               ///< Function return value

int next_char = ' ';            ///< Next character in source file descriptor
int char_col = 0;               ///< Column number of character in source file
int char_line = 0;              ///< Line number of character in source file

/// Log level name enum for opal_log function
typedef enum log_level
{
  NONE, ERROR, INFO, DEBUG, RESULT
} log_level_e;
short LOG_LEVEL = ERROR;        ///< Current log level

/// Buffer used to populate error message string for perror()
char perror_msg[1024] = { 0 };

/*
 * ==================================
 * ALEX data structures and variables used
 * ==================================
 */
/// Lexeme types enum
typedef enum lexeme_type
{
  lx_NOP = 0,
  lx_EOF,
  lx_Ident,
  lx_Integer,
  lx_String,
  lx_Assign,
  lx_Add,
  lx_Sub,
  lx_Negate,
  lx_Mul,
  lx_Div,
  lx_Mod,
  lx_Eq,
  lx_Neq,
  lx_Lss,
  lx_Gtr,
  lx_Leq,
  lx_Geq,
  lx_And,
  lx_Or,
  lx_Not,
  lx_If,
  lx_Else,
  lx_While,
  lx_Lparen,
  lx_Rparen,
  lx_Lbrace,
  lx_Rbrace,
  lx_Semi,
  lx_Comma,
  lx_Print,
  lx_Input
} lexeme_type_e;

/// Lexeme type names for logging
const char op_name[][16] =
  { "No_operation", "End_of_file", "Identifier", "Integer", "String",
      "Op_Assign", "Op_Add", "Op_Subtract", "Op_Negate", "Op_Multiply",
      "Op_Divide", "Op_Mod", "Op_Equal", "Op_NotEqual", "Op_Less", "Op_Greater",
      "Op_LessEqual", "Op_GreaterEqual", "Op_And", "Op_Or", "Op_Not",
      "Keyword_If", "Keyword_Else", "Keyword_While", "LeftParen", "RightParen",
      "LeftBrace", "RightBrace", "Semicolon", "Comma", "Keyword_print",
      "Keyword_input" };

/// Struct for lexeme in the symbol table linked list
typedef struct lexeme
{
  lexeme_type_e type;
  int line;
  int column;
  int int_val;
  char *char_val;
  struct lexeme *next;
} lexeme_s;

/// Struct to hold next lexeme
lexeme_s next_lexeme = { 0 };

/// A buffer to hold string value of lexeme
char next_lexeme_str[1024] = { 0 };

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
/// Read next character from source file
int read_next_char();

/*
 * ==================================
 * MARC FUNCTION DECLARATIONS
 * ==================================
 */
/// Read source, remove comments, write to destination
short rem_comments(FILE*, FILE*);
/// Process include files, write to destination
short proc_includes(FILE*, FILE*);

/*
 * ==================================
 * ALEX FUNCTION DECLARATIONS
 * ==================================
 */
/// Get lexeme for a string literal
lexeme_s get_string_literal_lexeme(int, int);
/// Get lexeme for binary or unary operator
lexeme_type_e binary_unary (char, lexeme_type_e, lexeme_type_e, int, int);
/// Get identifier lexeme
lexeme_s get_identifier_lexeme (int, int);
/// Get the next lexeme
lexeme_s get_next_lexeme(void);
/// Stringify lexeme
short get_lexeme_str(lexeme_s, char*);
/// Populate symbol table with lexemes in source file descriptor
short build_symbol_table (lexeme_s*, int*);
/// Print symbol table to destination file descriptor
short print_symbol_table (lexeme_s*, FILE*);

#endif /* OPAL_H_ */
