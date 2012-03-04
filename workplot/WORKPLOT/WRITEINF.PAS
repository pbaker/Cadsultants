Procedure WriteInfo(var device_type : integer;
                    var text_string : full_line);

Type
  Regpack = record
             ax,bx,cx,dx,bp,si,di,ds,es,flags: integer;
           end;
Var
  cvalue, i : integer;
  Regs : Regpack;

Const
  IntNo : Integer = 96;

Begin

   case device_type of
      1:
         {writeln(Lst,text_string);}
          begin
{           writeln(Trm,'Length = ',Length(text_string));}

           for i := 1 to Length(text_string) do
             begin
{               writeln(Trm,'I = ',i);}
               cvalue := Ord(text_string[i]);

{               writeln(Trm,'Character value = ',cvalue);}

               with Regs do
                 begin
                 ax := cvalue;
                 end;

               Intr(96,Regs);

              end;
           end;

      2:
         writeln(Aux,text_string);
      4:
         writeln(Trm,text_string);
      5:
         writeln(Output,text_string);
   end; {case}

End; {procedure : WRITEINFO}
