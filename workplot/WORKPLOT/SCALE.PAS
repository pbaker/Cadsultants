Procedure Output_Scale(var menu_response : integer;
                       var scale         : real;
                       var add_text      : add_text_size);

label
  enter_scale;

Begin

enter_scale:

         ClrScr;
         writeln;
         writeln('     Output Scale = 1');
         GotoXY(21,2);
         numlen := 12;
         GetNum(menu_response,text_string,numlen);
         case menu_response of
            -7, -6, -5, -4, -3:
               goto enter_scale;
            0:
            begin
               add_text [4] := text_string;
               Val(text_string,scale,numlen);
               if scale <= 0 then
                  scale := 3.6
               else
                  scale := scale * 3.6;
            end;
         end; {case}

End; {procedure : SCALE}
