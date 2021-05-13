# Tests description

## MARC
 - Test01 - Multiple comments, single line comment at end of valid statement
 - Test02 - Single include file
 - Test03 - Multiple comments in file
 - Test04 - Single line comment with multi-line opening
 - Test05 - Single line comment with consecutive slashes inside, multi-line 
            open and close
 - Test06 - Multiple include files
 - Test07 - Error handling when include file does not exist
 - Test08 - Error handling when include file has restricted permissions
 - Test09 - CLI test for invalid case: 0 arguments & 0 flags provided.
 - Test10 - CLI test for invalid case: 1 argument & 1 valid flag provided.
 - Test11 - CLI test for invalid case: 1 argument & 1 invalid flag provided.
 - Test12 - CLI test for valid case: 2 arguments & 1 valid flag provided.
 - Test13 - CLI test for invalid case: 2 arguments & 1 invalid flag provided.
 - Test14 - CLI test for invalid case: >2 arguments & 1 valid flag provided.
 - Test15 - CLI test for invalid case: >2 arguments & 1 invalid flag provided.
 
## ALEX
 - Test16 - Test for parsing integers
 - Test17 - Test for parsing valid identifier lexemes
 - Test18 - Tests for parsing invalid identifier lexemes with periods
 - Test19 - Tests for parsing invalid identifier lexemes with symbols
 - Test20 - Tests for correct abstract syntax tree creation.
 - Test21 - Tests for correct abstract syntax tree creation.
 - Test22 - Negative test for correct abstract syntax tree creation.

## ASTRO
 - Test23 - Tests for correct order of operations
 - Test24 - Tests for Input and Not lexeme types

## GENIE
 - Test25 - Test assembly code generated for arithematic operations
 - Test26 - Test assembly code generated for while loop structure (TODO)
 - Test27 - Test assembly code generated for if/else control structure (TODO)
