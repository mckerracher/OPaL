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
%define NULL 	0
%define isTrue	1
%define isFalse	0

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
  POP	RAX					; Get 'a' from stack
  POP	RBX					; Get 'b' from stack
  ADD	RAX, RBX			; Sum a + b
  PUSH	RAX					; Push sum onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_SUB
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Different of integers on top of stack
; Desc  - Push (stack[-2] - stack[-1]) on stack
; -----------------------------------------------------------------------------
%macro O_SUB 0
  POP	RBX					; Get 'a' from stack
  POP	RAX					; Get 'b' from stack
  SUB	RAX, RBX			; Subtract a - b
  PUSH	RAX					; Push difference onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_NEGATE
; Args  - None
; Pre   - Operand integer on top of stack
; Post  - Negative of integer on stack
; Desc  - Push negative of stack[-1] on stack
; -----------------------------------------------------------------------------
%macro O_NEGATE 0
  POP 	RAX					; Get 'a' from stack
  NEG	RAX					; Negate a
  PUSH 	RAX					; Push -(a) onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_MUL
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Product of integers on top of stack
; Desc  - Push (stack[-1] * stack[-2]) on stack
; -----------------------------------------------------------------------------
%macro O_MUL 0
  POP	RAX					; Get 'a' from stack
  POP	RBX					; Get 'b' from stack
  IMUL	RBX					; Multiply a * b
  PUSH	RAX					; Push product onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_DIV
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Quotient of integer division on stack
; Desc  - Push (stack[-2] / stack[-1]) on stack
; -----------------------------------------------------------------------------
%macro O_DIV 0
  POP	RBX					; Get 'b' from stack
  POP	RAX					; Get 'a' from stack
  XOR   RDX, RDX			; Clear for division
  IDIV	RBX					; Divide a / b
  PUSH	RAX					; Push dividend onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_MOD
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Remainder of integer division on stack
; Desc  - Push (stack[-2] % stack[-1]) on stack
; -----------------------------------------------------------------------------
%macro O_MOD 0
  POP	RBX					; Get 'b' from stack
  POP	RAX					; Get 'a' from stack
  XOR   RDX, RDX			; Clear for division
  IDIV	RBX					; Divide a / b
  PUSH	RDX					; Push remainder onto stack
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_EQ
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Integer comparison result on stack
; Desc  - If stack[-1] == stack[-2], push isTrue on stack, else isFalse
; -----------------------------------------------------------------------------
%macro O_EQ 0
  POP	RBX					; Get 'b' from stack
  POP	RAX					; Get 'a' from stack
  CMP	RAX, RBX			; a ?? b
  JNE	%%a_neq_b
  PUSH	isTrue				; a == b
  JMP	%%end
%%a_neq_b:
  PUSH  isFalse				; a != b
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
  POP	RBX					; Get 'b' from stack
  POP	RAX					; Get 'a' from stack
  CMP	RAX, RBX			; a ?? b
  JE	%%a_eq_b
  PUSH	isTrue				; a != b
  JMP	%%end
%%a_eq_b:
  PUSH  isFalse				; a == b
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
  POP	RBX					; Get 'b' from stack
  POP	RAX					; Get 'a' from stack
  CMP	RAX, RBX			; a ?? b
  JNL	%%a_geq_b
  PUSH	isTrue				; a < b
  JMP	%%end
%%a_geq_b:
  PUSH isFalse				; a >= b
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
  POP	RBX					; Get 'b' from stack
  POP	RAX					; Get 'a' from stack
  CMP	RAX, RBX			; a ?? b
  JNG	%%a_leq_b
  PUSH	isTrue				; a > b
  JMP	%%end
%%a_leq_b:
  PUSH isFalse				; a <= b
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
  POP	RBX					; Get 'b' from stack
  POP	RAX					; Get 'a' from stack
  CMP	RAX, RBX			; a ?? b
  JG	%%a_gtr_b
  PUSH	isTrue				; a <= b
  JMP	%%end
%%a_gtr_b:
  PUSH  isFalse				; a > b
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
  POP	RBX					; Get 'b' from stack
  POP	RAX					; Get 'a' from stack
  CMP	RAX, RBX			; a ?? b
  JL	%%a_less_b
  PUSH	isTrue				; a >= b
  JMP	%%end
%%a_less_b:
  PUSH  isFalse				; a < b
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
  PUSH isFalse
  JMP  %%end
%%a_and_b:
  PUSH isTrue
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
  PUSH isFalse
  JMP  %%end
%%a_or_b:
  PUSH isTrue
%%end:
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_NOT
; Args  - None
; Pre   - Operand integers on top of stack
; Post  - Logical NOT result of integer on top of stack
; Desc  - If stack[-1] is non-zero, push 0, if value is 0, push 1
; -----------------------------------------------------------------------------
%macro O_NOT 0
  POP  RAX               ; Get integer from stack
  CMP  RAX, 0            ; Compare value with 0
  JNE  %%nz
  PUSH 1                 ; If 0, push 1 on stack
  JMP  %%end
%%nz:
  PUSH 0                 ; If non-zero, push 0 on stack
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
  ; TBD
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
  POP  RAX
  CMP  RAX, 0
  JE   %1
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_JNZ
; Args  - Label to jump to
; Pre   - Value to compare with 0 on top of stack
; Post  - None
; Desc  - If top of stack is non-zero, jump to given label
; -----------------------------------------------------------------------------
%macro O_JNZ 1
  POP  RAX
  CMP  RAX, 0
  JNE  %1
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
  ; TBD
%endmacro

; -----------------------------------------------------------------------------
; Macro - O_PRTI
; Args  - None
; Pre   - Integer to print on top of stack
; Post  - None
; Desc  - Prints integer on top of stack to STDOUT
; -----------------------------------------------------------------------------
%macro O_PRTI 0
  ; TBD
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
  MOV  RAX, SYS_EXIT
  MOV  RDI, 0
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
