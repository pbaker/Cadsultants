code    segment
	   assume cs:code
	   org     100h

plotter_base     equ  1E8h
baud_factor  equ  12	   ;9600

start:  jmp  init

mess             db    13,10
       		  db   'PC WorkWindow Systems',13,10
			  db   'Serial Plotter Driver',13,10
                 db   'Version 1.0 ',13,10
                 db   '9600 baud / IRQ 96 / 1E8h',13,10
			  db   'Copyright 1988 CADSULTANTS, INC.',13,10,10,'$'
;
; interrupt routine to transmit characters to plotter
;
  write_plotter proc far
;
  push ax
  push bx
  push cx
  push dx
  push si
  push di
  push bp
  push sp
  push ds
  push ss
;
  push cs
  pop  ds
;
;  call debug

  push ax			          ; save character
  mov  dx,plotter_base + 5		; output port address

get_stat:

  in   al,dx
  test al,32
  jz   get_stat

  mov  dx,plotter_base
  pop  ax
  out  dx,al
;
  buff_end:
;
  pop  ss
  pop  ds
  pop  sp
  pop  bp
  pop  di
  pop  si
  pop  dx
  pop  cx
  pop  bx
;
  mov  al,20h
  out  20h,al
;
  pop  ax
  iret
;
  write_plotter endp

; -------------------------------------
  debug proc    near
	   
        push ax
        mov  al,7
	   mov  ah,14
        int  10h
        pop  ax
        ret

  debug endp
; -------------------------------------
; PROGRAM INSTALLATION CODE

  assume         ds:code

init:
;
; set serial communication parameters
;
  mov  dx,plotter_base + 3                ; line control register
  mov  al,80h
  out  dx,al
;
; set plotter port baud rate
;
  mov  ax,baud_factor                 ; baud rate divisor
  mov  dx,plotter_base
  out  dx,al

  mov  al,ah
  inc  dx
  out  dx,al

  mov  dx,plotter_base + 3
  mov al,00010011b                 ; no parity 1-stop 8-data
  out  dx,al
;
; set interrupt routine address
;
  mov  dx,offset write_plotter

  mov  al,60h        ; plotter interrupt is 60 hex
  mov  ah,25h        ; DOS function to set interrupt vector
  int  21h
;
; Sign off message
;
  mov  dx,offset   mess
  mov  ah,9h
  int  21h
;
; exit but remain resident
;
  mov  dx,offset   init
  int  27h
 
code ends
     end     start
