Procedure Title;

{          Title - Sets Up the Initial Title Page
------------------------------------------------------------------------
}

var
  I : integer;
  dummy : char;

Begin

Window(1,1,80,25);
ClrScr;
TextColor(15);
Frame(13,10,67,18);
Window(13,10,67,18);
GotoXY(1,1);
writeln('');
writeln('           PC WorkPLOT II  -   Version 1.0');
writeln('');
textcolor(4);
writeln('           COPYRIGHT 1986,87,88 CADSULTANTS, INC.');
writeln('           ALL RIGHTS RESERVED           ');
writeln('');
TextColor(15 + blink);
write  ('           Strike Any Key To Continue...');
read(Kbd,dummy);
{
     Time Delay for message
}
Window(1,1,80,25);
ClrScr;

End; {procedure : TITLE}
