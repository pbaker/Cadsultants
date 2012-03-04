{$C-}
Procedure GetText (var function_key : integer;
                   var text_string  : file_length;
                   var BufLen       : integer);

{       GetText -  Evaluates Keyboard Input
----------------------------------------------------------------------------
     This procedure returns text_string of characters
}

label
  done,next_key;

var
old_string      : file_length;
funckey,
KeyPressed      : boolean;
ch              : char;
character_value,american_flag,
xpos,ypos,L,xmin,xmax       : integer;

begin
{
   KeyPressed := True;
}
   american_flag := 0;
   xmin := WhereX;
   xmax := (xmin + BufLen);
   ypos := WhereY;
   write(text_string);
   GotoXY(xmin,ypos);
   old_string  := text_string;
   text_string := '';

next_key:

{  if KeyPressed then
     begin
}

       Read(Kbd,ch);
       american_flag := american_flag + 1;

        case ch of
          #47, #63:            { ? / Key }
           begin
            function_key := -6;
            text_string := old_string;
            goto done;
           end;
          #8:
           begin              { Backspace Key }

           if american_flag = 1 then
           begin
             write('                              ');
             GotoXY(xmin,ypos);
           end;

           xpos := WhereX;

           if (xpos = xmin) then
            goto next_key;

              xpos := xpos-1;
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
               if american_flag = 1 then
                  text_string := old_string;
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
               if american_flag = 1 then
                  american_flag := 0;
               goto next_key;
             end; {case character_value}

            case function_key of
              20:
               begin
                function_key := -7;       { Home Key }
                if american_flag = 1 then
                   text_string := old_string;
                goto done;
               end;
              21:
               begin
                function_key := -3;       { Alternate Action }
                if american_flag = 1 then
                   text_string := old_string;
                goto done;
               end;
              22:
               begin
                function_key := -4;       { Page Up Key }
                if american_flag = 1 then
                   text_string := old_string;
                goto done;
               end;
              24:
               begin
                function_key := -1;       { Reject }
                text_string := old_string;
                goto done;
               end;
              26:
               begin
                function_key :=  0;       { Entry Complete }
                if american_flag = 1 then
                   text_string := old_string;
                goto done;
               end;
              29:
               begin
                function_key := -2;       { Terminate }
                if american_flag = 1 then
                   text_string := old_string;
                goto done;
               end;
              30:
               begin
                function_key := -5;       { Page Dn Key }
                if american_flag = 1 then
                   text_string := old_string;
                goto done;
               end;
         end;  {case function_key}
        end;   {case #27}

      #32..#125:
        begin

        if american_flag = 1 then
        begin
             write('                              ');
             GotoXY(xmin,ypos);
        end;

         xpos := WhereX;
         if (xpos = xmax) then
           goto next_key;
         write(ch);
         text_string := Concat(text_string,ch);
         goto next_key;
        end;
    else
        begin
          function_key := 0;
          goto next_key;
        end;
     end; {case ch
   end;    if KeyPressed}

goto next_key;

done:

End; {procedure : GETTEXT}
