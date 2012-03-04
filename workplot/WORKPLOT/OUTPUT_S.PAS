Procedure Output_Scale(var menu_response  : integer;
                       var scale          : real);
{
         OUTPUT_SCALE - Change Output Scale
--------------------------------------------------------------------------
}
label
  enter_scale;

var
  scale_num : string [11];

Begin

Str(scale:11:8,scale_num);

enter_scale:

         ClrScr;
         writeln;
         writeln('     Output Scale = ',scale_num);
         GotoXY(21,2);
         numlen := 12;
         GetNum(menu_response,text_string,numlen);
         if text_string = ' ' then
            text_string := '0';
         if text_string = '+' then
            text_string := '0';
         if text_string = '-' then
            text_string := '0';
           case menu_response of
              -7, -6, -5, -4, -3:
                 goto enter_scale;
              0:
                 Val(text_string,scale,numlen);
           end; {case}

End; {procedure : SCALE}
