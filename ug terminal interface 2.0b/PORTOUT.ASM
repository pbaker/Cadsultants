; portout.asm
;
;  subroutine to output a character to COM1: or COM2:
;
  codes segment
;
  public ugout,trmout
;
  assume cs:codes
;
;---------------------------------------------------------------
;  output to unigraphics
;
  ugout proc far
;
  push ds  ; save registers on stack
  push dx
  push si
  push ax  ; save character on stack
;
  mov si,0    ; offset into table
  mov dx,40h  ; segment of table
  mov ds,dx   ; set data segment to this table
  mov dx,[si] ; get table
  add dx,5    ; point to line status register
;
get_stat:
;
  in   al,dx    ; get status byte
  test al,32    ; check for hold register empty
  jz   get_stat ; loop until okay to send
;
  mov dx,[si]   ; point to data register
  pop ax        ; restore character from stack
  out dx,al     ; output character in al
;
  pop si
  pop dx
  pop ds
  ret
;
  ugout   endp
;-----------------------------------------------------------------
;  output to terminal
;
  trmout proc far
;
  push ds  ; save registers on stack
  push dx
  push si
  push ax  ; save character on stack
;
  mov si,2     ; offset into table
  mov dx,40h   ; segment of table
  mov ds,dx    ; set data segment to this table
  mov dx,[si]  ; get table
  add dx,5     ; point to line status register
;
get_term_stat:
;
  in   al,dx         ; get status byte
  test al,32         ; check for hold register empty
  jz   get_term_stat ; loop until okay to send
;
  mov dx,[si]  ; point to data register
  pop ax       ; restore character from stack
  out dx,al    ; output character in al
;
  pop si
  pop dx
  pop ds
;
  ret
;
  trmout  endp
;-----------------------------------------------------------
codes ends
      end
