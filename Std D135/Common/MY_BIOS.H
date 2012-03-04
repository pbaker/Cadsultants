/*externs of MY BIOS "C" Functions*/

#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1



extern char far char_save_page[25][80];
extern char far attr_save_page[25][80];

extern int far win_x_low;
extern int far win_y_low;
extern int far win_x_up;
extern int far win_y_up;

extern int far row_curser;
extern int far col_curser;



extern void far read_character(int far *,int far *);
extern void far output_character(int,int);
extern void far blinking_string(char far *,int,int);
extern void far normal_string(char far *,int,int);
extern void far turn_off_curser(void);
extern void far turn_on_curser(void);
extern void far make_win(int,int,int,int);
extern void far clear_win(int);
extern void far save_win(void);
extern void far restore_win(void);
extern void far goto_row_col(int,int);
extern void far read_curser_pos(void);
extern void far draw_box(void);
extern void far teletype(int);
extern void far scroll_up(int,int,int,int,int,int);
extern void far scroll_down(int,int,int,int,int,int);
