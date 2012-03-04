/* MY_GLOBS.H  (FOR CLIPPER D135 ONLY) */



/*October 13, 1988 added conditional compilation for diagnostic stuff
using HAVE_DIAGS to for the #ifdef's (change my2funcs.c each time)*/

/*#define HAVE_DIAGS*/



#include "stdio.h"
#include "dos.h"



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



struct header_data
{
	char visibility,Xform_enable;
	int viewing_3d_matrix[16];
	int origin[3];
	int my_set_viewport_values[4];
	unsigned int dash_pattern;
	char dash_enable;
};

extern struct header_data far update_header_data;


extern short far color_sub_table[50][10];
extern char far segment_visibility[];

extern unsigned char far megatek_locate[50][64];
extern long far megatek_ends[];
extern unsigned char far *current_megatek_pointer;
extern long far current_megatek_insert_pos;
extern int far current_megatek_page;



extern long far current_word[];
extern long far last_word[];

extern short far sub_calling_table[64][10];
extern short far our_versions_of_subs[10][100];


struct ems_position
{
	int logical_page_num,offset;
};


extern struct ems_position far start_segment_ptrs[];
extern struct ems_position far end_segment_ptrs[];





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


extern struct dl_pointer far dl_pointer_element;
extern struct dl_pointer far read_element;


#define TO_SCREEN_ONLY 0
#define TO_LIST_ONLY 1
#define TO_BOTH 2

extern int far *interrupt_reason_ptr;

extern int far which_insert_mode;

extern short far *current_pointer_to_dl;


extern int far current_color;
extern int far available_pages[];

extern int far handle;
extern int far physical_page;
extern int far logical_page;
extern int far num_unalloc_pages;
extern int far page_frame_segment_address;

extern int far current_logical_page;
extern int far current_physical_page;
extern int far current_offset;

extern short far *physical_pages_pointer;




extern struct our_element huge *ptr_sub;

extern struct our_element far an_element;

extern struct our_element huge our_sub7[];
extern struct our_element huge our_sub6[];
extern struct our_element huge our_sub3[];
extern struct our_element huge my_sub6[];


extern int far insert_sub_x_Xlate[];
extern int far insert_sub_y_Xlate[];

extern short far sub_display_lists[NUMBER_OF_SUBS][2*SUB_NUM_ELS];
extern short far *sub_ends[];


extern short far *sub_host_to_pix_ptrs[NUMBER_OF_SUBS][SUB_NUM_ELS]; 
extern short far *current_sub_pointer;


extern int far size_dl_ptr_element;
extern int far size_display_list_header;


extern int far my_2d_modelling_matrix[];
extern int far my_3d_modelling_matrix[];
extern int far my_2d_viewing_matrix[];
extern int far my_3d_viewing_matrix[];




struct a_word
{
	unsigned int most_sig,least_sig;
};


extern struct a_word far vm512;
extern struct a_word far vm345;
extern struct a_word far vm33536;
extern struct a_word far vm49536;
extern struct a_word far vm1170;
extern struct a_word far vm1166;
extern struct a_word far vm69;
extern struct a_word far vm7;


extern union REGS far my_input_regs;
extern union REGS far my_output_regs;
extern char far char_save_page[25][80];
extern char far attr_save_page[25][80];
extern int far win_x_low;
extern int far win_y_low;
extern int far win_x_up;
extern int far win_y_up;
extern int far row_curser;
extern int far col_curser;
extern char far type_char_read;
extern char far value_char_read;
extern int far curser_ratio;
extern int far button_byte;
extern int far mouse_phase;
extern char far depress_flag;
extern int far mouse_x;
extern int far mouse_y;


