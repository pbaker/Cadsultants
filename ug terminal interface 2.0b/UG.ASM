; ug.asm
; Rev 2.0a
;
; Unigraphics terminal interface
; Copyright 1984 CADSULTANTS, INC.
;
; uses MODGRAPH GX1000 terminal for graphics output
; uses MODGRAPH GX1000 keyboard for pfk and alphanumerics
; uses PC monitor (color card) for message monitor
; uses summamouse for cursor control
;
; 7/24/85 revised ctrl_E code still has collision problems
;         coded new algorithm
;
; 7/19/85 revised ctrl_E (enq) code to eliminate  collision
;         problems with keyboard and graphics data
;
; 7/17/85 moved UG version dependent routines external
;         added ug_buffer "near full" gauge (14K)
;         changed ug buffer size to 16K
;         minor changes to escape filters
;         call to logon message routine
;
; 7/15/85 keyboard programmed internally
;
; 7/8/85  fixed display of attention indicator with selective erase
;         changed location of escape key on keybgoard
;         added BREAK condition key and subroutine
;         changed function key flag to escape
;
; 7/3/85  ICU response set to D90 COLOR.
;         This enables selective erase.
;         Color commands disabled for use with mono GX1000
;
; 6/12/85 single key selection for:
;
;         VT100/4014 text mode toggle
;         enable/disable help screen generation
;         toggle display of help screen
;         return to DOS
;         also -  new mouse keys
;
; 6/11/85 new host character interpreter handles all icu functions
;         message monitor now initial display device
;         help screen toggle bug fix
;
; 4/27/85 changed decode and keyboard definition program
;         new implementation of xon/xoff
;         changed clrnon routine to be compatible with UG-II
;
; 4/23/85 autoexec.bat used to start program
;         
; external port initialization
; 48K data buffering (UNIGRAPHICS)
; features interrupt driven i/o
; dual plane message monitor
;
; equates
;
unlock equ '0'
lock   equ '1'
;
ctrl_A equ 01
ctrl_E equ 05
ctrl_F equ 06
ctrl_G equ 07
ctrl_P equ 16
ctrl_Q equ 17
ctrl_S equ 19
ctrl_T equ 20
ctrl_X equ 24
ctrl_Z equ 26
;
page0  equ 0
page1  equ 1
;
escape equ 27
cr     equ 13
space  equ 32
;
xon  equ 17
xoff equ 19
;
lf  equ 10
bs  equ  8
bel equ  7
;
dim_chr    equ 28
bright_chr equ 29
;
lights_on  equ 25
lights_off equ 24
;
mouse_button1 equ  30
mouse_button2 equ  31
;
function_key_flag  equ  27
exit_to_dos        equ  04
help_screen_toggle equ  03 
help_screen_mode   equ  02
text_mode_toggle   equ  01
break_code         equ  28
buffer_gauge       equ  14000
;-----------------------------------------------------------------       
  stacks  segment stack
;
  dw 128 dup (0)
;
  stacks  ends
;-----------------------------------------------------------------
;
  abs0   segment at 0
;
  org   00Ch*4   ; address of int vector for com1:
;
  async_int1   label   word   
;
  org   00Bh*4   ; address of int vector for com2:
;
  async_int2   label   word
;
  abs0   ends
; ---------------------------------------------------------------
  extrn set_kb:far, trmout:far, ugout:far, lights:far, logon:far
  extrn send_break:far
;
  codes   segment
;
  public read_buffer1, dischr
;
  assume  cs:codes,ss:stacks,ds:codes
;
  gauge_status     db  0
  char_type        db  29
  page_for_write   db  0
  page_for_display db  0
  xon_xoff         db  0
  text_flag        db  0
  help_off_on      db  0
;
  spos1  dw  0
  spos2  dw  0
  epos1  dw  0
  epos2  dw  0
  count1 dw  0
  count2 dw  0
  ret_segment dw  0
  ret_offset  dw  0
;
  buffer2 db  512   dup (?)
  buffer1 db  16000 dup (?)
