Procedure Choose (var menu_option        : menu_option_size;
                  var header             : header_length;
                  var number_of_options,
                      menu_response,
                      default_option     : integer;
                  var text_string        : full_line);
{
                 C H O O S E - Menu Display Program
-----------------------------------------------------------------------
         This procedure displays up to 6 pages of menu options with
10 menu options per page max.  The program returns the following
values for menu response:

         menu_response            description
         -----------------------------------------------------
             -7              Backspace Key
             -6              ? or / Key
             -5              Pg Dn Key
             -4              Pg Up Key
             -3              Alternate Action - Up Arrow Key
             -2              Terminate        - Down Arrow Key
             -1              Reject           - Left Arrow Key
              0              Entry Complete   - Right Arrow Key
          1 thru 10          Function Keys
}
{
         Declarations
-----------------------------------------------------------
}

label
  last_page,
  display;

var
  temp_menu_option      : array [1..20] of full_line;
  substring             : string [1];
  function_key,
  I,J,L,
  number_of_pages,
  options,
  options_last_page,
  page_number,
  control_key           : integer;
  pages                 : real;
{
         Executable Portion
-----------------------------------------------------------
}
Begin
{
     Initialize Flags and Counters
}
   control_key     := 1;
   number_of_pages := 0;
   options := number_of_options;
{
     Calculate Number of full pages
     and Number of options on last page
}
   while options > 10 do
   begin
      options := options-9;
      number_of_pages := number_of_pages+1;
   end;
   options_last_page := options;
{
     Add a space before each menu option
     and check for default
}
   for I := 1 to number_of_options do
   begin
      substring := copy(menu_option[I],1,1);
      if (substring <> ' ') and (substring <> '>') then
         menu_option[I] := ' '+menu_option[I];
   end;
   if default_option <> 0 then
   begin
      menu_option [default_option] := copy(menu_option[default_option],2,80);
      menu_option [default_option] := '>'+menu_option[default_option];
   end;
{
     Load Header and Menu Options with string data
}
   if number_of_pages = 0 then
      goto last_page;
   for I := 1 to number_of_pages do
   begin
      for J := 1 to 9 do
      begin
         if I = 1 then
            temp_menu_option [J] := Concat('     ',Chr(J+48),' ',
                                    menu_option [J])
         else
            temp_menu_option [10*(I-1)+J] := Concat('     ',Chr(J+48),' ',
                                             menu_option[9*(I-1)+J]);
      end;
      temp_menu_option [10*I] := Concat('    ',Chr(49),Chr(48),
                                 '  Next Page');
   end;

last_page:

   case options_last_page of
      1..9:
      begin
         for I := 1 to options_last_page do
         begin
            if number_of_pages = 0 then
               temp_menu_option [I] := Concat('     ',Chr(I+48),' ',
                                       menu_option[I])
            else
               temp_menu_option [10*number_of_pages+I] := Concat('     ',
                    Chr(I+48),' ',menu_option[9*number_of_pages+I]);
         end;
      end;
      10:
      begin
         for I := 1 to 9 do
         begin
            if number_of_pages = 0 then
               temp_menu_option [I] := Concat('     ',Chr(I+48),' ',
                                       menu_option[I])
            else
               temp_menu_option [10*number_of_pages+I] := Concat('     ',
                    Chr(I+48),' ',menu_option[9*number_of_pages+I]);
         end;
         temp_menu_option [(number_of_pages+1)*10] := Concat('    ',
              Chr(49),Chr(48),' ',menu_option[9*number_of_pages+10]);
      end;
   end; {case}
{
     Display Menus
}
   page_number := 1;

display:

   ClrScr;
   writeln ('     ',header);
   writeln;
   if number_of_pages = 0 then
   begin
      function_key := options_last_page;
      for I := 1 to options_last_page do
         writeln (temp_menu_option [(page_number-1)*10+I]);
   end
   else
   begin
      function_key := 10;
      for I := 1 to 10 do
         writeln (temp_menu_option [(page_number-1)*10+I]);
   end;
{
     Check for Appropriate Key
}
   GetKey (function_key);
   case function_key of
   0,11..19, 23, 25, 27, 28, 31, 32,36:
      goto display;
   10:
      if number_of_pages <> 0 then
      begin
         number_of_pages := number_of_pages -1;
         page_number := page_number+1;
         goto display;
      end
      else
      begin
         menu_response := (page_number-1)*10+function_key-(page_number-1);
         text_string := menu_option[menu_response];
          L := Length(text_string);
           if copy(text_string,1,1) = '>' then
             text_string := copy(text_string,2,L-1);
      end;
   20:
      control_key := -7;        { Home Key }
   21:
      control_key := -3;        { ALTERNATE ACTION }
   22:
      control_key := -4;        { Pg Up Key }
   24:
   begin
      if page_number = 1 then
         control_key := -1      { REJECT }
      else
      begin
         page_number := page_number-1;
         number_of_pages := number_of_pages+1;
         goto display;
      end;
   end;
   26:
      begin
         text_string := menu_option[default_option];
          L := Length(text_string);
           if copy(text_string,1,1) = '>' then
             text_string := copy(text_string,2,L-1);
             control_key := 0;         { ENTRY COMPLETE }
      end;
   29:
      control_key := -2;        { TERMINATE }
   30:
      control_key := -5;        { Pg Dn Key }
   35:
      control_key := -6;        { ? / Key }
   else
   begin
      menu_response := (page_number-1)*10+function_key-(page_number-1);
      text_string := menu_option[menu_response];
      L := Length(text_string);
      if copy(text_string,1,1) = '>' then
         text_string := copy(text_string,2,L-1);
   end;
   end; {case}

   if control_key < 1 then
      menu_response := control_key;

End; {procedure CHOOSE}
