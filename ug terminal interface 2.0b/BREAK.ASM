; BREAK sequence routine
;
  break_duration equ 300 ; milliseconds
;
  public send_break
;
  codes segment
;
  assume cs:codes
;-----------------------------------------------------------
  send_break proc far
;
  push ax
  push cx
  push dx
;
  cli               ; disable interupts
  mov  dx,3FBh      ; point to line control register port
  in   al,dx        ; get control byte
  push ax           ; save it
  and  al,10111111b ; clear bit 6 
  or   al,01000000b ; set bit 6
  out  dx,al        ; output new control byte
;
  mov  cx,break_duration ; set counter
;
delay1:
;
  push cx
  mov  cx,260  ; timing constant
;
delay2:    
;
  loop delay2
  pop  cx
  loop delay1
;
  pop  ax      ; restore original byte
  mov  dx,3FBh ; point to line control register
  out  dx,al   ; output original byte
  sti          ; re-enable interupts
;
  pop dx   
  pop cx
  pop ax
;
  ret
;
  send_break   endp
;
  codes ends
        end