; ---------------------------------------------------------------
; procedure to lock/unlock MODGRAPH keyboard from further input
;
lock_kb proc far
;
  push ax       ; save lock/unlock key
  mov  al,escape
  call trmout
  mov  al,'^'
  call trmout
  mov  al,'2'
  call trmout
  mov  al,'6'
  call trmout
  mov  al,';'
  call trmout
  pop  ax       ; restore lock/unlock key
  call trmout
  mov  al,'s'
  call trmout
;
  ret
;
  lock_kb endp
;-----------------------------------------------------
; procedure to toggle display between page 0 (ug menu)
; and page 1 (help screen)
;
  help_screen  proc  far
;
  push   ax
  mov    al,page_for_display
  cmp    al,0
  je     display_page_one
;
  mov    ah,5
  mov    al,page0
  int    10h
;
  mov    page_for_display,page0
  pop    ax
  ret
;
display_page_one:   
;
  mov    ah,5
  mov    al,page1
  int    10h
  mov    page_for_display,page1
  pop    ax
  ret
;
  help_screen  endp
; -----------------------------------------------------------------
; routine to position cursor (message monitor)
; input conditions: x (column) is in dl, y (row) is in dh
;
  poscur  proc    far
;
  push ax
  push bx
  push dx
;
  mov  ah,2
  mov  bh,page_for_write
  int  10h
;
  pop     dx
  pop     bx
  pop     ax
;
  ret
;
  poscur  endp
;-----------------------------------------------------------------
; routine to display character on message monitor
;
  dischr proc far
;
  push ax
  push bx
  push cx
  push dx
  push ds
;
  push cs
  pop  ds
;
  cmp  char_type,dim_chr
  je   dim
;
bright:
;
; bug in BIOS routine to emulate TTY.  Routine should write to
; page indicated by bh register.  It does not!  Instead, it
; writes to the currently displayed page.  The following code
; is a fix for same.
;
  cmp  page_for_display,page0  ; check for displayed page
  je   ok_for_tty              ; if zero, we're ok
  push ax                      ; if not, save character
;
  mov ah,5
  mov al,page0
  int 10h
;
  mov page_for_write,page0    ; update flags
  mov page_for_display,page0
;
  pop ax  ; restore character
;
ok_for_tty: 
;
  mov  ah,14
  mov  bl,00001111b
  mov  bh,page_for_write       ; this setting doesn't work !!!
  int  10h
  jmp  dis_end
;
dim:
;
  cmp al,space
  je  bright
  cmp al,cr
  je  bright
  cmp al,lf
  je  bright
  cmp al,bs
  je  bright
  cmp al,bel
  je  bright
;
; change the following attribute value for color display
;
  mov bl,00000111b      ; light grey attribute
;
  mov ah,9  ; write character and attribute
  mov bh,page_for_write ; page for write
  mov cx,1  ; number of chars. to write
  int 10h   ; BIOS call
; 
  mov ah,3  ; read cursor position
  mov bh,page_for_write
  int 10h   ; BIOS entry
;
  cmp  dl,79
  je   wrap
  inc  dl
  call poscur
  jmp  dis_end
;
wrap: 
;
  inc  dh
  mov  dl,0
  call poscur
;
dis_end:
;
        pop  ds
        pop  dx
        pop  cx
        pop  bx
        pop  ax
;
        ret
;
  dischr  endp
; --------------------------------------------------------------
; initialize buffer 1
;
  initbuff1 proc far
;
  cli
  mov   si,0
  mov   di,0
  mov   spos1,0
  mov   epos1,0
  mov   count1,0
  sti
  ret
;
  initbuff1  endp
; --------------------------------------------------------------
; initialize buffer 2
;
  initbuff2 proc far
;
  cli
  mov   si,0
  mov   di,0
  mov   spos2,0
  mov   epos2,0
  mov   count2,0
  sti
  ret
;
  initbuff2  endp
; ------------------------------------------------------------  
; procedure to de-program MODGRAPH keyboard
;
clr_kb proc far
;
  push ax
  mov  al,escape
  call trmout
  mov  al,'^'
  call trmout
  mov  al,'2'
  call trmout
  mov  al,'5'
  call trmout
  mov  al,'6'
  call trmout
  mov  al,'K'
  call trmout
  pop  ax
  ret
