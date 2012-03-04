extern void far read_more();
extern void far comp_xy();
extern unsigned int far comp16_3();
extern int far comp_abs_coord();
extern int far comp_rel_coord();
extern void far display_segs();
extern void far my_diag();
extern void far display_part_seg();
extern void far set_joystick();
extern char far break_flag();
extern unsigned long far compress_1st();
extern unsigned long far compress_next();
extern unsigned long far compr16_1st();
extern unsigned long far compr16_next();
extern void far insert_ours();
extern void far get_our_value();
extern void far get_sub_value();
extern void far insert_compr();
extern void far insert_sub();
extern void far my_line3();
extern void far my_char();
extern void far compile_sub_curser();
extern void far display_a_seg();
extern void far megatek_update();
extern void far my_ugout();
extern int far sub6_abs_coord();
extern int far sub6_rel_coord();
extern int far comp_coord();
extern void far matrix_update();
extern void far interp_my_sub6();
extern void far interp_sub();
extern void far sleep();
extern void far my_pckb();

/*bios routines actually used in d135 emulation*/

extern void far read_character(int far *,int far *);
extern void far output_character(int,int);
extern void far make_win();
extern void far clear_win();
extern void far save_win();
extern void far restore_win();
extern void far goto_row_col();
extern void far read_curser_pos();
extern void far blinking_string();
extern void far normal_string();
extern void far turn_on_curser();
extern void far turn_off_curser();
extern void far reverse_string(char far *,int,int);


/*externs of generic graphics board function calls*/
extern void far my_size_chars();
extern void far my_set_viewport(int,int,int,int,int);
extern void far my_lock_viewport();
extern void far my_scroll();
extern void far my_color();
extern void far color_reg();
extern void far my_dash_pattern();
extern void far my_dash_enable();
extern void far my_line_xy();
extern void far place_char();
extern void far clear_page();
extern void far clear_screen();
extern void far draw_mode();

/*the error_logger function*/
extern void far error_logger(char *);


extern int far get_next_free_page(int);

extern int far megatek_seek(int,long);
extern long far my_ftell(void);
extern void far goto_megatek_end(void);
extern long far my_ftell(void);
extern void far insert_megatek(void);
extern void far report_seek_error(int,long);



extern int far choose_menu(char far *,char far *[],int,int);
extern int far information_menu(char far *,char far *[],int,char,char);
extern void far error_message(int,char far *[],int);
