Procedure Plotter(var menu_response,
                      plotter_type,
                      number_of_plotters,
                      x_offset,
                      y_offset             : integer;
                  var adjusted_scale       : real;
                  var plotter_menu         : menu_option_size;
                  var add_text             : add_text_size);
{
         PLOTTER - Allows selection of HP plotter
--------------------------------------------------------------------------
}
label
  pick_plotter;

Begin

pick_plotter:

         ClrScr;
         number_of_options := number_of_plotters;
         header := 'Select Plotter Type';
         for i := 1 to number_of_plotters do
            menu_option [i] := plotter_menu [i];
         output_type := plotter_type;
         Choose(menu_option,header,number_of_options,
                menu_response,output_type,text_string);
         case menu_response of
            -7, -6, -5, -4, -3:
               goto pick_plotter;
            0:
               add_text[3]  := plotter_menu[plotter_type];
            1..20:
            begin
               plotter_type := menu_response;
               add_text [3] := plotter_menu[menu_response];
               Assign(Input,add_text[7]);
               Reset(Input);
               for i := 1 to plotter_type do
               begin
                  readln(Input,text_string);
                  readln(Input,tek2hp_factor);
                  readln(Input,x_offset);
                  readln(Input,y_offset);
               end;
               adjusted_scale := scale*tek2hp_factor;
               Close(Input);
            end;
         end; {case}

End; {procedure : PLOTTER}
