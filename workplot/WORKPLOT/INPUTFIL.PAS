Procedure Inputfile(var menu_response     : integer;
                    var filename          : file_length);

{
         INPUTFILE - Key in new filename to input
                     and check to see if it exists
-----------------------------------------------------------------------------
}

label
  enter_input,done;

var

  dummy_ch  : char;
  save_name : string [35];

Begin

  save_name := filename;

enter_input:
         ClrScr;
         writeln;
         write('   Enter File Name : ');
         gotoXY(22,2);
         BufLen := 30;
         GetText(menu_response,filename,BufLen);
         case menu_response of
            -7, -6, -5, -4, -3:
            begin
               writeln('     No Filename, Select Again');
               filename := save_name;
               goto enter_input;
            end;
            -2, -1:
               goto done;
      end; {case}
{
     Open input file
}
         assign(Input,filename)
         {$I-};reset(Input);{$I+};
         exist := (IOresult = 0);
         if not exist then
         begin
            writeln;
            writeln;
            writeln('     File Does Not Exist');
            write  ('     Any key to continue...');
            read(Kbd,dummy_ch);
            filename := save_name;
            goto enter_input;
         end;

done:

End; {procedure : INPUTFILE}
