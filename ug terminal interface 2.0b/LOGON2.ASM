; logon2.asm
;
; routine to display initial logon message
;
  extrn dischr:far
;
  cr equ 13
  lf equ 10
;
  codes segment
;
  public logon
;
  assume cs:codes, ds:codes
;
; --------------------------------------------------------------
logon_data db cr,lf,lf,lf,lf,lf,lf,lf,lf
;
db '                          UG Terminal Interface Program',cr,lf
db '                                  Release 2.0a',cr,lf,lf
;
db '                This program is the property of CADSULTANTS, INC.',cr,lf,lf
;
db '             UNAUTHORIZED USE OR DISTRIBUTION IS EXPRESSLY FORBIDDEN',cr,lf
;
db cr,lf,lf
;
db 'Keyboard programmed for UG II',cr,lf
db 'Hit Carriage Return On Graphics Keyboard to begin...',cr,lf,lf,0
; routine to display logon message
;
logon proc far
;
  push ax
  push si
  push ds
;
  push cs
  pop  ds
  mov  si,0
;
logon_loop:
;
  mov  al,logon_data[si]
  cmp  al,0
  je   logon_end
  call dischr
  inc  si
  jmp  logon_loop
;
logon_end: 
;
  pop  ds
  pop  si  
  pop  ax
  ret
;
logon endp
;
codes ends
      end
