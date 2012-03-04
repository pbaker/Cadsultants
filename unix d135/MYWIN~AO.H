
/*  MYWINDOWS.H     DECLARES ALL EXTERNS AND STRUCTURES , ETC  FOR  SINGLE 
	SCREEN  HYPERFORMANCE  D135 */




#define MAX_WINDOW_ROWS  100
#define MAX_WINDOW_COLS  80



#define MAIN_MENU        0
#define LIGHTS           1
#define CALCULATOR       2
#define PART_INFO        3
#define GRAPHICS_WINDOW  4
#define VT100_WINDOW     5
#define HOME_MENU        6


#define RESTORED_GRAPHICS 0
#define SAVED_GRAPHICS 1


#define FOREGROUND 0
#define BACKGROUND 1
#define SHADOW     3
#define BORDER     4


#define EXPANDED_POS   0
#define RETRACTED_POS  1


#ifdef ekdkdkkd

struct old_window
{
	char  *text_array[MAX_WINDOW_ROWS];
	int screen_row,screen_col;
	int num_rows,num_cols;
	int foreground,background,shadow,border;
	int text_size;
	int first_row_to_display;
	int top_row_for_insert;
	int row_offset,col_offset;
	int blt_list_start;
	char blt_status,was_updated,can_be_retracted,is_visible;
};

#endif



struct window_display
{
	int scroll_height;
	int screen_row,screen_col;
	int screen_width,screen_height;
	int foreground,background,shadow,border;
	int text_size;
	int row_offset,col_offset;
};


struct a_window
{
	char  *text_array[MAX_WINDOW_ROWS];
	int first_row_to_display;
	int top_row_for_insert;
	int blt_list_start;
	char blt_status,was_updated,can_be_retracted,is_visible,is_retracted;

	struct  window_display  full_size, retracted;
};




extern void  send_char_to_window();
extern void  clear_pop_up_window();
extern void  display_pop_up_window();
extern void  insert_char_to_window();
extern void  insert_line_to_window();


/* BLT Window functions */
extern void  save_window_blt();
extern void  restore_window_blt();
extern void  restore_graphics_if_needed();

extern int  return_window_color();

extern void  toggle_retraction();


extern char  is_window_array_empty();