;
clr_kb endp
;------------------------------------------------------------------------------
; procedure to disable serial port interrupts
;
clr_int proc far
;
  push ax
  push dx
;
  sub ax,ax    ; zero in ax
  mov dx,03F9h
  out dx,al    ; disable com1
  mov dx,02F9h
  out dx,al    ; disable com2
;
  in al,21h       ; get 8259 control byte
  or al,00011000b ; disable com1 and com2
  out 21h,al      ; output new control byte
;
  pop dx
  pop ax
  ret
;
clr_int endp
; ---------------------------------------------------------------
; selective erase routine.  ax = 1 begin selective erase
;                           ax = 2 end selective erase
  vector_mode proc far 
;
  push ax
  mov  al,escape
  call trmout
  mov  al,'/'
  call trmout
  pop  ax
  call trmout
  mov  al,'d'
  call trmout
  ret
;
  vector_mode endp
; ----------------------------------------------------------------    
; read character from buffer 2  (terminal)
;
  read_buffer2 proc far
;
  cmp count2,0
  jne buff2a
  ret
;
buff2a:
;
  mov si,spos2
  mov al,buffer2[si]   ; get character
  inc spos2            ; increment pointer
  dec count2           ; decrement counter
  cli                  ; disable interrupts
  cmp count2,0         ; check for buffer empty
  jne  exit2
;
  mov spos2,0  ; reset pointers if buffer empty
  mov epos2,0
;
exit2:
;
  sti          ; re-enable interrupts
;
  cmp al,ctrl_S        ; check for ctrl_S (XOFF)
  jne exit2a
  mov xon_xoff,1       ; if found, set flag for read_buffer1 (UG)
;
exit2a:
;
  mov bl,1     ; dummy value in bl...
  cmp bl,0     ; for comparison. (force zflag)
  ret
;
  read_buffer2 endp
; ---------------------------------------------------------------
; read character from buffer 1 (UG)
;
  read_buffer1 proc far
;
  push bx
  push si
  push ds
;
  mov  ax,codes
  mov  ds,ax
;
  cmp  count1,0
  je   exit1
;
  cmp  xon_xoff,1     ; check for X-OFF graphics terminal
  jne  xoff_ok
;
not_ok:
;
  call read_buffer2  
  jz   not_ok         ; read next character
  cmp  al,xon         ; wait for X-ON
  jne  not_ok
  mov  xon_xoff,0     ; when X-ON found, reset flag
;
  cmp  gauge_status,1  ; check for buffer near full condition
  je   xoff_ok         ; if so, don't resume host transmission
  call ugout           ; if not, send X-ON (already in al)
;
 xoff_ok:          
;
  mov si,spos1
  mov al,buffer1[si] ; get character
  inc spos1          ; increment pointer
  dec count1         ; decrement counter
  cli                ; disable interrupts
  cmp count1,0       ; check for buffer empty
  je  init1          ; reset pointers if empty
;
exit1:
;
  sti                ; re-enable interrupts
  pop  ds
  pop  si
  pop bx
  ret
;
init1:
;
  mov spos1,0    ; reset pointers
  mov epos1,0
  sti            ; re-enable interrupts
;
  cmp  gauge_status,1  ; check for buffer near full
  jne  init1a
  mov  gauge_status,0  ; set gauge to empty
  push ax              ; save character
  mov  al,ctrl_Q       ; tell UG to send some more
  call ugout
  pop  ax              ; restore character
;
init1a:
;
  mov  bl,1   ; dummy value in bl...
  cmp  bl,0   ; for comparison. (force zflag)
  pop  ds
  pop  si
  pop bx
  ret
;
read_buffer1 endp
; --------------------------------------------------------
;  routine to blank active message monitor display
;
  clrall proc far
;
  push ax
  push bx
  push cx
  push dx
;
  mov  dx,0
  call poscur
