; lights2.asm
;
; routine which controls the display of "help" data for UG II
;
  extrn read_buffer1:far
;
  codes  segment
;
  public lights
;
  page_0 equ 0
  page_1 equ 1
  cr     equ 13
;
  assume cs:codes, ds:codes
;
; ------------------------------------------------------------------
  light_data dw  light0
             dw  light1
             dw  light2
             dw  light3
             dw  light4
             dw  light5
             dw  light6
             dw  light7
             dw  light8
             dw  light9
             dw  light10
             dw  light11
             dw  light12
             dw  light13
             dw  light14
             dw  light15
             dw  light16
             dw  light17
             dw  light18
             dw  light19
             dw  light20
             dw  light21
             dw  light22
             dw  light23
             dw  light24
             dw  light25
             dw  light26
             dw  light27
             dw  light28
             dw  light29
             dw  light30
             dw  light31
;
light0       db        9,11,'F8  - Macros',0
light1       db         9,6,'F10 - Change Mod',0
light2       db         9,9,'F7  - File/Terminate',0
light3       db        9,14,'F20 - Redisplay',0
;
light4       db   49,3,'np 1   Option 1',0
light5       db   49,4,'np 2   Option 2',0
light6       db   49,5,'np 3   Option 3',0
light7       db   49,6,'np 4   Option 4',0
light8       db   49,7,'np 5   Option 5',0
light9       db   49,8,'np 6   Option 6',0
light10      db   49,9,'np 7   Option 7',0
light11      db  49,10,'np 8   Option 8',0
light12      db  49,11,'np 9   Option 9',0
light13      db  49,12,'np 0   Option 10',0
;
light14      db  49,14,' PF1   Option 11',0
light15      db  49,15,' PF2   Option 12',0
light16      db  49,16,' PF3   Option 13',0
light17      db  49,17,' PF4   Option 14',0
;
light18      db         9,4,'F11 - Blank/Unblank',0
light19      db         9,5,'F12 - Calculator',0
light20      db        9,13,'F15 - Module Parameters',0
light21      db        9,15,'F16 - System Parameters',0
light22      db        9,16,'F18 - WCS',0
light23      db        9,12,'F9  - Module Function',0
light24      db        9,17,'F19 - Zoom/Pan/Regen',0
light25      db        9,10,'F17 - Layer Control',0
light26      db         9,6,'F13 - Display Control',0
light27      db         9,8,'F14 - Edit Work View',0
;
light28      db 9,19,' np <Enter> - Entry Complete',0
light29      db 9,20,' np <  .  > - Reject',0
light30      db 9,21,' np <  -  > - Terminate',0
light31      db 9,22,' np <  ,  > - Alternate Action',0
; -----------------------------------------------------------------
; routine to position cursor  (row & column in dx)
;
pos_help_cursor proc far
;
  push ax
  push bx
  push dx
;
  mov  ah,2
  mov  bh,page_1
  int  10h
;
  pop  dx
  pop  bx
  pop  ax
;
  ret
;
pos_help_cursor endp
;-----------------------------------------------------------
; routine to clear help screen
;
clr_hlp_scrn proc far
;
  push ax
  push bx
  push cx
  push dx
;
  mov  dx,0
  call pos_help_cursor
;
  mov  al,0
  mov  bh,15
  mov  ch,0
  mov  cl,0
  mov  dh,24
  mov  dl,79
  mov  ah,6
  int  10h
;
  pop  dx
  pop  cx
  pop  bx
  pop  ax
;
  ret
;
clr_hlp_scrn endp
;-------------------------------------------------------------------
; routine to display "light" control text
;
  display_lights proc far
;
        push    ax
        push    bx
        push    cx               
        push    dx
;
; bl loaded with character attribute by caller
;
   mov ah,9              ; write character and attribute
   mov bh,page_1
   mov cx,1              ; number of chars. to write
   int 10h               ; BIOS call
;
   mov ah,3              ; read cursor position
   mov bh,page_1
   int 10h               ; BIOS entry
;
   inc  dl
   call pos_help_cursor
