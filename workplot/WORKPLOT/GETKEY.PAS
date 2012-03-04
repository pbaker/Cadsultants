{$C-}
Procedure GetKey (var function_key : Integer);
{
              GetKey - Evaluates Keyboard Input
--------------------------------------------------------------------
         This procedure reads input from the keyboard and checks
to see if it is a valid response.
}

label
  done;

var
  FuncKey,
  KeyPressed       : boolean;
  ch               : char;
  menu_range,
  character_value              : integer;

Begin

   if KeyPressed then
   begin
      Read(Kbd,ch);
      case ch of
         #47, #63:           { ? / Key }
         begin
            function_key := 35;
            goto done;
         end;
         #8:                 { Backspace Key }
         begin
            function_key := 36;
            goto done;
         end;
         #27:
         begin
            Read(Kbd,ch);
            character_value := Ord(ch);
            menu_range := 58+function_key;
            case character_value of
               59..68:       { Function Keys 1 thru 10 }
               begin
                  if character_value > menu_range then
                  begin
                     function_key := 0;
                     goto done;
                  end;
                  function_key := character_value-58;
              end;
               71..83:       { Numeric Keypad }
                  function_key := character_value-51;
               104..112:     { Alt Function Keys }
                  function_key := character_value-93;
               else
               begin
                  function_key := 0;
                  goto done;
               end;
            end; {case}
         end;
         else
         begin
            function_key := 0;
            goto done;
         end;
      end; {case ch}
   end; {if KeyPressed}

done:

End; {procedure : GETKEY}