;
  mov al,0     ; blank entire screen
  mov bh,15    ; white characters
  mov ch,0     ; upper left row
  mov cl,0     ; upper left column
  mov dh,24    ; lower right row
  mov dl,79    ; lower right column
  mov ah,6     ; scroll up function
  int 10h      ; BIOS call
;
  pop dx
  pop cx
  pop bx
  pop ax
;
  ret
;
  clrall  endp
;--------------------------------------------------------------
; routine to blank "non-protected" area of monitor screen
;
  clrnon proc far
;
  push ax
  push bx
  push cx
  push dx
;
  mov  dx,0
  call poscur
;
  mov al,0    ; number of lines to scroll up
              ; 0 = blank entire window
  mov bh,15   ; white characters
  mov ch,0    ; upper left row
  mov cl,0    ; upper left column
  mov dh,16   ; lower right row
  mov dl,59   ; lower right column
  mov ah,6    ; scroll up function
  int 10h     ; BIOS call
;
  pop dx
  pop cx
  pop bx
  pop ax
;
  ret
;
  clrnon  endp
;------------------------------------------------------
; routine to initialize message monitor
;
  mminit proc far
;
  push ax
;
  mov  al,3  ; 80 x 25 color
  mov  ah,0  ; set display mode function
  int  10h
;
  mov  al,page_for_write
  mov  ah,5
  int 10h
;
  call clrall
;
  pop     ax
  ret
;
  mminit  endp
;------------------------------------------------------------
; call routine to enable interrupts
;
  set_int proc far
;
  push ax
  push dx
  push ds
;
  assume ds:abs0
;
  sub ax,ax
  mov ds,ax
;
; compute interrupt handler addresses
;
  mov async_int1, offset write_buffer1
  mov async_int1+2,cs
;
  mov dx,03F9h ; interrupt enable register
  mov al,01h   ; enable received data bit
  out dx,al    ; enable interrupt
;
  in  al,21h   ; 8259 mask register
  and al,00EFh ; 0 at bit 4
  out 21h,al   ; interrupt unmasked
;
  mov dx,3FCh
  mov al,08h
  out dx,al
; -------------------------------------------------------------
  mov async_int2, offset write_buffer2
  mov async_int2+2,cs
;
  mov dx,02F9h      ; interrupt enable register
  mov al,01h        ; enable received data bit
  out dx,al         ; enable interrupt
;
  in  al,21h        ; 8259 mask register
  and al,00F7h      ; 0 at bit 3
  out 21h,al        ; interrupt unmasked
;
  mov dx,2FCh
  mov al,08h
  out dx,al
;
  pop ds
  pop dx
  pop ax
;
   ret
;
  set_int   endp
; --------------------------------------------------------------
;  routine to interpret keyboard entry
;  outputs appropriate codes for function keys
; 
  decode proc far
;
  cmp  al,ctrl_Q
  jne  dc0
  mov  xon_xoff,0
  call ugout
;
dc0:
;
  cmp  al,mouse_button1    ; mouse button #1
  jne  dc1
  mov  al,space            ; screen pick/location
  call ugout
  jmp  keyend
;
dc1:
;
  cmp  al,mouse_button2    ; mouse button #2
  jne  dc2
  mov  al,60               ; entry complete
  jmp  fktx
;
dc2:
;   
  cmp  al,function_key_flag
  jne  not_a_function
  jmp  get_function_id
;
not_a_function:
;
  call ugout
  jmp  keyend
;
get_function_id:
;
  call read_buffer2      ; get next character
  jz   get_function_id   ; keep trying.
;
dc3:
;
  cmp  al,help_screen_toggle
  jne  dc4
  cmp  help_off_on,1     ; check to see if help screen is disabled
  je   dc3a              ; if it is, don't toggle...
                         ; ... or you're asking for trouble!
  call help_screen
;
dc3a:  
;
  jmp  keyend
;
dc4:
;
  cmp  al,exit_to_dos
  jne  dc5
  call clr_int
  call clr_kb
  mov  ax,ret_segment
  push ax
  mov  ax,ret_offset
  push ax
  ret
