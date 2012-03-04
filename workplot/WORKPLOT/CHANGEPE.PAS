Procedure ChangePen(var menu_response : integer;
                    var add_text      : add_text_size);
{
         CHANGEPEN - Change Pen Assignments to Plotter
--------------------------------------------------------------------------
}
const
  plot_menu : array [1..15] of string [30]
            = ('Blue       plots with pen # ',
               'Green      plots with pen # ',
               'Cyan       plots with pen # ',
               'Red        plots with pen # ',
               'Magenta    plots with pen # ',
               'Yellow     plots with pen # ',
               'White      plots with pen # ',
               'Olive      plots with pen # ',
               'Pink       plots with pen # ',
               'Brown      plots with pen # ',
               'Orange     plots with pen # ',
               'Purple     plots with pen # ',
               'Dark Red   plots with pen # ',
               'Aquamarine plots with pen # ',
               'Gray       plots with pen # ');
label
  change_pen,enter_pen;
var
  pen_number : char;

Begin

   output_type := 0;

change_pen:

         number_of_options := 15;
         header := 'Change Pen Assignments';
         for i := 1 to 15 do
            begin
               menu_option [i]  := plot_menu[i]+color_text[i];
            end;
         Choose(menu_option,header,number_of_options,
                menu_response,output_type,text_string);
         case menu_response of
            -7, -6, -5, -4, -3:
               goto change_pen;
            1..15:
            begin
enter_pen:
               ClrScr;
               writeln;
               write('     ',plot_menu[menu_response]);
               read(Kbd,pen_number);
               write(pen_number);
               if (pen_number >= '0') and (pen_number <= '9') then
               begin

{                  Str(pen_number,color_text[menu_response]);
                   goto change_pen;
}
                  color_text[menu_response] := pen_number;
                  goto change_pen;

               end
               else
                  goto enter_pen;
            end;
         end; {case}

End; {procedure : CHANGEPEN}
