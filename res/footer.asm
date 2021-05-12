  ;=== User code end ===;

SECTION .bss
  bss0 RESB 255          ; reserve 255 bytes for user input

SECTION .data
  char  DB 0             ; Used for user input

  ;=== User variables ===;