;
dc5:
;
  cmp  al,help_screen_mode
  je   screen_mode
  jmp  dc6
;
screen_mode:
;
  cmp help_off_on,0    ; see if help screen is enabled
  je  mode1            ; if enabled, toggle it OFF
;
mode0:
;
  mov  help_off_on,0   ; if not enabled...
  jmp  keyend          ; turn it ON
;
mode1:
;
  mov  help_off_on,1   ; set status flag to off condition
  mov  ah,5            ; momentarily display help screen
  mov  al,page1
  int  10h
;
  call clrall          ; clear it, else old screen will appear
                       ; when it is re-enabled!
;
  mov  ah,5            ; display primary screen
  mov  al,page0
  int  10h
;
  mov  page_for_display,page0  ; set display flag to primary screen
  jmp  keyend
;
dc6:
;
  cmp  al,text_mode_toggle
  je   check_text_mode
  jmp  dc7
;
check_text_mode:
;
  cmp text_flag,0
  je  vt100_text
  jmp graphics_text
;
vt100_text:
;        
  mov  al,7
  call trmout
  mov  text_flag,1
  mov  al,escape
  call trmout
  mov  al,'^'
  call trmout
  mov  al,'1'
  call trmout
  mov  al,'8'
  call trmout
  mov  al,';'
  call trmout
  mov  al,'0'
  call trmout
  mov  al,'s'
  call trmout
  jmp  keyend
;
graphics_text:
;
  mov  text_flag,0
  mov  al,escape
  call trmout
  mov  al,'^'
  call trmout
  mov  al,'1'
  call trmout
  mov  al,'8'
  call trmout
  mov  al,';'
  call trmout
  mov  al,'1'
  call trmout
  mov  al,'s'
  call trmout
  jmp  keyend
;
dc7:
;
  cmp  al,break_code
  jne  dc7a
  call send_break
  jmp  keyend
;
dc7a:
;
  cmp  al,32
  jb   invalid_range
  cmp  al,63
  ja   invalid_range
  jmp  fktx
;
invalid_range:
;
  push ax                   ; save character
  mov  al,function_key_flag ; load 1st character
  call ugout                ; send to host
  pop  ax                   ; restore 2nd character
  call ugout                ; send to host
  jmp  keyend               ; done
;-------------------------------------------------------------
; function key transmitter.  part of decode procedure
;
fktx:
;
  push ax    ; save character on stack
  mov  al,31 ; header required by UG
  call ugout ; SEND IT
;
  mov  al,97 ; second byte required by UG
  call ugout ; SEND IT
;
  pop  ax    ; restore character from stack
  call ugout ; send it
;
keyend:
;
  ret
;
  decode  endp
; -------------------------------------------------
; routine for keyboard input
;
  keybrd proc far
;
  call read_buffer2
  jz   kbend
  call decode
;
kbend:
;
  ret
;
  keybrd  endp
; -------------------------------------------------
; main routine
;
  main proc far
;
  mov  ret_segment,ds
  sub  ax,ax
  mov  ret_offset,ax
;
  push cs
  pop  ds
;
reset:
;
  call initbuff1
  call initbuff2
;
  mov  char_type,bright_chr   ; bright character
  mov  page_for_write,page0   ; primary display page
;
  call mminit
;
  mov  char_type,dim_chr      ; dim character
  mov  page_for_write,page1   ; secondary display page
;
  call clrall
;
  mov  char_type,bright_chr   ; back to bright
  mov  page_for_write,page0   ; primary screen
  mov  page_for_display,page0 ; primary screen
;
  call set_int
;
  call set_kb
;
  call logon
;
  jmp  mm1
;-----------------------------------------------------------------
tdata:
;
  call keybrd
  call read_buffer1
  jz   tdata
;
  cmp  al,escape ; check for escape or ctrl-p
  je   es1
  cmp  al,ctrl_P
  jne  td1
  jmp  cp1
;
td1:
;
  call trmout         ; if not escape or ctrl-p, output
                      ; actual character to terminal
  jmp  tdata          ; get next character