extern int far curser_x_off;
extern char far board_string[];
extern int far char_y_off;
extern int far char_x_off;
extern int far update_level;
extern char far doing_redisplay;
extern char far save_write_error;
extern char far plot_write_error;
extern char far time_redisplay;
extern char far write_error;
extern char far print_list;
extern int far list_size;
extern char far is_a_seg_open;
extern long far seg_starts[];
extern long far num_words_in_segs;
extern char far joy_off;
extern char far pen_press2;
extern int far pen2_x;
extern int far pen2_y;
extern int far num_op_code_4s;
extern int far num_op_code_0s;
extern int far ins_matrix_sub;
extern int far ins_matrix_seg;
extern char far rw_sub_w_0_1;
extern int far rw_which_seg;
extern int far size_an_element;
extern int far redis_old_x;
extern int far redis_old_y;
extern char far change_vis_to_true;
extern char far rotate_mode;
extern char far button1;
extern char far rewrite_color;
extern int far segment_calls[];
extern int far last_sub_rewritten;
extern int far version;
extern int far check_resp;
extern int far sub_starts[];
extern int far size_header;
extern char far drive_letter;
extern int far which_changed;
extern int far where_called[NUMBER_OF_SUBS][20];
extern char far quit_segment;
extern char far draw_black;
extern int far num_pages;
extern int far my_y_max;
extern char far ins_matrix_flag;
extern int far which_redis;
extern char far quit_redis;
extern int far text_col;
extern int far text_row;
extern int far graphics_text;
extern char far locate_called;
extern char far locate_1st_draw;
extern int far locate_seg;
extern int far save_joy_x;
extern int far save_joy_y;
extern char far change_sub3_6_7;
extern int far save_line_type;
extern int far save_my_x;
extern int far save_my_y;
extern int far my_sub6_ptr;
extern char far curser_displayed;	
extern int far which_page;
extern char far clear_seg_flag;
extern char far num_dims;
extern int far new_x;
extern int far new_y;
extern char far diag_flag;
extern char far read_joystick;
extern char far pen_press;
extern int far my_x;
extern int far my_y;
extern int far my_z;
extern int far old_x;
extern int far old_y;
extern char far joy_enable;
extern char far rw_op_code;
extern int far sub1_0_seg;
extern char far black;
extern char far rw_sub1_0_flag;
extern long far ptr_rw_sub1_0;
char far save_color;
extern int far save_sub_insert_ptr;
extern char far sub_rewrite;
extern char far no_draw_flag;


extern int far segment_nums[];

extern unsigned char far megatek_data[];

extern FILE far *out_ptr;
extern FILE far *rewrite_file;
extern FILE far *save_file_ptr;
extern FILE far *plot_file;
extern FILE far *display_file;
extern FILE far *copy_header_ptr;
extern FILE far *error_log_ptr;


extern char far path_error_log[];
extern char far errors_happened;


extern int far sub_nums[];

extern char far fill_sub;
extern char far our_call;


extern int far sub_insert_ptrs[];



extern int far which_cseg;
extern int far which_sub;

/*
extern float far x_y_z[];
extern float far x_y_z_prime[];
extern float far x_y[];
extern float far x_y_prime[];
*/

extern int far save_insert_ptr;
extern char far print_flag;
extern int far num_transform_segs;
extern int far num_matrixes_ins;
extern char far rewrite_flag;
extern char far draw_flag;

extern int far line_type;
extern unsigned char far command;
extern char far out_string[];
extern char far diag_string[];
extern int far xx;
extern int far yy;
extern int far zz;
extern int far x;
extern int far y;
extern int far old_segment;
extern int far new_segment;
extern int far x_origin;
extern int far y_origin;
extern int far z_origin;
extern int far a_segment;
extern unsigned char far flag;
extern unsigned char far size;
extern unsigned char far short_x;
extern unsigned char far short_y;
extern unsigned char far most_x;
extern unsigned char far most_y;
extern unsigned char far least_x;
extern unsigned char far least_y;
extern unsigned char far temp;
extern unsigned char far a_char;
extern unsigned char far arg2;
extern unsigned char far arg3;
extern unsigned int far dash_pat;
extern unsigned int far pick_label;
extern unsigned int far num_millis;
extern unsigned int far id;
extern unsigned int far switches_flag;
extern int far displacement;
extern int far a_subroutine;
extern int far old_subroutine;
extern int far new_subroutine;
extern int far horz_lower;
extern int far vert_lower;
extern int far horz_upper;
extern int far vert_upper;
extern int far dx;
extern int far dy;
extern int far pick_width;
extern int far pick_height;
extern int far diag_num;
extern int far mode_flag;
extern int far sub_level;
extern int far joy_x;
extern int far joy_y;

