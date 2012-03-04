; kb_stat.asm
;
;
_TEXT   SEGMENT   BYTE PUBLIC  'CODE'
_TEXT   ENDS

_DATA   SEGMENT   WORD PUBLIC  'DATA'
_DATA   ENDS

CONST   SEGMENT   WORD PUBLIC  'CONST'
CONST   ENDS

_BSS    SEGMENT   WORD PUBLIC  'BSS'
_BSS    ENDS

DGROUP  GROUP     CONST,  _BSS,  _DATA
        ASSUME    CS: _TEXT,  DS: DGROUP,  SS: DGROUP,  ES: DGROUP

PUBLIC _keyboard_status


;---------------------------------------------------------------
;
;
  _text segment
;
  assume cs:_text,  ds:dgroup,  es:dgroup
;
;--------------------------------------------------------------
;
; subroutine to tell if a key was pressed
;
_keyboard_status proc far
;
	push bp
	mov bp,sp
	mov ah,1
	int 16h
	jnz read
	mov ax,-1
	jmp stat_end
read:
	mov ax,0
stat_end:
	mov sp,bp
	pop bp
	ret
;
_keyboard_status endp
;
;
;
_text ends
;
end