;-----------------------------------------------------------------
; escape handler.   possible request for terminal information
;
es1:
;
  call read_buffer1
  jz   es1
;
es2:
;
  cmp  al,5 ; check for ctrl-E
  jne  es3
  jmp  enq  ; (Request for Status and address)
;
es3:
;
  cmp  al,ctrl_Z  ; check for crosshair enable
  je   es3a
  jmp  es4
;
es3a:
;
  mov  al,escape
  call trmout    ; send escape (already in al)
  mov  al,'^'    ; follow escape with alphagraphics mode sequence
  call trmout
  mov  al,'1'
  call trmout
  mov  al,'8'
  call trmout
  mov  al,';'
  call trmout
  mov  al,'1'
  call trmout
  mov  al,'s'
  call trmout
;
  mov  al,escape
  call trmout
  mov  al,ctrl_Z
  call trmout
  jmp  tdata
;
es4:
;
  cmp  al,'x'    ; start selective erase
  jne  es4a
  mov  al,'1'
  call vector_mode
  jmp  tdata
;
es4a:
;
  cmp  al,'y'    ; end selective erase
  jne  es4b
  mov  al,'0'
  call vector_mode
  jmp  tdata
;
es4b:
;
  cmp  al,'G'    ; genisco 2200 enhanced command flag
  je   es4c
  jmp  es4d
;
es4c:
;
  call read_buffer1
  jz   es4c
  cmp  al,'O'    ; genisco command to draw circle
                 ; used for attention indicator
  je   es4c1
  jmp  es4c2
;
es4c1:
;
  call read_buffer1 ; read from ug until cr
  jz   es4c1
  cmp  al,cr
  jne  es4c1
  mov  al,31        ; exit to alphagraphics mode
  call trmout
  mov  al,'O'       ; attention indicator
  call trmout
  mov  al,29        ; re-enter graphics vector mode
  call trmout
  jmp  tdata
;
es4c2:
;
  call read_buffer1 ; bit bucket for unimplemented
  jz   es4c2        ; genisco codes
  cmp  al,cr
  jne  es4c2
  jmp  tdata
;
es4d:
;
  push ax           ; save current character
  mov  al,escape
  call trmout       ; send escape
  pop  ax           ; restore character 
  call trmout       ; send to graphics terminal
  jmp  tdata
;----------------------------------------------------------------
;  ctrl-p handler.  possible entry into message monitor
;
cp1:
;
  call read_buffer1
  jz   cp1
;
  cmp  al,cr
  jne  cp2
  jmp  mm1
;
cp2:
;
  call trmout
  jmp  tdata
;------------------------------------------------------------
; enq handler
;
enq:
;
;  mov  al,lock  ; lock MODGRAPH keyboard
;  call lock_kb
;
reset_counter:        ; set or reset timeout counter
;
  mov  cx,1000