;
   pop dx
   pop cx
   pop bx
   pop ax
;
   ret
;
  display_lights  endp
; --------------------------------------------------------------
; turns all lights ON or OFF
; bl contains character attribute set by caller
;
  all_lights proc far
;
  push ax
  push bx
  push cx
  push dx
;
  mov ax,0
  mov cx,0
;
begin_lights:
;
  add ax,ax              ; double to index table
  mov si,ax              ; point into table
  mov si,light_data[si]  ; address of text
;
  mov  dl,[si]
  inc  si
  mov  dh,[si]
  call pos_help_cursor
  inc  si
;
read_mess:
;
  mov al,[si]
  cmp al,0
  je  end_read
  call display_lights
  inc  si
  jmp  read_mess
;
end_read:
;
  cmp cx,31        ; 0-31 (32 buttons)
  je  lights_done  ; check for last button
  inc cx           ; increment counter
  mov ax,cx
  jmp begin_lights ; display next message
;
lights_done:
;
  pop dx
  pop cx
  pop bx
  pop ax
;
  ret
;
  all_lights   endp
; --------------------------------------------------------------------
; light routine
;
lights proc far
;
  push ax
  push bx
  push dx
  push si
  push ds
;
  mov  ax,codes
  mov  ds,ax
;
light_ctrl:
;
  call read_buffer1    ; get character
  jz   light_ctrl
;
  cmp  al,cr           ; check for end of light control
  jne  lcd             ; not end, it is control code
  jmp  light_ctrl_end  ; else end
;
; light control data
;
lcd:
;
  cmp  al,24    ; else, check for all lights off
  jne  lc2
;
  mov  ah,5
  mov  al,page_1
  int  10h
  call clr_hlp_scrn
  jmp  light_ctrl  ; get next control code
;
lc2:
;
  cmp  al,25        ; check for all lights on
  jne  lc3
  mov  bl,00001111b ; bright white
  call all_lights
  jmp  light_ctrl   ; get next control code
;
lc3:
;
; determine if data is within valid range
; for individual light control
;
  cmp  al,32      ; 32 is lowest valid character
  jb   light_ctrl
  cmp  al,95      ; 95 is greatest valid character
  ja   light_ctrl
;
; data within valid range.  determine action
;
  cmp  al,64      ; 0-63 are ON, 64-95 are OFF
  jb   one_light_on
;
; turn one light off
;
  mov  bl,00000000b ; black
  mov  ah,0         ; clear high byte
  sub  al,64        ; points to actual f.k. light
;
; the following 5 lines will disable the display
; of pfk options 1 thru 14
;
;  cmp  al,4
;  jb   ok_off
;  cmp  al,17
;  ja   ok_off
;  jmp  light_ctrl
;
ok_off:
;
  add  ax,ax             ; double to index table
  mov  si,ax             ; point into table
  mov  si,light_data[si] ; address of text
  mov  dl,[si]
  inc  si
  mov  dh,[si]
  call pos_help_cursor
  jmp  light_text
;
one_light_on:
;
  mov  bl,00001111b    ; bright white
  mov  ah,0
  sub  al,32
;
; the following 5 lines will disable the display
; of pfk options 1 thru 14
;
;  cmp  al,4
;  jb   ok_on
;  cmp  al,17
;  ja   ok_on
;  jmp  light_ctrl
;
ok_on:
;
  add  ax,ax
  mov  si,ax
  mov  si,light_data[si]
  mov  dl,[si]
  inc  si
  mov  dh,[si]
  call pos_help_cursor
;
light_text:
;
  inc  si
  mov  al,[si]
  cmp  al,0
  je   light_text_end
;
  call display_lights
  jmp  light_text
;
light_text_end:
;
  jmp  light_ctrl
;
light_ctrl_end:
;
  mov ah,5
  mov al,page_0
  int 10h    ; reset active page
;
  pop  ds
  pop  si
  pop  dx
  pop  bx
  pop  ax
;
 ret
;
lights endp
;------------------------------------------------
codes ends
      end
