/* MY_GLOBS.H  (FOR CLIPPER D135 ONLY) */



/*October 13, 1988 added conditional compilation for diagnostic stuff
using HAVE_DIAGS to for the #ifdef's (change my2funcs.c each time)*/


#define HAVE_DIAGS


#include <stdio.h>



#define TERMINATE  -1
#define ENTRY_COMPLETE  -2
#define REJECT  -3




#define FAILURE 0
#define SUCCESS 1
#define NEXT_MEGATEK_PAGE 16378
#define NUM_MEGA_WORDS 4090
#define MEGATEK 0
#define DISPLAY_LIST 1



#define GRAPHICS 1
#define TEXT 0
#define SUB_NUM_ELS 50
#define NULLCODE (char)100
#define NUMBER_OF_SEGS 30
#define SUB7_NUM_ELS 500
#define NUMBER_OF_SUBS 30

#include "my_subs.h"


#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1

#define SVWPT_PAGE_1 4

#define INTERP_A_SUB_TIME 0x1000
#define XFORM_ENABLE_INTERRUPT 0x2000
#define XFORM_DISABLE_INTERRUPT 0x3000
#define INSERT_2D_XLATE_INTERRUPT 0x4000
#define DL_BRANCH_INTERRUPT 0x5000
#define END_OF_SEGMENT_INTERRUPT 0x6000
#define MATRIX_SUB_INTERRUPT 0x7000




#define PAGE1_SUB1 2
#define PAGE2_SUB1 3
#define NO_DRAW_SUB1 4



#define THE_MATRIX  0
#define THE_2D_ORIGIN  1
#define THE_CLIPPING  2
#define THE_DASH_PATTERN  3
#define THE_DASH_ENABLE  4
#define THE_WHOLE_HEADER  5
#define THE_3D_ORIGIN  6
#define THE_Z_ORIGIN  7
#define THE_2D_XLATE 8


extern struct sub_holder *sub_struct_ptr;



/* BLT STUFF    blt */

/* pass to get_next_free_page() to identify the use of the new page */
#define BLT_LIST 2 

/* add to page number inside of available_pages[] to mark it as a BLT EMS page */
#define BLT_CODE 300    

/* the MAX number of BLT lists Allowed */
#define NUMBER_OF_BLT_LISTS 30

extern int  which_blt_list;   /* is the Active One */
extern int  blt_list_starts[];   /* first EMS page of Every BLT list */
extern int  retracted_column[];  /* contains the retracted menu column */




struct header_data
{
	char visibility,Xform_enable;
	int viewing_3d_matrix[16];
	int origin[3];
	int my_set_viewport_values[4];
	unsigned int dash_pattern;
	char dash_enable;
};

extern struct header_data  update_header_data;

/*
extern short  color_sub_table[50][10];
*/

extern char  segment_visibility[];

extern unsigned char  megatek_locate[50][64];
extern long  megatek_ends[];
extern unsigned char  *current_megatek_pointer;
extern long  current_megatek_insert_pos;
extern int  current_megatek_page;



extern long  current_word[];
extern long  last_word[];

/*
extern short  sub_calling_table[64][10];
extern short  our_versions_of_subs[10][100];
*/

struct ems_position
{
	int logical_page_num,offset;
};


extern struct ems_position  start_segment_ptrs[];
extern struct ems_position  end_segment_ptrs[];





struct our_element
{
	char op_code,flag1,flag2;
	int arg1,arg2,arg3,arg4;
};


struct dl_pointer
{
	char op_code;
	int logical_page_num,offset;
};


extern struct dl_pointer  dl_pointer_element;
extern struct dl_pointer  read_element;


#define TO_SCREEN_ONLY 0
#define TO_LIST_ONLY 1
#define TO_BOTH 2


extern int  which_insert_mode;

extern short  *current_pointer_to_dl;


extern int  current_color;
extern int  available_pages[];

extern int  handle;
extern int  physical_page;
extern int  logical_page;
extern int  num_unalloc_pages;
extern int  page_frame_segment_address;

extern int  current_logical_page;
extern int  current_physical_page;
extern int  current_offset;

extern short  *physical_pages_pointer;




extern struct our_element  *ptr_sub;

extern struct our_element  an_element;

extern struct our_element  our_sub7[];
extern struct our_element  our_sub6[];
extern struct our_element  our_sub3[];
extern struct our_element  my_sub6[];


extern int  insert_sub_x_Xlate[];
extern int  insert_sub_y_Xlate[];

/*
extern short  sub_display_lists[NUMBER_OF_SUBS][2*SUB_NUM_ELS];
*/

extern short  *sub_ends[];


extern short  *sub_host_to_pix_ptrs[NUMBER_OF_SUBS][SUB_NUM_ELS]; 
extern short  *current_sub_pointer;


extern int  size_dl_ptr_element;
extern int  size_display_list_header;


extern short my_2d_modelling_matrix[];
extern short  my_3d_modelling_matrix[];
extern short  my_2d_viewing_matrix[];
extern short  my_3d_viewing_matrix[];




struct a_word
{
	unsigned int most_sig,least_sig;
};


extern struct a_word  vm512;
extern struct a_word  vm345;
extern struct a_word  vm33536;
extern struct a_word  vm49536;
extern struct a_word  vm1170;
extern struct a_word  vm1166;
extern struct a_word  vm69;
extern struct a_word  vm7;