;
eloop1:               ; loop until terminal input buffer empty
;
  call keybrd         ; check for character
  jnz  reset_counter  ; reset if character found,
                      ; else loop until timeout
  loop eloop1         ; decrements cx `til zero
; 
  mov  al,29           ; header expected by UG
  call ugout           ; send it
  mov  al,escape       ; arm terminal for escape sequence
  call trmout          
  mov  al,05           ; load ctrl-E (enquire status byte)
  call trmout          ; send ctrl-E
;
eloop2:
;
  call read_buffer2    ; get terminal response
  jz   eloop2          
  call ugout           ; send to UG
  cmp  al,cr           ; check for terminator
  jne  eloop2          ; if not cr, read next character
;
;  mov  al,unlock       ; unlock MODGRAPH keyboard
;  call lock_kb
  jmp  tdata           ; done
; -------------------------------------------------------------
; interpret characters received from host.  perform emulation
;
rd0:
;
  cmp  al,ctrl_P  ; position cursor
  jne  rd1
;
rd0b:  
;
  call read_buffer1  ; get column
  jz   rd0b          ; loop until it arrives
  mov  dl,al         ; store in dl
;
rd0e:   
;
  call read_buffer1  ; get row
  jz   rd0e          ; loop unitl it arrives
  mov  dh,al         ; store in dh
  call poscur        ; cursor position routine
  jmp  rdugend
;
rd1:
;
  cmp  al,dim_chr    ; check for protected character
  je   setchr
  cmp  al,bright_chr ; check for non-protected character
  jne  rd3
;
setchr:
;
  mov  char_type,al  ; set character type flag
  jmp  rdugend
;
rd3:
;
  cmp  al,6          ; clear non-protected screen area
  jne  rd4
  call clrnon
  jmp  rdugend
;
rd4:
;
  cmp  al,2          ; home cursor
  jne  rd5
  mov  dx,0          ; row and column set to 0,0
  call poscur        ; position cursor
  jmp  rdugend
;
rd5:
;
  cmp  al,127        ; rubout (ignored)
  jne  rd6
  jmp  rdugend
;
rd6:
;
  cmp  al,12         ; clear entire screen
  jne  rd6a
  call clrall
  jmp  rdugend
;
rd6a:
;
  cmp  al,10         ; newline
  jne  rd7
  call dischr
  mov  al,13
  call dischr
  jmp  rdugend
;
rd7:
;
  cmp  al,11          ; ug code to move cursor up
  jne  rd8
;
  mov  ah,3              ; read cursor position
  mov  bh,page_for_write ; on current page
  int  10h               ; BIOS call
;
  cmp  dh,0              ; check for row = 0
  jne  rd7a              ; if zero, can`t move up. 
  jmp  rdugend
;
rd7a:
;
  sub  dh,1              ; if not 0, subtract 1
  call poscur            ; move cursor to new location
  jmp  rdugend
;
rd8:
;

  cmp  al,escape ; possible escape sequence
  jne  rd9
  jmp  escape_sequence
;
rd9:
;
  cmp  al,0     ; ignored
  jne  rd9a
  jmp  rdugend
;
rd9a:
;
  cmp  al,1     ; ignored
  jne  rd9b
  jmp  rdugend
;
rd9b:
;
  cmp  al,3     ; enable blink
  jne  rd9c
  jmp  rdugend  ; not implemented
;
rd9c:
;
  cmp  al,4     ; disable blink 
  jne  rd9d
  jmp  rdugend  ; not implemented
;
rd9d:
;
  cmp  al,5     ; read cursor
  jne  rd9e
  jmp  rdugend  ; not implemented
;
rd9e:
;
  cmp  al,9     ; ignored
  jne  rd9f
  jmp  rdugend 
;
rd9f:
;
  cmp  al,14    ; start blink
  jne  rd9g
  jmp  rdugend  ; not implemented
; 
rd9g:
;
  cmp  al,15    ; end blink
  jne  rd9h
  jmp  rdugend  ; not implemented
; 
rd9h:
;
  cmp  al,17    ; ignored
  jne  rd9i
  jmp  rdugend 
; 
rd9i:
;
  cmp  al,18    ; scroll enable
  jne  rd9j
  jmp  rdugend  ; not implemented
; 
rd9j:
;
  cmp  al,19    ; scroll disable
  jne  rd9k
  jmp  rdugend  ; not implemented
; 
rd9k:
;
  cmp  al,20    ; start underline
  jne  rd9l
  jmp  rdugend  ; not implemented
; 
rd9l:
;
  cmp  al,21    ; end underline
  jne  rd9m
  jmp  rdugend  ; not implemented
; 
rd9m:
;
  cmp  al,22    ; ignored
  jne  rd9n
  jmp  rdugend 
; 
rd9n:
;
  cmp  al,23    ; cursor up
  jne  rd9o
  jmp  rdugend  ; not implemented
; 
rd9o:
;
  cmp  al,24    ; cursor right
  jne  rd9p
  jmp  rdugend  ; not implemented
; 
rd9p:
;
  cmp  al,25    ; cursor left
  jne  rd9q
  mov  al,8
  call dischr
  jmp  rdugend
; 
rd9q:
;
  cmp  al,26    ; cursor down
  jne  rd9r
  mov  al,10
  call dischr
  jmp  rdugend
