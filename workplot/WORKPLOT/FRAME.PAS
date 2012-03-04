procedure Frame(ULX, ULY, LRX, LRY: Integer);

{            Frame - Sets Up the Frame in Which
                     Information Will Be Displayed In
---------------------------------------------------------------------------
}

var
  I : Integer;

Begin {Frame}

   GotoXY(ULX-1, ULY-1);
   Write(chr(201));
   for I := (ULX) to (LRX) do
     write (chr(205));

   Write(chr(187));

   for i := (ULY) to (LRY) do
   begin
     GotoXY(ULX-1,i); Write(chr(186));
     GotoXY(LRX+1,i); Write(chr(186));
   end;

   GotoXY(ULX-1, LRY+1);
   Write(chr(200));

   for i := (ULX) to (LRX) do
     Write(chr(205));

   Write(chr(188));

End; {procedure : FRAME}