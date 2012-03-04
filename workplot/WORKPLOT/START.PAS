Procedure Start(var menu_response,
                    op             : integer;
                var add_text       : add_text_size);

label
  strt;

Begin
   output_type := 0;
   KeyInfo(message_string,message_count,page);

strt:

   PageMgr(message_string,message_count,menu_response,page);
   output_type := 6;
   number_of_options := 6;
   header := 'Select Operation';
   menu_option [1] := main_menu[1]+add_text[1];
   menu_option [2] := main_menu[2]+add_text[2];
   menu_option [3] := main_menu[3]+add_text[3];
   menu_option [4] := main_menu[4]+add_text[4];
   menu_option [5] := main_menu[5];
   menu_option [6] := main_menu[6];
   Choose(menu_option,header,number_of_options,
          menu_response,output_type,text_string);
   case menu_response of
      0:
         op := 6;
      -7, -6, -5, -4, -3:
         goto strt;
      1..6:
         op := menu_response;
   end; {case}

End; {procedure : START}
