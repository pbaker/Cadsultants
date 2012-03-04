extern void far read_more(int);
extern void far comp_xy(void);
extern unsigned int far comp16_3(char,char,char);
extern int far comp_abs_coord(char,char);
extern int far comp_rel_coord(char,char);
extern void far display_segs(void);
extern void far my_diag(char *);
extern void far display_part_seg(void);
extern void far set_joystick(int);
extern char far break_flag(void);
extern void far insert_ours();
extern void far get_our_value();
extern void far get_sub_value();
extern void far insert_sub();
extern void far compile_sub_curser();
extern void far display_a_seg(int);
extern void far megatek_update(void);
extern void far my_ugout(int);
extern int far sub6_abs_coord(int);
extern int far sub6_rel_coord(int);
extern int far comp_coord(int);
extern void far matrix_update(void);
extern void far interp_my_sub6(void);
extern void far interp_sub(int);
extern void far my_pckb(void);


/*bios routines actually used in d135 emulation*/

extern void far read_character(int far *,int far *);
extern void far output_character(int,int);
extern void far make_win(int,int,int,int);
extern void far clear_win(int);
extern void far save_win(void);
extern void far restore_win(void);
extern void far goto_row_col(int,int);
extern void far read_curser_pos(void);
extern void far blinking_string(char *,int,int);
extern void far normal_string(char *,int,int);
extern void far turn_on_curser(void);
extern void far turn_off_curser(void);




/*externs of generic graphics board function calls*/
extern void far my_line3(int,int,int,int,int,int);
extern void far my_char(char,int,int,int,int);

extern void far my_size_chars(int);
extern void far my_set_viewport(int,int,int,int,int);
extern void far my_lock_viewport(void);
extern void far my_scroll(int);
extern void far my_color(int);
extern void far color_reg(int,int,int,int);
extern void far my_dash_pattern(int);
extern void far my_dash_enable(int);
extern void far my_line_xy(int,int,int,int);
extern void far place_char(int,int,int,int);
extern void far clear_page(int);
extern void far clear_screen(void);
extern void far draw_mode(int);
extern void far my_circle(int,int,int);
extern void far my_rdmask(int);
extern void far my_wrmask(int);


/*the error_logger function*/
extern void far error_logger(char *);


extern void far control_window(void);
extern int far choose_menu(char far *,char far *[],int,int);
extern int far information_menu(char far *,char far *[],int,char,char);
extern void far error_message(int,char far *[],int);



extern int far megatek_seek(int,long);
extern long far my_ftell(void);
extern void far goto_megatek_end(void);
extern void far insert_megatek(void);
extern void far report_seek_error(int,long);

extern int far get_next_free_page(void);
