; github.com/torvalds/linux/blob/master/arch/x86/entry/syscalls/syscall_64.tbl
%define SYS_READ  0
%define SYS_WRITE 1
%define SYS_OPEN  2
%define SYS_EXIT 60

; pubs.opengroup.org/onlinepubs/9699919799/basedefs/unistd.h.html
%define STDIN     0
%define STDOUT    1
%define STDERR    2

; pubs.opengroup.org/onlinepubs/9699919799/basedefs/stdlib.h.html
%define EXIT_SUCCESS 0
%define EXIT_FAILURE 1

; Constants for better code readability
%define NULL    0
%define isTrue  1
%define isFalse 0

; =============================================================================
; Arithematic instructions
; =============================================================================

; -----------------------------------------------------------------------------
; Macro - O_ADD
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Sum of integers on top of stack
; Desc  - Push (stack[-1] + stack[-2]) on stack
; -----------------------------------------------------------------------------
%macro O_ADD 0
  POP  RAX                  ; Get 'a' from stack
  POP  RBX                  ; Get 'b' from stack
  ADD  RAX, RBX             ; Sum a + b
  PUSH RAX                  ; Push sum onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_SUB
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Different of integers on top of stack
; Desc  - Push (stack[-2] - stack[-1]) on stack
; -----------------------------------------------------------------------------
%macro O_SUB 0
  POP  RBX                  ; Get 'a' from stack
  POP  RAX                  ; Get 'b' from stack
  SUB  RAX, RBX             ; Subtract a - b
  PUSH RAX                  ; Push difference onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_NEGATE
; Args  - None
; Pre   - Operand integer on top of stack
; Post  - Negative of integer on stack
; Desc  - Push negative of stack[-1] on stack
; -----------------------------------------------------------------------------
%macro O_NEGATE 0
  POP   RAX                 ; Get 'a' from stack
  NEG   RAX                 ; Negate a
  PUSH  RAX                 ; Push -(a) onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_MUL
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Product of integers on top of stack
; Desc  - Push (stack[-1] * stack[-2]) on stack
; -----------------------------------------------------------------------------
%macro O_MUL 0
  POP  RAX                  ; Get 'a' from stack
  POP  RBX                  ; Get 'b' from stack
  IMUL RBX                  ; Multiply a * b
  PUSH RAX                  ; Push product onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_DIV
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Quotient of integer division on stack
; Desc  - Push (stack[-2] / stack[-1]) on stack
; -----------------------------------------------------------------------------
%macro O_DIV 0
  POP  RBX                  ; Get 'b' from stack
  POP  RAX                  ; Get 'a' from stack
  XOR  RDX, RDX             ; Clear for division
  IDIV RBX                  ; Divide a / b
  PUSH RAX                  ; Push dividend onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_MOD
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Remainder of integer division on stack
; Desc  - Push (stack[-2] % stack[-1]) on stack
; -----------------------------------------------------------------------------
%macro O_MOD 0
  POP  RBX                  ; Get 'b' from stack
  POP  RAX                  ; Get 'a' from stack
  XOR  RDX, RDX             ; Clear for division
  IDIV RBX                  ; Divide a / b
  PUSH RDX                  ; Push remainder onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_EQ
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Integer comparison result on stack
; Desc  - If stack[-1] == stack[-2], push isTrue on stack, else isFalse
; -----------------------------------------------------------------------------
%macro O_EQ 0
  POP  RBX                  ; Get 'b' from stack
  POP  RAX                  ; Get 'a' from stack
  CMP  RAX, RBX             ; a ?? b
  JNE  %%a_neq_b
  PUSH isTrue               ; a == b
  JMP  %%end
%%a_neq_b:
  PUSH isFalse              ; a != b
