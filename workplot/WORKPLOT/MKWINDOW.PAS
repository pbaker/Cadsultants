Procedure MkWindows;
{
      MKWINDOWS - Constructs two windows
----------------------------------------------------------------------------
}

Begin

   for i := 1 to windows do
   begin
      TextColor(window_color[i]);
      Frame(window_coord[i,1], window_coord[i,2],
            window_coord[i,3], window_coord[i,4]);
   end;

   SelectWindow(1);

End; {procedure : MKWINDOWS}
