/* HPGLMENU.C menus for hardcopy selections*/



#include "stdio.h"
#include "my_utils.h"

#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1
#define DOT_MATRIX 0
#define LASER 1
#define MAKE_FILE 2


extern void far create_hp_file(char);
extern int far win_y_low;
extern int far win_x_low;



FILE far *hp_output,far *pixel_ems_save_file;
char far hp_output_name[80];
char far pixel_save_file_name[80];



void far run_pp(which_set_file)
int which_set_file;
{
	restore_win();
	make_win(0,0,79,24);
	save_win();
	clear_display_page();

	goto_row_col(11,30);
	printf("Hardcopy In Progress");
	goto_row_col(12,30);
	printf("Please Wait");


	sprintf(hp_output_name,"d135hpgl.plt");
	hp_output = fopen(hp_output_name,"w");

	create_hp_file('f');   /*read from EMS pages 
				 ( 't' reads from pixel save file) */ 
	fclose(hp_output);


	clear_display_page();


	switch(which_set_file)
	{
		case DOT_MATRIX :
		{
			system("pp d135hpgl.plt dotmat.pp");
			break;
		}

		case LASER :
		{
			system("pp d135hpgl.plt laser.pp");
			break;
		}

	}


	restore_win();
	make_win(10,3,60,17);
	save_win();

}




void far just_make_file()
{
int i,ch;


	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf("      Enter Filename");
	goto_row_col(win_y_low + 3,win_x_low + 8);	


	i = 0;
	for(ch = getch() ; (ch != 13) /*enter key*/
		 && (ch != 0) ; ch = getch())
	{
		if(ch != 8)
		{
			putchar(ch);
			hp_output_name[i] = ch;
			i++;
		}
		else
		{
			if(i > 0)
			{
				i--;

				goto_row_col(win_y_low + 3,win_x_low + 8 + i);
				putchar(' ');
				goto_row_col(win_y_low + 3,win_x_low + 8 + i);
			}
		}
	}

	hp_output_name[i] = '\0';

	if(ch == 0)
		return;


	for(i = 0 ; (hp_output_name[i] != '.') &&
		 (hp_output_name[i] != '\0') ; i++)
			;


	if(hp_output_name[i] == '.')
		i++;
	else
	{
		hp_output_name[i] = '.';
		i++;
	}


	hp_output_name[i] = 'p';
	hp_output_name[i + 1] = 'l';
	hp_output_name[i + 2] = 't';
	hp_output_name[i + 3] = '\0';

	hp_output = fopen(hp_output_name,"w");

	if(hp_output != NULL)
	{
		create_hp_file('f');   /*read from EMS pages 
				 ( 't' reads from pixel save file) */ 
		fclose(hp_output);
	}
	else
		putchar(7);

}




void far hpgl_menu()
{
int ch;

	while(9)
	{
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("    Hardcopy");
		goto_row_col(win_y_low + 3,win_x_low + 1);
		printf("      F1 Dot Matrix");
		goto_row_col(win_y_low + 4,win_x_low + 1);
		printf("      F2 Laser");
		goto_row_col(win_y_low + 5,win_x_low + 1);
		printf("      F3 File");

		ch = getch();

		if(ch != 0)
			continue;

		ch = getch();

		switch(ch)
		{
			case 0x3b :  /*Dot matrix*/
			{
				run_pp(DOT_MATRIX);
				break;
			}

			case 0x3c :  /*laser*/
			{
				run_pp(LASER);
				break;
			}

			case 0x3d : /*to a file*/
			{
				just_make_file();
				break;
			}

			case 75 :
			case 77 :
			case 80 :
				return;
		}
	}
}

