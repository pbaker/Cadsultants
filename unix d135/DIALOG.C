#include "my_globs.h"
#include "my_utils.h"
#include <pw_lib.h>





#define W_BORDER_INDENT  9
#define V_BORDER_INDENT  4

#define CHAR_WIDTH  50
#define CHAR_HEIGHT  60

#define W_LEFT  8000
#define W_RIGHT  9900
#define W_TOP  2048
#define W_BOTTOM  -1000

#define V_LEFT  0
#define V_RIGHT 255
#define V_TOP  767
#define V_BOTTOM 0

#define FORE_COLOR  0x44
#define BACK_COLOR  0

#define CHAR_X_SHIFT  3*CHAR_WIDTH
#define CHAR_Y_SHIFT  2*CHAR_HEIGHT

extern void  error_logger();
extern void  restore_graphics_viewport();
extern void  set_message_viewport();
int  dialog_recursion = 0;



void show_mm_entry()
{
#ifdef HAVE_DIAGS
	my_diag("\nICU ENTRY to message monitor\n");
#endif
}




void  dialog_string(the_string,dialog_id)
char  *the_string;
int dialog_id;
{
int row,col;
int i;


	dialog_recursion++;
	if(dialog_recursion > 5)
	{
		dialog_recursion--;
		return;
	}



	set_message_viewport();


	switch(dialog_id)
	{
		case -1 :
		case 1 :  /* ERRORS */
		{
			row = 46;
			col = 5;
			break;
		}
		

		case -2 :
		case 2 :  /* Screen Update */
		{
			row = 46;
			col = 22;
			break;
		}


		case -3 :
		case 3 :  /*TIMER*/
		{
			row = 47;
			col = 5;
			break;
		}


		case -4 :
		case 4 :  /* reason */
		{
			row = 47;
			col = 22;
			break;
		}

		case -5 :
		case 5 : /*com errors*/
		{
			row = 48;
			col = 5;
			break;
		}



		default :
		{
			row = 49;
			col = 5;
		}

	}



	if(dialog_id < 0)
	{
	/* clear dialog message */
		color(BACK_COLOR);

		for(i = 0 ; i < 15 ; i++)
		{
			move2((col + i)*CHAR_WIDTH + W_LEFT - 10,
				W_TOP - row*CHAR_HEIGHT - 10);
			boxr(CHAR_WIDTH,CHAR_HEIGHT);
		}

		color(FORE_COLOR);
	}
	else
	{
	/* display dialog message */
		color(FORE_COLOR);
		move2(col*CHAR_WIDTH + W_LEFT,W_TOP - row*CHAR_HEIGHT);
		gtext(the_string);
	}


	restore_graphics_viewport();

	dialog_recursion--;
}