%%end:
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_NEQ
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Integer comparison result on stack
; Desc  - If stack[-1] != stack[-2], push isTrue on stack, else isFalse
; -----------------------------------------------------------------------------
%macro O_NEQ 0
  POP  RBX                  ; Get 'b' from stack
  POP  RAX                  ; Get 'a' from stack
  CMP  RAX, RBX             ; a ?? b
  JE   %%a_eq_b
  PUSH isTrue               ; a != b
  JMP  %%end
%%a_eq_b:
  PUSH isFalse              ; a == b
%%end:
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_LSS
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Integer comparison result on stack
; Desc  - If stack[-2] < stack[-1], push isTrue on stack, else isFalse
; -----------------------------------------------------------------------------
%macro O_LSS 0
  POP  RBX                  ; Get 'b' from stack
  POP  RAX                  ; Get 'a' from stack
  CMP  RAX, RBX             ; a ?? b
  JNL  %%a_geq_b
  PUSH isTrue               ; a < b
  JMP  %%end
%%a_geq_b:
  PUSH isFalse              ; a >= b
%%end:
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_GTR
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Integer comparison result on stack
; Desc  - If stack[-2] > stack[-1], push isTrue on stack, else isFalse
; -----------------------------------------------------------------------------
%macro O_GTR 0
  POP  RBX                  ; Get 'b' from stack
  POP  RAX                  ; Get 'a' from stack
  CMP  RAX, RBX             ; a ?? b
  JNG  %%a_leq_b
  PUSH isTrue               ; a > b
  JMP  %%end
%%a_leq_b:
  PUSH isFalse              ; a <= b
%%end:
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_LEQ
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Integer comparison result on stack
; Desc  - If stack[-2] <= stack[-1], push isTrue on stack, else isFalse
; -----------------------------------------------------------------------------
%macro O_LEQ 0
  POP  RBX                  ; Get 'b' from stack
  POP  RAX                  ; Get 'a' from stack
  CMP  RAX, RBX             ; a ?? b
  JG   %%a_gtr_b
  PUSH isTrue               ; a <= b
  JMP  %%end
%%a_gtr_b:
  PUSH isFalse              ; a > b
%%end:
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_GEQ
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Integer comparison result on stack
; Desc  - If stack[-2] >= stack[-1], push isTrue on stack, else isFalse
; -----------------------------------------------------------------------------
%macro O_GEQ 0
  POP  RBX                  ; Get 'b' from stack
  POP  RAX                  ; Get 'a' from stack
  CMP  RAX, RBX             ; a ?? b
  JL  %%a_less_b
  PUSH isTrue               ; a >= b
  JMP  %%end
%%a_less_b:
  PUSH isFalse              ; a < b
%%end:
%endmacro

; =============================================================================
; Logical instructions
; =============================================================================

; -----------------------------------------------------------------------------
; Macro - O_AND
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Logical AND result of integers on stack
; Desc  - If stack[-1] && stack[-2], push isTrue on stack, else isFalse
; -----------------------------------------------------------------------------
%macro O_AND 0
  POP  RAX               ; Get 'a' from stack
  POP  RBX               ; Get 'b' from stack
  AND  RAX, RBX          ; a && b
  JNZ  %%a_and_b
  PUSH isFalse           ; If (a && b) is zero, push isFalse ..
  JMP  %%end
%%a_and_b:
  PUSH isTrue            ; .. else, push isTrue
%%end:
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_OR
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Logical OR result of integers on stack
; Desc  - If stack[-1] || stack[-2], push isTrue on stack, else isFalse
; -----------------------------------------------------------------------------
%macro O_OR 0
  POP  RAX               ; Get 'a' from stack
  POP  RBX               ; Get 'b' from stack
  OR   RAX, RBX          ; a || b
  JNZ  %%a_or_b
  PUSH isFalse           ; If (a || b) is zero, push isFalse ..
  JMP  %%end
%%a_or_b:
  PUSH isTrue            ; .. else, push isTrue