/*
extern union REGS  my_input_regs;
extern union REGS  my_output_regs;
*/
extern char  char_save_page[25][80];
extern char  attr_save_page[25][80];
extern int  win_x_low;
extern int  win_y_low;
extern int  win_x_up;
extern int  win_y_up;
extern int  row_curser;
extern int  col_curser;
extern int  type_char_read;
extern int  value_char_read;
extern int  curser_ratio;
extern int  button_byte;
extern int  mouse_phase;
extern char  depress_flag;
extern int  mouse_x;
extern int  mouse_y;


extern int  curser_x_off;
extern char  board_string[];
extern int  char_y_off;
extern int  char_x_off;
extern int  update_level;
extern char  doing_redisplay;
extern char  save_write_error;
extern char  plot_write_error;
extern char  time_redisplay;
extern char  write_error;
extern char  print_list;
extern int  list_size;
extern char  is_a_seg_open;
extern long  seg_starts[];
extern long  num_words_in_segs;
extern char  joy_off;
extern char  pen_press2;
extern int  pen2_x;
extern int  pen2_y;
extern int  num_op_code_4s;
extern int  num_op_code_0s;
extern int  ins_matrix_sub;
extern int  ins_matrix_seg;
extern char  rw_sub_w_0_1;
extern int  rw_which_seg;
extern int  size_an_element;
extern int  redis_old_x;
extern int  redis_old_y;
extern char  change_vis_to_true;
extern char  rotate_mode;
extern char  button1;
extern char  rewrite_color;
extern int  segment_calls[];
extern int  last_sub_rewritten;
extern int  version;
extern int  check_resp;
extern int  sub_starts[];
extern int  size_header;
extern char  drive_letter;
extern int  which_changed;
extern int  where_called[NUMBER_OF_SUBS][20];
extern char  quit_segment;
extern char  draw_black;
extern int  num_pages;
extern int  my_y_max;
extern char  ins_matrix_flag;
extern int  which_redis;
extern char  quit_redis;
extern int  text_col;
extern int  text_row;
extern int  graphics_text;
extern char  locate_called;
extern char  locate_1st_draw;
extern int  locate_seg;
extern int  save_joy_x;
extern int  save_joy_y;
extern char  change_sub3_6_7;
extern int  save_line_type;
extern int  save_my_x;
extern int  save_my_y;
extern int  my_sub6_ptr;
extern char  curser_displayed;	
extern int  which_page;
extern char  clear_seg_flag;
extern char  num_dims;
extern int  new_x;
extern int  new_y;
extern char  diag_flag;
extern char  read_joystick;
extern char  pen_press;
extern int  my_x;
extern int  my_y;
extern int  my_z;
extern int  old_x;
extern int  old_y;
extern char  joy_enable;
extern char  rw_op_code;
extern int  sub1_0_seg;
extern char  black;
extern char  rw_sub1_0_flag;
extern long  ptr_rw_sub1_0;
extern char  save_color;
extern int  save_sub_insert_ptr;
extern char  sub_rewrite;
extern char  no_draw_flag;


extern int  segment_nums[];

extern unsigned char  megatek_data[];

extern FILE  *out_ptr;
extern FILE  *rewrite_file;
extern FILE  *save_file_ptr;
extern FILE  *plot_file;
extern FILE  *display_file;
extern FILE  *copy_header_ptr;
extern FILE  *error_log_ptr;


extern char  path_error_log[];
extern char  errors_happened;


extern int  sub_nums[];

extern char  fill_sub;
extern char  our_call;


extern int  sub_insert_ptrs[];



extern int  which_cseg;
extern int  which_sub;

/*
extern float  x_y_z[];
extern float  x_y_z_prime[];
extern float  x_y[];
extern float  x_y_prime[];
*/

extern int  save_insert_ptr;
extern char  print_flag;
extern int  num_transform_segs;
extern int  num_matrixes_ins;
extern char  rewrite_flag;
extern char  draw_flag;

extern int  line_type;
extern unsigned char  command;
extern char  out_string[];
extern char  diag_string[];
extern int  xx;
extern int  yy;
extern int  zz;
extern int  x;
extern int  y;
extern int  old_segment;
extern int  new_segment;
extern int  x_origin;
extern int  y_origin;
extern int  z_origin;
extern int  a_segment;
extern unsigned char  flag;
extern unsigned char  size;
extern unsigned char  short_x;
extern unsigned char  short_y;
extern unsigned char  most_x;
extern unsigned char  most_y;
extern unsigned char  least_x;
extern unsigned char  least_y;
extern unsigned char  temp;
extern unsigned char  a_char;
extern unsigned char  arg2;
extern unsigned char  arg3;
extern unsigned int  dash_pat;
extern unsigned int  pick_label;
extern unsigned int  num_millis;
extern unsigned int  id;
extern unsigned int  switches_flag;
extern int  displacement;
extern int  a_subroutine;
extern int  old_subroutine;
extern int  new_subroutine;
extern int  horz_lower;
extern int  vert_lower;
extern int  horz_upper;
extern int  vert_upper;
extern int  dx;
extern int  dy;
extern int  pick_width;
extern int  pick_height;
extern int  diag_num;
extern int  mode_flag;
extern int  sub_level;
extern int  joy_x;
extern int  joy_y;

