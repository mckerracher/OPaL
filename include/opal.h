/// @file opal.h

#include <bits/types/FILE.h>
#include <stdbool.h>            /* boolean datatypes */
#include <stddef.h>

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
char *report_fn = NULL;         ///< Report file name

FILE *source_fp = NULL;         ///< Source file pointer
FILE *dest_fp = NULL;           ///< Destination file pointer
FILE *log_fp = NULL;            ///< Log file pointer
FILE *report_fp = NULL;         ///< Report file pointer

short retVal = 0;               ///< Function return value

int next_char = ' ';            ///< Next character in source file
int char_col = 0;               ///< Column number of character in source file
int char_line = 0;              ///< Line number of character in source file

/// Log level name enum for opal_log function
typedef enum log_level
{
  NONE, ERROR, INFO, DEBUG, RESULT
} log_level_e;
short LOG_LEVEL = ERROR;        ///< Current log level

/// Buffer used to populate error message string for perror()
#define perror_msg_len 1024
char perror_msg[perror_msg_len] = { 0 };

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

/// Struct for keyword string/type
typedef struct keyword
{
  const char *str;
  lexeme_type_e lex_type;
} keyword;

/// Array for supported keywords
keyword keyword_arr[] =
    {
        {"if", lx_If},
        {"else", lx_Else},
        {"while", lx_While},
        {"print", lx_Print},
        {"input", lx_Input}
    };

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
#define lexeme_str_len 1024
char lexeme_str[lexeme_str_len] = { 0 };

/// Extended regular expression pattern for integers
char *int_regex_pattern = "^[-+]?[0-9]+$";

/// Enum for abstract syntax tree node type
typedef enum ast_node_type
{
  nd_NOP = 0,
  nd_EOF,
  nd_Ident,
  nd_Integer,
  nd_String,
  nd_Assign,
  nd_Add,
  nd_Sub,
  nd_Negate,
  nd_Mul,
  nd_Div,
  nd_Mod,
  nd_Eq,
  nd_Neq,
  nd_Lss,
  nd_Gtr,
  nd_Leq,
  nd_Geq,
  nd_And,
  nd_Or,
  nd_Not,
  nd_If,
  nd_Else,
  nd_While,
  nd_Prts,
  nd_Prti,
  nd_Sequence,
  nd_Input,
} ast_node_type_e;

/// Syntax tree node type names for logging
const char node_name[][16] =
  { "No_operation", "End_of_file", "Identifier", "Integer", "String",
      "Op_Assign", "Op_Add", "Op_Subtract", "Op_Negate", "Op_Multiply",
      "Op_Divide", "Op_Mod", "Op_Equal", "Op_NotEqual", "Op_Less", "Op_Greater",
      "Op_LessEqual", "Op_GreaterEqual", "Op_And", "Op_Or", "Op_Not",
      "Keyword_If", "Keyword_Else", "Keyword_While", "Print string",
      "Print integer", "Code block", "Keyword_input" };

/// Struct for abstract syntax tree node
typedef struct node
{
  ast_node_type_e node_type;
  struct node *left;
  struct node *right;
  char *val;
} node_s;

/// Language grammar
typedef struct attributes
{
  char *text, *enum_text;
  lexeme_type_e lx_type;
  int right_associative, is_binary, is_unary;
  short precedence;
  ast_node_type_e node_type;
} attributes_s;

/**
 * Language grammar
 * Ref: https://en.wikipedia.org/wiki/Operators_in_C_and_C
 */