%%end:
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_NOT
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Logical NOT result of integer on top of stack
; Desc  - If stack[-1] is non-zero, push isFalse, if value is 0, push isTrue
; -----------------------------------------------------------------------------
%macro O_NOT 0
  POP  RAX               ; Get integer from stack
  CMP  RAX, 0            ; Compare value with 0
  JNE  %%nz
  PUSH isTrue            ; If value is zero, push isTrue on stack ..
  JMP  %%end
%%nz:
  PUSH isFalse           ; .. else, push isFalse on stack
%%end:
%endmacro

; =============================================================================
; Data operation instructions
; =============================================================================

; -----------------------------------------------------------------------------
; Macro - _INPUT_
; Args  - None
; Pre   - Prompt string index on top of stack
; Post  - User input integer on top of stack
; Desc  - Reads integer from user and pushes on top of stack
; -----------------------------------------------------------------------------
%macro _INPUT_ 0
  ;O_PRTS                 ; Print prompt string with macro

; Read digits from STDIN and store in buffer 'bss0' in a loop until newline
  XOR R9, R9             ; R9 will hold number of characters read
%%readi_start:
  MOV RDX, 1             ; Read 1 character ..
  MOV RDI, STDIN         ; .. of user input from STDIN ..
  MOV RAX, SYS_READ      ; .. with SYS_READ system call ..
  MOV RSI, char          ; .. and save character to memory location 'char'
  SYSCALL                ; Call kernel

  MOV AL, [char]         ; Move character read into RAX
  CMP AL, 0ah            ; If character is newline ..
  JE  %%readi_end        ; .. end reading user input

  MOV RAX, bss0          ; RAX points to buffer used for storage
  ADD RAX, R9            ; Increment address past current characters
  XOR RBX, RBX
  MOV BL, [char]         ; Copy the character to the BL register
  MOV [EAX], BL          ; Append character to the buffer 'bss0'
  INC R9                 ; Increment number of characters
  JMP %%readi_start      ; Read next character from screen
%%readi_end:

; Convert digits in buffer 'bss0' to integer
%%atoi:
  MOV RSI, bss0          ; RSI points to string to convert
  XOR RCX, RCX           ; RCX will hold number of digits processed so far
  XOR RAX, RAX           ; RAX will hold converted integer, starts off as 0
  XOR RBX, RBX           ; RBX will be used to convert ASCII to decimal
  XOR R8, R8             ; R8 will be the flag for negative value

  MOV BL, [RSI+RCX]      ; Read in the first character &'bss0+0'
  CMP BL, 45             ; If char is not -ve sign ..
  JNE %%isPositive       ; .. jump to label isPositive
  MOV R8, 1d             ; .. else set negative integer flag
  INC RCX                ; Move to second char in buffer
  DEC R9                 ; Decrement number of digits to be processed ..
  JMP %%atoi_loop        ; .. and convert string to integer

%%isPositive:
  XOR R8, R8             ; Clear negative integer flag

%%atoi_loop:
  XOR RBX, RBX
  MOV BL, [ESI+ECX]      ; Read in ASCII character to convert

  CMP BL, 48             ; If char ASCII value less than 0 ..
  JL  %%atoi_end         ; .. jump to end
  CMP BL, 57             ; If char ASCII value greater than 9 ..
  JG  %%atoi_end         ; .. jump to end

  SUB BL, 48             ; Get decimal value from ASCII
  ADD RAX, RBX           ; Add value to RAX

  DEC R9                 ; Decrement number of digits to be processed
  CMP R9, 0              ; If no more digits to process ..
  JE  %%atoi_end         ; .. jump to end

  MOV RBX, 10            ; Multiply current value in RAX by 10
  MUL RBX                ;
  INC RCX                ; Increment counter used for character address
  JMP %%atoi_loop        ; Process next digit

%%atoi_end:
  CMP R8, 1d             ; If negative integer flag is not set ..
  JNE %%push_val         ; .. jump to label push_val ..
  NEG RAX                ; .. else negate value

; Push integer value on top of stack
%%push_val:
  PUSH RAX               ; Push result integer value to top of stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - _FETCH_
