Procedure Device(var menu_response,
                     device_type  : integer;
                 var diskfile     : file_length;
                 var add_text     : add_text_size);
{
         DEVICE - Allows selection of output device
-------------------------------------------------------------------------
}

label
  pick_device,enter_df1,enter_df2,done;

var

  dummy_ch : char;
  save_name : string [35];

Begin

  save_name := diskfile;

pick_device:

   number_of_options := 5;
   header := 'Select Output Device';

   for i := 1 to 5 do
      menu_option [i] := device_menu [i];

   output_type := device_type;

   Choose(menu_option,header,number_of_options,
          menu_response,output_type,text_string);

   case menu_response of
      -7, -6, -5, -4, -3:
         goto pick_device;
      0:
      begin
         case device_type of
            1:               {LPT:}
               add_text [2] := device_menu[device_type];

            2:               {COM1:}
            begin
               writeln;
               writeln('    COM1: not implemented');
               writeln('    See Documentation for Alternate Method');
               write  ('    Any key to continue... ');
               read(Kbd,dummy_ch);
               goto pick_device;
            end;

            3:               {COM2:}
            begin
               writeln;
               writeln('    COM2: not implemented');
               writeln('    See Documentation for Alternate Method');
               write  ('    Any key to continue... ');
               read(Kbd,dummy_ch);
               goto pick_device;
            end;

            4:               {CRT}
               add_text [2] := device_menu[device_type];

            5:               {Diskfile}
            begin

               enter_df1:

               writeln;
               write ('     Name of Disk File : ');
               BufLen := 30;
               GetText(menu_response,diskfile,BufLen);
               case menu_response of
                  -7, -6, -5, -4, -3:
                  begin
                     writeln;
                     writeln;
                     writeln('     Invalid file specification');
                     write  ('     Any key to continue... ');
                     read(Kbd,dummy_ch);
                     diskfile := save_name;
                     goto enter_df1;
                  end;
                  -2:
                     goto done;
                  -1:
                     goto pick_device;
               end; {case}

               assign(Output,diskfile);
               {$I-};reset(Output);{$I+};
               exist := (IOresult = 0);

               if exist then

               begin
                  writeln;
                  writeln;
                  writeln('     File Exists');
                  write  ('     Overwrite? [Y/N]');
                  read(Kbd,dummy_ch);

                  if (dummy_ch = 'Y') or (dummy_ch = 'y') then
                    begin
                    end

                   else

                    goto pick_device;
               end

               else

               if Copy(diskfile,1,1) < '!' then
                  begin
                     writeln;
                     writeln;
                     writeln('     Invalid file specification');
                     write  ('     Any key to continue... ');
                     read(Kbd,dummy_ch);
                     diskfile := save_name;
                     goto enter_df1;
                  end;

               if diskfile = '' then
                  begin
                     writeln;
                     writeln;
                     writeln('     Invalid file specification');
                     write  ('     Any key to continue... ');
                     read(Kbd,dummy_ch);
                     diskfile := save_name;
                     goto enter_df1;
                  end;

               add_text [2] := diskfile;
               Close(Output);

            end;
         end; {case device_type}

      end;

      1, 4:
      begin
         device_type  := menu_response;
         add_text [2] := device_menu[menu_response];
      end;

      2:
      begin
         writeln;
         writeln('    COM1: not implemented');
         writeln('    See Documentation for Alternate Method');
         write  ('    Any key to continue... ');
         read(Kbd,dummy_ch);
         goto pick_device;
      end;

      3:
      begin
         writeln;
         writeln('    COM2: not implemented');
         writeln('    See Documentation for Alternate Method');
         write  ('    Any key to continue... ');
         read(Kbd,dummy_ch);
         goto pick_device;
      end;

      5:
      begin
         device_type := menu_response;
         enter_df2:
         writeln;
         write ('     Name of Disk File : ');
         BufLen := 30;
         GetText(menu_response,diskfile,BufLen);

         case menu_response of
            -7, -6, -5, -4, -3:
            begin
               writeln;
               writeln;
               writeln('     No file specified');
               write  ('     Any key to continue... ');
               read(Kbd,dummy_ch);
               diskfile := save_name;
               goto enter_df2;
            end;
            -2:
               goto done;
            -1:
               goto pick_device;
         end; {case}

         assign(Output,diskfile);
         {$I-};reset(Output);{$I+};
         exist := (IOresult = 0);
         if exist then
         begin
            writeln;
            writeln;
            writeln('     File Exists');
            write  ('     Overwrite? [Y/N]');
            read(Kbd,dummy_ch);
            if (dummy_ch = 'Y') or (dummy_ch = 'y') then
              begin
              end
            else
            goto pick_device;
         end
         else

               if Copy(diskfile,1,1) < '!' then
                  begin
                     writeln;
                     writeln;
                     writeln('     Invalid file specification');
                     write  ('     Any key to continue... ');
                     read(Kbd,dummy_ch);
                     diskfile := save_name;
                     goto enter_df2;
                  end;

               if diskfile = '' then
                  begin
                     writeln;
                     writeln;
                     writeln('     Invalid file specification');
                     write  ('     Any key to continue... ');
                     read(Kbd,dummy_ch);
                     diskfile := save_name;
                     goto enter_df2;
                  end;
            add_text [2] := diskfile;
         Close(Output);
      end;
   end; {case}

done:

End; {procedure : DEVICE}
