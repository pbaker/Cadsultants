(*
     CONVERSION OF TEK 4014 DATA TO PC GRAPHICS
*)
{$I GRAPH.P }

label
  continue;
type
  graphics_data  =  file of byte;
var
  input_file         : graphics_data;
  escape_string      : array [1..22] of string [30];
  control_string     : array [1..8]  of string [30];
  mode_select_string : array [1..12] of string [30];
  file_name          : string [15];
  output_string      : string [30];
  single_byte        : byte;
  x,y                : real;
  pointer,
  escape_flag,
  graphics_mode_flag,
  new_entity_flag    : integer;
  hiy,loy,hix,lox,
  xstart,ystart,
  xint,yint          : integer;
begin
ClrScr;
{GraphWindow(0,0,639,199);}
GraphColorMode;
Palette(2);
(*
           ASCII ESCAPE SEQUENCES
  -----------------------------------------------
*)
escape_string[1]  := '13 x 22 character size';  (* ESC-8 *)
escape_string[2]  := '12 x 20 character size';  (* ESC-9 *)
escape_string[3]  := ' 8 x 13 character size';  (* ESC-: *)
escape_string[4]  := ' 7 x 12 character size';  (* ESC-; *)
escape_string[5]  := 'Solid vector';            (* ESC-' *)
escape_string[6]  := 'Dotted vector';           (* ESC-a *)
escape_string[7]  := 'Dot-dash vector';         (* ESC-b *)
escape_string[8]  := 'Short dash vector';       (* ESC-c *)
escape_string[9]  := 'Long dash vector';        (* ESC-d *)
escape_string[10] := escape_string[5];          (* ESC-e *)
escape_string[11] := escape_string[5];          (* ESC-f *)
escape_string[12] := escape_string[5];          (* ESC-g *)
escape_string[13] := escape_string[5];          (* ESC-h *)
escape_string[14] := escape_string[6];          (* ESC-i *)
escape_string[15] := escape_string[7];          (* ESC-j *)
escape_string[16] := escape_string[8];          (* ESC-k *)
escape_string[17] := escape_string[9];          (* ESC-l *)
escape_string[18] := escape_string[5];          (* ESC-m *)
escape_string[19] := escape_string[5];          (* ESC-n *)
escape_string[20] := escape_string[5];          (* ESC-o *)
escape_string[21] := 'Enable bypass';           (* ESC-CTRL-X *)
escape_string[22] := 'Interactive data block request';  (* ESC-ENQ *)
(*
                ASCII CONTROL SEQUENCES
 -----------------------------------------------------
*)
control_string[1] := 'Bell';                    (* CTRL-G *)
control_string[2] := 'Backspace';               (* CTRL-H *)
control_string[3] := 'Horizontal tab';          (* CTRL-I *)
control_string[4] := 'Line feed';               (* CTRL-J *)
control_string[5] := 'Reverse line feed';       (* CTRL-K *)
control_string[6] := 'Carriage return';         (* CTRL-M *)
control_string[7] := 'Continue transmission';   (* CTRL-Q *)
control_string[8] := 'Stop transmission';       (* CTRL-S *)
(*
                 MODE SELECT COMMANDS
 ------------------------------------------------------
*)
mode_select_string[1]  := 'Alpha-numeric mode';    (* CTRL-M*)
mode_select_string[2]  := mode_select_string[1];   (* CTRL-SHIFT-O*)
mode_select_string[3]  := mode_select_string[1];   (* ESC-CTRL-L*)
mode_select_string[4]  := mode_select_string[1];   (* SHIFT-HOME*)
mode_select_string[5]  := mode_select_string[1];   (* CTRL-SHIFT-CLEAR*)
mode_select_string[6]  := mode_select_string[1];   (* CLEAR*)
mode_select_string[7]  := mode_select_string[1];   (* RETRN*)
mode_select_string[8]  := 'Enter Graphics mode';   (* CTRL-SHIFT-M*)
mode_select_string[9]  := 'Point-Plot mode';       (* CTRL-SHIFT-L*)
mode_select_string[10] := 'Incremental plot mode'; (* CTRL-SHIFT-N*)
mode_select_string[11] := 'Interactive mode';      (* ESC-CTRL-Z*)
mode_select_string[12] := mode_select_string[11];  (* ESC-SUB*)
(*
   Initial Settings
*)
pointer := 0;
escape_flag := 0;
graphics_mode_flag := 0;
new_entity_flag := 0;

(*
   Input and Open File to Decode
*)
write ('File to Decode: ');
readln (file_name);
assign (input_file,file_name);
reset (input_file);
(*
    Begin Looking at Bytes Individually
*)
while not Eof(input_file) do
   begin
      read (input_file,single_byte);
      output_string := '';
(*
   Procedure to handle escape sequences
*)
      if escape_flag = 1 then
      begin
         escape_flag := 0;
         case single_byte of
            5:
               output_string := escape_string[22];
            12:
            begin
               output_string := mode_select_string[3];
               graphics_mode_flag := 0;
            end;
            24:
               output_string := escape_string[21];
            26:
            begin
               output_string := mode_select_string[11];
               graphics_mode_flag := 0;
            end;
            56..59:
               output_string := escape_string[single_byte-55];
            96..110:
               output_string := escape_string[single_byte-91];
         end; {case single_byte}
         {writeln (single_byte,'          ',output_string);}
         goto continue;
      end; {if escape_flag}

      case single_byte of
         27:
         begin
            escape_flag := 1;
            {writeln (single_byte,'         ',output_string);}
         end;
(*
    Procedure to handle control characters
*)
         7..11:
         begin
            output_string := control_string[single_byte-6];
            {writeln (single_byte,'         ',output_string);}
         end;
         13:
         begin
            output_string := mode_select_string[1];
            graphics_mode_flag := 0;
            {writeln (single_byte,'         ',output_string);}
         end;
         17:
         begin
            output_string := control_string[7];
            {writeln (single_byte,'         ',output_string);}
         end;
         19:
         begin
            output_string := control_string[8];
            {writeln (single_byte,'         ',output_string);}
         end;
         28:
         begin
            output_string := mode_select_string[9];
            graphics_mode_flag := 0;
            {writeln (single_byte,'         ',output_string);}
         end;
         29:
         begin
            output_string := mode_select_string[8];
            graphics_mode_flag := 1;
            new_entity_flag := 1;
            {writeln (single_byte,'         ',output_string);}
         end;
         30:
         begin
            output_string := mode_select_string[10];
            graphics_mode_flag := 0;
            {writeln (single_byte,'         ',output_string);}
         end;
         31:
         begin
            output_string := mode_select_string[2];
            graphics_mode_flag := 0;
            {writeln (single_byte,'         ',output_string);}
         end;
(*
   Procedure to handle valid characters (ASCII 32-127)
*)
         32..63:
         begin
            output_string := Chr(single_byte);
            if graphics_mode_flag = 1 then
            begin
               pointer := pointer+1;
               if pointer = 1 then
               begin
                  hiy := single_byte;
                  hiy := (hiy-32)*32;
                  {writeln (single_byte,'         ',output_string,}
                         {  '          hiy = ',hiy);}
               end;
               if pointer = 3 then
               begin
                  hix := single_byte;
                  hix := (hix-32)*32;
                  {writeln (single_byte,'         ',output_string,}
                        {   '          hix = ',hix);}
               end; {if pointer}
            end; {if graphics_mode_flag}
         end; {case 32..63}
         96..127:
         begin
            output_string := Chr(single_byte);
            if graphics_mode_flag = 1 then
            begin
               pointer := 2;
               loy := single_byte;
               loy := loy-96;
               {writeln (single_byte,'         ',output_string,}
                      {  '          loy = ',loy);}
            end; {graphics_mode_flag}
         end; {case 96..127}
         64..95:
         begin
            output_string := Chr(single_byte);
            if graphics_mode_flag = 1 then
            begin
               pointer := 4;
               lox := single_byte;
               lox := lox-64;
               {writeln (single_byte,'         ',output_string,}
                       { '          lox = ',lox);}
               x := hix+lox;
               y := hiy+loy;
               x := 319-x*(319/1023);
               y := 199-y*(199/875);
               xint := Round(x);
               yint := Round(y);
               {writeln ('                                 x coord: ',xint);}
               {writeln ('                                 y coord: ',yint);}
               pointer := 0;
               if new_entity_flag = 0 then
               begin
                  draw(xstart,ystart,xint,yint,2);

                  xstart := xint;
                  ystart := yint;
               end; {if new_entity_flag}
               if new_entity_flag = 1 then
               begin
                  xstart := xint;
                  ystart := yint;
                  new_entity_flag := 0;
               end; {if new_entity_flag}
            end; {if graphics_mode_flag}
         end; {case 64..95}
      end; {case single_byte}
{   writeln (single_byte,'         ',output_string);}
   continue:
   end; {while not Eof}
close (input_file);
end.