; Args  - Array index
; Pre   - None
; Post  - Push value at data[index] on top of stack
; Desc  - Gets integer from array 'data[index]' and pushes it on top of stack
; -----------------------------------------------------------------------------
%macro _FETCH_ 1
  MOV  RAX,[data+(8*%1)] ; Get from [source] + (size) * index
  PUSH RAX               ; Push integer on top of stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - _STORE_
; Args  - Array index
; Pre   - Integer to store on top of stack
; Post  - data[index] = integer at top of stack
; Desc  - Stores integer from top of stack into array 'data' at given index
; -----------------------------------------------------------------------------
%macro _STORE_ 1
  POP  RAX               ; Get integer to store
  MOV  [data+(8*%1)],RAX ; Store in [destination] + (size) * index
%endmacro

; =============================================================================
; Logical instructions
; =============================================================================

; -----------------------------------------------------------------------------
; Macro - O_JZ
; Args  - Label to jump to
; Pre   - Value to compare with 0 on top of stack
; Post  - None
; Desc  - If top of stack is 0, jump to given label
; -----------------------------------------------------------------------------
%macro O_JZ 1
  POP  RAX               ; Get value from top of stack ..
  CMP  RAX, 0            ; .. and compare with zero
  JE   %1                ; If value is zero, jump to given label
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_JNZ
; Args  - Label to jump to
; Pre   - Value to compare with 0 on top of stack
; Post  - None
; Desc  - If top of stack is non-zero, jump to given label
; -----------------------------------------------------------------------------
%macro O_JNZ 1
  POP  RAX               ; Get value from top of stack ..
  CMP  RAX, 0            ; .. and compare with zero
  JNE  %1                ; If value is non-zero, jump to given label
%endmacro

; =============================================================================
; Print instructions
; =============================================================================

; -----------------------------------------------------------------------------
; Macro - O_PRTS
; Args  - None
; Pre   - strs[index] to print on top of stack
; Post  - None
; Desc  - Prints string at 'strs[index]' to STDOUT
; -----------------------------------------------------------------------------
%macro O_PRTS 0
  POP  RAX               ; Get index of string to print from stack

  MOV  RBX, 8d           ; Add (index*8) to array address ..
  IMUL RBX               ; .. to get string address

  MOV  RSI, [strs+RAX]   ; Get address of string to print
  MOV  RDX, [lens+RAX]   ; Get length of string to print
  MOV  RAX, SYS_WRITE    ; Use sys_write system call
  MOV  RDI, STDOUT       ; Output to stdout
  SYSCALL                ; Call kernel
  CMP  RDX, RAX          ; If sys_write wrote expected number of bytes ..
  JE   %%end             ; .. return from macro
  HALT RAX               ; .. else, exit with difference as code ..
%%end:
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_PRTS
; Args  - Char to print
; Pre   - None
; Post  - None
; Desc  - Print given character to STDOUT
; -----------------------------------------------------------------------------
%macro O_PRTS 1
  PUSH %1                ; Push char on stack
  MOV  RAX, SYS_WRITE    ; Use sys_write system call to print
  MOV  RDI, STDOUT       ; Output to stdout
  MOV  RSI, RSP          ; Print char on stack
  MOV  RDX, 1            ; Length
  SYSCALL                ; Call kernel
  CMP  RAX, RDX          ; If sys_write wrote expected number of bytes ..
  JE   %%end             ; .. return from macro
  HALT RAX               ; .. else, exit with difference as code ..
%%end:
  ADD RSP, 8             ; Remove char from stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_PRTI
; Args  - None
; Pre   - Integer to print on top of stack
; Post  - None
; Desc  - Prints integer on top of stack to STDOUT
; -----------------------------------------------------------------------------
%macro O_PRTI 0
  POP  RAX               ; Get integer from stack

  CMP  RAX, 0            ; Check if number is negative
  JGE  %%start           ; If number is positive, print number
  PUSH RAX               ; Backup number before printing -ve sign
  O_PRTS "-"             ; Print '-' sign using macro
  POP  RAX               ; Restore number after printing -ve sign
  NEG  RAX               ; If number is negative, get positive value
