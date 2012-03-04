Procedure PageMgr(var message_string      : message_array;
                  var message_count,
                      menu_response,
                      page                : integer);
{
          PageMgr - Manages Help Screen Display
--------------------------------------------------------------------------------
}

const
  page_down = 'PgDn';
  page_up   = 'PgUp';
  max_lines = 6;

label
  all_done;

var
  total_pages      : real;
  no_lines         : integer;

Begin

   SelectWindow(2);
   ClrScr;
   GotoXY(1,1);
   no_lines := max_lines-2;
{
      Determine Maximum Number of Pages
}
    total_pages := (message_count-2)/no_lines;
    if Frac(total_pages) > 0 then
       total_pages := Trunc(total_pages)+1;

    writeln(message_string[1]);
    writeln(message_string[2]);

case menu_response of
-5:                 {Pg Dn Response}
begin
   if page = total_pages then
   begin
      for I := (no_lines)*(page-1)+(max_lines-3) to message_count do
         writeln (message_string[I]);
      goto all_done;
   end;
   if message_count-(no_lines*page+2) > no_lines then
   begin
      for I := no_lines*page+3 to message_count-(no_lines*(page-1)+2) do
         writeln(message_string[I]);
   end
   else
   begin
      for I := no_lines*page+3 to message_count do
         writeln(message_string[I]);
   end;
   page := page+1;
end;
-4:               { Pg Up Response }
begin
   if page > 1 then
      page := page-1;
   if page < 2 then
   begin
      for I := 3 to max_lines do
         writeln(message_string[I]);
      goto all_done;
   end;
   for I := no_lines*(page-1)+3 to message_count-(no_lines*(page-2)+2) do
      writeln(message_string[I]);
end;
else
begin
   if message_count > max_lines then
   begin
      for I := 3 to max_lines do
           writeln(message_string[I]);
   end
   else
   begin
      for I :=3 to message_count do
         writeln(message_string[I]);
   end;
   page :=page+1;
end;
end;  { case }

all_done:

   if total_pages > 1 then
   begin
      if page > 1 then
      begin
         GotoXY(65,1);
         TextColor(Cyan + Blink);
        writeln(page_up);
      end;
      if page < total_pages then
      begin
         GotoXY(65,6);
         TextColor(Cyan + Blink);
         writeln(page_down);
      end;
   end;

   SelectWindow(1);

End;  { procedure : PAGEMGR }
