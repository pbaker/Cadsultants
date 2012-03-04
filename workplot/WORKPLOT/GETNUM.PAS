{$C-}
Procedure GetNum (var function_key : integer;
                   var text_string  : full_line;
                   var BufLen       : integer);

{      GetNum - Evaluates Keyboard Input
----------------------------------------------------------------------------
       This procedure returns text_string of ascii numbers
}

label
  done,next_key;

var
funckey,
KeyPressed      : boolean;
ch,sign         : char;
character_value,
xpos,ypos,L,xmin,xmax,decimal_flag,decimal_position,first_flag   : integer;

begin

   text_string := '';
   xmin := WhereX;
   xmax := (xmin + BufLen);
   first_flag := 0;

next_key:

  if KeyPressed then
     begin
       Read(Kbd,ch);
        case ch of
          #47, #63:            { ? / Key }
           begin
            function_key := -6;
            goto done;
           end;
          #8:
           begin              { Backspace Key }
           xpos := WhereX;
           ypos := WhereY;
           if (xpos = xmin) then
            goto next_key;

              xpos := xpos-1;

           if (xpos = xmin) then
            sign := '+';

           if (xpos = decimal_position) then
            decimal_flag := 0;

              GotoXY(xpos,ypos);
               writeln(' ');
              GotoXY(xpos,ypos);
               L := Length(text_string)-1;
             text_string := copy(text_string,1,L);
             goto next_key;
           end;
          #13:               { Carriage Return }
            begin
              function_key := 0;
              goto done;
            end;
          #27:               { Escape }
           begin
            Read(Kbd,ch);
             character_value := Ord(ch);
             case character_value of
              71..83:       { Numeric Keypad }
               begin
                function_key := character_value-51;
               end;
              else
               goto next_key;
             end; {case character_value}

            case function_key of
              20:
               begin
                function_key := -7;
                goto done;
               end;
              21:
               begin
                function_key := -3;       { Alternate Action }
                goto done;
               end;
              22:
               begin
                function_key := -4;       { Page Up Key }
                goto done;
               end;
              24:
               begin
                function_key := -1;       { Reject }
                goto done;
               end;
              26:
               begin
                function_key :=  0;       { Entry Complete }
                goto done;
               end;
              29:
               begin
                function_key := -2;       { Terminate }
                goto done;
               end;
              30:
               begin
                function_key := -5;       { Page Dn Key }
                goto done;
               end;
         end;  {case function_key}
        end;   {case #27}

      #48..#57:
        begin
         if first_flag = 0 then
           begin
             first_flag := 1;
             write('                 ');
             GotoXY(21,2);
           end;
         xpos := WhereX;
         if (xpos = xmax) then
           goto next_key;
         write(ch);
         text_string := Concat(text_string,ch);
         goto next_key;
        end;

      #43, #45:
        begin
         if first_flag = 0 then
           begin
             first_flag := 1;
             write('                 ');
             GotoXY(21,2);
           end;
          if Wherex > xmin then
           goto next_key;

          sign := ch;
          write(ch);
          text_string := Concat(text_string,ch);
          goto next_key;
        end;

      #46:
        begin
          if first_flag = 0 then
           begin
             first_flag := 1;
             write('                 ');
             GotoXY(21,2);
           end;
          if decimal_flag = 1 then
           goto next_key;

          decimal_flag := 1;
          decimal_position := WhereX;
          write(ch);
          text_string := Concat(text_string,ch);
          goto next_key;
        end;
    else
        begin
          function_key := 0;
          goto next_key;
        end;
     end; {case ch}
   end;   {if KeyPressed}

goto next_key;

done:

 if Copy(text_string,1,1) = '+' then
   text_string := Copy(text_string,2,(Length(text_string) - 1));

End; {procedure : GetNum}
