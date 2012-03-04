code    segment
	   assume cs:code
	   org     100h

plotter_base     equ  1E8h
baud_factor  equ  12	   ;9600

start:  jmp  init

string	db	27,'.I;5;6:'

mess             db    13,10
       		  db   'PC WorkWindow Systems',13,10
			  db   'Serial Plotter Driver',13,10
                 db   'Version 1.0 ACK/NAK',13,10
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
  push ax			          ; save character
;
  mov  dx,plotter_base + 5	; output port address

get_stat:
  
  in   al,dx
  test al,32
  jz   get_stat
;
  push dx

  mov  dx,plotter_base
;
; send enq
;
  mov  al,5
  out  dx,al
;
; wait for character to be received...
;

 ack_loop:
;
  mov  dx,1EDh
  in   al,dx
  test al,1
  jz   ack_loop
;
; get plotter response
;
  mov  dx,1E8h
  in   al,dx
;
  pop  dx

; check for ok
;
  cmp  al,6
  jne  get_stat
;
; send plotter command
; 
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
; set plotter for ack/nak
;
  mov  dx,plotter_base + 5	; output port address
  mov  si,0
  mov  cx,8

get_stat1:
  
  in   al,dx
  test al,32
  jz   get_stat1

  push dx
  mov  dx,plotter_base
  mov  al,string[si]
  out  dx,al
  inc  si
  pop  dx
  loop get_stat1
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