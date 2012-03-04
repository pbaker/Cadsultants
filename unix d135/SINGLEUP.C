
#include "my_globs.h"
#include "my_utils.h"
#include <pw_lib.h>
#include "mywindows.h"


#define OLD_X_Y  0
#define NEW_X_Y  1




extern char  visibility_changed_to_on[];
extern char  stop_after_matrix_sub;

extern void  display_the_curser();
extern void  my_flood();
extern void  change_page_gmark_undraw();




void  single_view_update(matrix_sub)
int matrix_sub;
{
int i;
char do_a_flood;
int visi_count;
int save_row,save_col;
int save_row_curser,save_col_curser;
int temp;

	visi_count = 0;

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
	{
		if((segment_visibility[i] == 't') ||
			(visibility_changed_to_on[i] == 't'))
		{
			visi_count++;
		}
	}


	if(visi_count <= 3)
	{
		matrix_update();
		return;
	}



	if(ins_matrix_flag == 'f')
		return;




/* NO  SAVE_MAP_INFO()  Because display_a_seg Saves that stuff */

/*
	save_row = row_curser;
	save_col = col_curser;

	read_curser_pos();

	save_row_curser = row_curser;
	save_col_curser = col_curser;

	turn_off_curser();

	blinking_string("VIEW UPDATE",24,20);
*/




	do_a_flood = 't';
	ins_matrix_flag = 'f';


	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
	{
		if(segment_calls[i] == matrix_sub)
		{
			if(do_a_flood == 't')
			{
				if(curser_displayed == 't')
					display_the_curser(OLD_X_Y);

				change_page_gmark_undraw();

				stop_after_matrix_sub = 't';
				display_a_seg(i);
				stop_after_matrix_sub = 'f';

				temp = return_window_color(GRAPHICS_WINDOW,
					BACKGROUND);

				my_flood((temp << 4) | temp);

				change_page_gmark_undraw();

				stop_after_matrix_sub = 't';
				display_a_seg(i);
				stop_after_matrix_sub = 'f';

				if(curser_displayed == 't')
					display_the_curser(OLD_X_Y);

			}


			do_a_flood = 'f';

			display_a_seg(i);
		}
	}



/*
	normal_string("              ",24,20);	
	goto_row_col(save_row_curser,save_col_curser);

	turn_on_curser();

	row_curser = save_row;
	col_curser = save_col;
*/


/* NO  RESTORE_MAP_INFO()  */
}




void  call_single_update()
{
	single_view_update(ins_matrix_sub);
}


