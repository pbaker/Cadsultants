
/*  HYPER  D135  (TRIAL)       MY_UTILS.H  */


typedef void (*ERROR_INSTRUCTIONS)();
extern void  error_handler();
extern void  null_error_function();



extern void  single_view_update();



extern void  read_more();
extern void  comp_xy();
extern unsigned int  comp16_3();
extern int  comp_abs_coord();
extern int  comp_rel_coord();
extern void  display_segs();
extern void  my_diag();
extern void  display_part_seg();
extern void  set_joystick();
extern char  break_flag();
extern void  insert_ours();
extern void  get_our_value();
extern void  get_sub_value();
extern void  insert_compr();
extern void  insert_sub();
extern void  my_line3();
extern void  my_char();
extern void  compile_sub_curser();
extern void  display_a_seg();
extern void  megatek_update();
extern void  my_ugout();
extern int  sub6_abs_coord();
extern int  sub6_rel_coord();
extern int  comp_coord();
extern void  matrix_update();
extern void  interp_my_sub6();
extern void  interp_sub();
extern void  sleep();
extern void  my_pckb();

/*bios routines actually used in d135 emulation*/

extern void  read_character();
extern void  output_character();
extern void  make_win();
extern void  clear_win();
extern void  save_win();
extern void  restore_win();
extern void  goto_row_col();
extern void  read_curser_pos();
extern void  blinking_string();
extern void  normal_string();
extern void  turn_on_curser();
extern void  turn_off_curser();
extern void  reverse_string();


/*externs of generic graphics board function calls*/
extern void  my_size_chars();
extern void  my_set_viewport();
extern void  my_lock_viewport();
extern void  my_scroll();
extern void  my_color();
extern void  color_reg();
extern void  my_dash_pattern();
extern void  my_dash_enable();
extern void  my_line_xy();
extern void  place_char();
extern void  clear_page();
extern void  clear_screen();
extern void  draw_mode();

/*the error_logger function*/
extern void  error_logger();


extern int  get_next_free_page();

extern int  megatek_seek();
extern long  my_ftell();
extern void  goto_megatek_end();
extern long  my_ftell();
extern void  insert_megatek();
extern void  report_seek_error();



extern int  choose_menu();
extern int  information_menu();
extern void  error_message();