const attributes_s grammar[] =
  {
    { "NOP", "No_Operation", lx_NOP, FALSE, FALSE, FALSE, -1, nd_NOP },
    { "EOF", "End_of_file", lx_EOF, FALSE, FALSE, FALSE, -1, -1 },
    { "Identifier", "Identifier", lx_Ident, FALSE, FALSE, FALSE, -1, nd_Ident },
    { "Integer", "Integer", lx_Integer, FALSE, FALSE, FALSE, -1, nd_Integer },
    { "String", "String", lx_String, FALSE, FALSE, FALSE, -1, nd_String },
    { "=", "Op_assign", lx_Assign, FALSE, FALSE, FALSE, -1, nd_Assign },
    { "+", "Op_add", lx_Add, FALSE, TRUE, FALSE, 12, nd_Add },
    { "-", "Op_subtract", lx_Sub, FALSE, TRUE, FALSE, 12, nd_Sub },
    { "-", "Op_negate", lx_Negate, FALSE, FALSE, TRUE, 14, nd_Negate },
    { "*", "Op_multiply", lx_Mul, FALSE, TRUE, FALSE, 13, nd_Mul },
    { "/", "Op_divide", lx_Div, FALSE, TRUE, FALSE, 13, nd_Div },
    { "%", "Op_mod", lx_Mod, FALSE, TRUE, FALSE, 13, nd_Mod },
    { "==", "Op_equal", lx_Eq, FALSE, TRUE, FALSE, 9, nd_Eq },
    { "!=", "Op_notequal", lx_Neq, FALSE, TRUE, FALSE, 9, nd_Neq },
    { "<", "Op_less", lx_Lss, FALSE, TRUE, FALSE, 10, nd_Lss },
    { ">", "Op_greater", lx_Gtr, FALSE, TRUE, FALSE, 10, nd_Gtr },
    { "<=", "Op_lessequal", lx_Leq, FALSE, TRUE, FALSE, 10, nd_Leq },
    { ">=", "Op_greaterequal", lx_Geq, FALSE, TRUE, FALSE, 10, nd_Geq },
    { "&&", "Op_and", lx_And, FALSE, TRUE, FALSE, 5, nd_And },
    { "||", "Op_or", lx_Or, FALSE, TRUE, FALSE, 4, nd_Or },
    { "!", "Op_not", lx_Not, FALSE, FALSE, TRUE, 14, nd_Not },
    { "if", "Keyword_if", lx_If, FALSE, FALSE, FALSE, -1, nd_If },
    { "else", "Keyword_else", lx_Else, FALSE, FALSE, FALSE, -1, -1 },
    { "while", "Keyword_while", lx_While, FALSE, FALSE, FALSE, -1, nd_While },
    { "(", "LeftParen", lx_Lparen, FALSE, FALSE, FALSE, -1, -1 },
    { ")", "RightParen", lx_Rparen, FALSE, FALSE, FALSE, -1, -1 },
    { "{", "LeftBrace", lx_Lbrace, FALSE, FALSE, FALSE, -1, -1 },
    { "}", "RightBrace", lx_Rbrace, FALSE, FALSE, FALSE, -1, -1 },
    { ";", "Semicolon", lx_Semi, FALSE, FALSE, FALSE, -1, -1 },
    { ",", "Comma", lx_Comma, FALSE, FALSE, FALSE, -1, -1 },
    { "print", "Keyword_print", lx_Print, FALSE, FALSE, FALSE, -1, -1 },
  };

/// Lexeme currently being processed by build_syntax_tree()
lexeme_s *ast_curr_lexeme = NULL;


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
int read_next_char(void);
/// Initialize HTML report
short init_report (FILE*);

/*
 * ==================================
 * MARC FUNCTION DECLARATIONS
 * ==================================
 */
/// Read source, remove comments, write to destination
short rem_comments(FILE*, FILE*);
/// Process include files, write to destination
short proc_includes(FILE*, FILE*);
/// Append MARC output to HTML report file
short print_marc_html(FILE*, FILE*);

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
short get_lexeme_str(const lexeme_s*, char*, int);
/// Populate symbol table with lexemes in source file pointer
short build_symbol_table (lexeme_s*, int*);
/// Print symbol table to destination file pointer
short print_symbol_table (lexeme_s*, FILE*);
/// Determine if regular expression is an integer
bool match(const char *str, const char *pattern);
/// Print symbol table to HTML report
short print_symbol_table_html (lexeme_s*, FILE*);
/// Free symbol table linked list
void free_symbol_table (lexeme_s*);

/*
 * ==================================
 * ASTRO FUNCTION DECLARATIONS
 * ==================================
 */
/// Build abstract syntax tree from symbol table
node_s* build_syntax_tree (lexeme_s*);
/// Build and return statement node
node_s* make_statement_node(void);
/// Build and return expression inside parantheses
node_s *make_parentheses_expression(void);
/// Build expression node
node_s *make_expression_node(int);
/// Check if lexeme is expected type, else print error and exit
void expect(lexeme_type_e);
/// Build and return leaf nodes for identifier/integer/strings
node_s *make_leaf_node(lexeme_type_e, lexeme_s*);
/// Print abstract syntax tree to destination file
short print_ast (node_s*, FILE*);
/// Print abstract syntax tree to HTML report
short print_ast_html (node_s*, FILE*);
/// Free syntax tree
void free_syntax_tree (node_s*);

#endif /* OPAL_H_ */