%%start:
  XOR  RSI, RSI          ; Zero out source index register
%%loop:
  XOR  RDX, RDX          ; Zero out quotient register
  MOV  RBX, 10d          ; Keep dividing number by 10
  DIV  RBX               ; to get remainder (digit) in RDX
  ADD  RDX, 48d          ; Add 48 to convert decimal to ASCII
  PUSH RDX               ; Push digits on stack
  INC  RSI               ; Increment source index register
  MOV  RBX, RSI          ; Move number of digits to RBX, for printing
  CMP  RAX, 0            ; If quotient is zero, all digits on stack
  JZ   %%next            ; If all digits on stack, print them
  JMP  %%loop            ; If quotient not zero, get next digit
%%next:
  CMP  RBX, 0            ; If source index (RBX) is zero, no more digits ..
  JZ   %%exit            ; .. to add to buffer
  MOV  RAX, SYS_WRITE    ; Use sys_write system call to print
  MOV  RDI, STDOUT       ; Output to stdout
  MOV  RSI, RSP          ; Print digit on stack
  MOV  RDX, 1            ; Length 1 byte per digit
  SYSCALL                ; Call kernel
  CMP  RAX, 1            ; If sys_write wrote more/less bytes ..
  JNE  %%error           ; .. exit with difference as code
  DEC  RBX               ; Decrement source index after every digit
  ADD  RSP, 8            ; Move to next digit
  JMP  %%next            ; Get next char to print
%%error:
  HALT RAX
%%exit:
%endmacro

; =============================================================================
; Execution instructions
; =============================================================================

; -----------------------------------------------------------------------------
; Macro - HALT
; Args  - None
; Pre   - None
; Post  - None
; Desc  - Runs SYS_EXIT system call with code - 0
; -----------------------------------------------------------------------------
%macro HALT 0
  MOV  RAX, SYS_EXIT     ; Use SYS_EXIT system call to exit ..
  MOV  RDI, 0            ; .. with exit code 0
  SYSCALL
%endmacro

; -----------------------------------------------------------------------------
; Macro - HALT
; Args  - Exit code
; Pre   - None
; Post  - None
; Desc  - Runs SYS_EXIT system call with given code
; -----------------------------------------------------------------------------
%macro HALT 1
  MOV  RAX, SYS_EXIT     ; Use SYS_EXIT system call to exit ..
  MOV  RDI, %1           ; .. with given argument as exit code
  SYSCALL
%endmacro

; =============================================================================
; Program instructions
; =============================================================================

SECTION .text
global _start
  _start:

  NOP
  ;=== User code start ===;
  PUSH	1
  _STORE_	0
_if_2:
  _FETCH_	0
  PUSH	5
  O_LSS
  O_JZ		_else_2
  PUSH	0
  O_PRTS
  JMP		_fi_2
_else_2:
_if_11:
  _FETCH_	0
  PUSH	5
  O_EQ
  O_JZ		_else_11
  PUSH	1
  O_PRTS
  JMP		_fi_11
_else_11:
  PUSH	2
  O_PRTS
_fi_11:
_fi_2:
  HALT
  ;=== User code end ===;

SECTION .bss
  bss0 RESB 255          ; reserve 255 bytes for user input

SECTION .data
  char  DB 0             ; Used for user input

  ;=== User variables ===;
  ; === Strings ===;
  msg0: DB "a is less than 5", 13, 10, "", NULL
  len0 EQU $ - msg0
  msg1: DB "a is equal to 5", 13, 10, "", NULL
  len1 EQU $ - msg1
  msg2: DB "a is greater than 5", 13, 10, "", NULL
  len2 EQU $ - msg2
  strs: DQ msg0, msg1, msg2, 
  lens: DQ len0, len1, len2, 
  ; === Integers ===;
  data  TIMES 1 DQ 0