; 
rd9r:
;
  cmp  al,30    ; ignored
  jne  rd9s
  jmp  rdugend  ; not implemented
; 
rd9s:
;
  cmp  al,31    ; ignored
  jne  rd9t
  jmp  rdugend  ; not implemented
; 
rd9t:
;
  call dischr   ; displayable character
  jmp  rdugend
;
escape_sequence:
;
  call read_buffer1    ; get next character
  jz   escape_sequence ; loop until it arrives
;
escape_zero:
;
  cmp  al,'0'      ; system reset
  jne  escape_one
  jmp  reset
;
escape_one:
;
  cmp  al,'1'      ; transparent data
  jne  escape_two
  jmp  tdata
;
escape_two:
;  
  cmp  al,'2'      ; light control
  jne  escape_three
  cmp  help_off_on,1
  je   bit_bucket
  call lights
  jmp  mm1
;
bit_bucket:  
;
  call read_buffer1 ; get character
  jz   bit_bucket
;
  cmp  al,cr        ; check for end of light control
  jne  bit_bucket   ; not end, ignore and get next
  jmp  rdugend      ; end of light data
;
escape_three:
;
  cmp  al,'3'       ; screen edit mode
  jne  escape_four
  jmp  rdugend      ; not implemented
;    
escape_four:
;
  cmp  al,'4'       ; DG function mode
  jne  escape_five
  jmp  rdugend      ; not implemented
;
escape_five:
;
  cmp  al,'5'    ; read configuration
  jne  ignored
;
  mov  al,1Eh    ; header
  call ugout
  mov  al,'o'    ; signifies that data follows
  call ugout
  mov  al,'!'    ; icu model #1
  call ugout
  mov  al,'&'    ; D90 COLOR !!!!!
  call ugout
  mov  al,'%'    ; firmware revision 2
  call ugout
  jmp  mm1
;
ignored:
;
  mov  dh,24     ; unexpected character after escape
  mov  dl,2
  call poscur
  call dischr
;
rdugend:
;
  jmp mm1     ; end of interpreter
;--------------------------------------------------------------
mm1:
; 
  call keybrd
  call read_buffer1
  jz   mm1
  jmp  rd0    ; process character 
;
  main endp
;--------------------------------------------------------------
; write characters into buffer 1  (ug input)
;
  write_buffer1 proc far
;
wb1:
;
  push  ax
  push  dx
  push  di
;
  mov   dx,3fdh  ; line status register
  in    al,dx    ; get status byte
  test  al,01h   ; check for character received
;
  jz    buff1_end   ; if no character, return

  mov   dx,3f8h  ; received data register
  in    al,dx    ; get character
;
  mov  di,epos1   
  mov  buffer1[di],al  ; write character to buffer
  inc  epos1           ; increment pointer
  inc  count1          ; increment count
;
  mov  ax,epos1        ; check for buffer near full
  cmp  ax,buffer_gauge
  jne  buff1_end
  mov  al,ctrl_S
  call ugout
  mov  gauge_status,1  ; status is near full
;
buff1_end:
;
  pop  di
  pop  dx
;
  mov  al,20h
  out  20h,al
;
  pop  ax
  iret
;  
  write_buffer1 endp
;--------------------------------------------------------------
; write characters into buffer 2 (terminal input)
;
  write_buffer2 proc far
;
wb2:
;
  push ax
  push dx
  push di
;
  mov  dx,2FDh  ; line status register
  in   al,dx    ; get status byte
  test al,01h   ; check for character received
;
  jz   buff2_end   ; if no character, return
;
  mov  dx,2F8h  ; received data register
  in   al,dx    ; get character
;
  mov  di,epos2   
  mov  buffer2[di],al  ; write character to buffer
  inc  epos2           ; increment pointer
  inc  count2          ; increment count
;
buff2_end:
;
  pop  di
  pop  dx
;
  mov  al,20h
  out  20h,al
;
  pop  ax
  iret
;
  write_buffer2 endp
;----------------------------------------------------------------
  codes ends
;
  end   main
