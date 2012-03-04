Procedure SelectWindow (Win : integer);

{        SelectWindow - Selects Window in Which Coordinates
                        and other Needed Information Are
                        to be Displayed
----------------------------------------------------------------------
}

Begin

   TextColor(window_color[win]);
   Window(window_coord[win,1],window_coord[win,2],window_coord[win,3],
          window_coord[win,4]);
   GotoXY(1,1);

End; {procedure : SELECTWINDOW}
