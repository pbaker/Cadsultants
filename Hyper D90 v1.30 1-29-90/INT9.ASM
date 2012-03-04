; int9.asm
;
; 07/22/88     revector kb_int to our routine (which does nothing
;              except slow the interrupt process down - which
;		in-turn fixes the communication errors at 19,200 baud!)    
;
; 5/1/89       update this routine for use with C code (WORKxx V1.24)
;
; --------------------------------------------------------------
 CONST   SEGMENT  WORD PUBLIC 'CONST'
 CONST   ENDS
;---------------------------------------------------------------
 _BSS    SEGMENT  WORD PUBLIC 'BSS'
 _BSS    ENDS
;---------------------------------------------------------------
 _DATA 	 SEGMENT  WORD PUBLIC 'DATA'
 _DATA   ENDS
;---------------------------------------------------------------

DGROUP  GROUP   CONST,  _BSS,   _DATA
        ASSUME  CS:_INTEXT, DS:DGROUP, SS:DGROUP, ES:DGROUP
;---------------------------------------------------------------
;
 _INTEXT   SEGMENT  BYTE PUBLIC 'CODE'

  public _set_int9, _restore_int9


  oldint9      dw    002c4h
               dw    00070h
; ----------------------------------------------

  _restore_int9 proc far

 assume ds:_intext

   push bp
   mov  bp,sp

   push ax
   push dx
   push ds

   push cs
   pop  ds

; restore original int 9 vector

   cli
   mov  dx,word ptr [oldint9]
   mov  ds,word ptr [oldint9+2]
   mov  ax,2509h
   int  21h
   sti
 
   pop  ds
   pop  dx
   pop  ax

  mov sp,bp
  pop bp

  ret
 
 _restore_int9 endp
; -------------------------------------
 _set_int9 proc far
;

   push bp
   mov  bp,sp
  
   push ax
   push bx
   push dx
   push ds
   push es
;
; get kb_int vector (int 09h)
;

   push cs
   pop  ds

   cli
   mov  ax,3509h
   int  21h
   sti

   mov [oldint9],bx
   mov [oldint9+2],es
;
; point int 9 to our routine
;
   cli
   mov  dx,offset _newint9
   mov  ax,2509h
   int  21h
   sti

   pop  es
   pop  ds
   pop  dx
   pop  bx
   pop  ax

   mov  sp,bp
   pop  bp  
 
 ret

 _set_int9 endp

;---------------------------------------------------------------

; routine which intercepts kb_int request
;
 _newint9      proc    far

;   cli
   jmp  dword ptr cs:oldint9

 _newint9  endp
 
;---------------------------------------------------------------
 _INTEXT   ENDS
         end
