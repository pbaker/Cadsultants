#include "stdio.h"
#include "stdlib.h"



#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1


#define DOT_MATRIX 0
#define LASER 1
#define MAKE_FILE 2


extern void far clear_win(int);
extern void far make_win(int,int,int,int);
extern void far goto_row_col(int,int);
extern void far draw_box();
extern void far save_win(void);
extern void far restore_win(void);
extern void far clear_display_page();



extern int far hp_x_offset;
extern int far hp_y_offset;
extern float far hp_scale;


extern void far create_hp_file(char);
extern int far win_y_low;
extern int far win_x_low;



FILE far *hp_output,far *pixel_ems_save_file;


char far hp_output_name[80];
char far pixel_save_file_name[80];
int far last_device = 0x3d;
char far parameter_value[30];



struct offset_values
{
	int x_offset,y_offset;
};



struct offset_values offset_table[3][5] = 
{
/* A */	 {
		{0,0},{0,0},{-4809,-4013},{-4741,-4013},{-4809,-4013}
	 },


/* B */  {
		{0,0},{0,0},{-7857,-5283},{-7513,-5283},{-7722,-5283}
	 },


/* C */  {
		{0,0},{0,0},{-10431,-8128},{-9889,-8128},{-10228,-8128}
	 }
};


float far paper_scale[3][5] = 
{
   /* A */   {2.533,2.533,2.348,2.314,2.348},
   /* B */   {3.335,3.335,3.335,3.335,3.335},
   /* C */   {5.131,5.131,5.093,4.828,4.994}
};



int far size_of_paper = 0;
int far type_of_plotter = 0;
int far type_of_7580b = 0;


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






void far enter_device()
{
int ch;

	while(999)
	{
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("   Output Device");
		goto_row_col(win_y_low + 3,win_x_low + 1);
		printf("     F1 Laser");
		goto_row_col(win_y_low + 4,win_x_low + 1);
		printf("     F2 Dot Matrix");
		goto_row_col(win_y_low + 5,win_x_low + 1);
		printf("     F3 File");
		goto_row_col(win_y_low + 6,win_x_low + 1);
		printf("     F4 Plotter  (LPT1)");


		goto_row_col(win_y_low + 3 + last_device - 0x3b,
			win_x_low + 8);
		putchar('>');


		ch = getch();

		if(ch != 0)
			continue;

		ch = getch();

		switch(ch)
		{
			case 0x3b : /*laser*/
			case 0x3c : /*dot matrix*/
			case 0x3d : /*file*/
			case 0x3e : /*plotter*/
			{
				last_device = ch;
				break;
			}


			case 75 :
			case 77 :
			case 80 :
				return;
		}
	}
}






void far paper_size()
{
int ch;


	while(888)
	{
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("  ANSI Paper Size");
		goto_row_col(win_y_low + 3,win_x_low + 1);
		printf("    F1  A");
		goto_row_col(win_y_low + 4,win_x_low + 1);
		printf("    F2  B");
		goto_row_col(win_y_low + 5,win_x_low + 1);
		printf("    F3  C");

		goto_row_col(win_y_low + 3 + size_of_paper,win_x_low + 7);
		putchar('>');


		ch = getch();

		if(ch != 0)
			continue;

		ch = getch();

		switch(ch)
		{
			case 0x3b :
			case 0x3c :
			case 0x3d :
			{
				size_of_paper = ch - 0x3b;

				hp_scale = paper_scale[size_of_paper]
					[type_of_plotter];
				hp_x_offset = offset_table[size_of_paper]
					[type_of_plotter].x_offset;
				hp_y_offset = offset_table[size_of_paper]
					[type_of_plotter].y_offset;

/*
fprintf(stdprn,"Size of paper = %d , hp_scale = %f\n",size_of_paper,
	hp_scale);
fflush(stdprn);
*/

				break;
			}

			case 75 :
			case 77 :
			case 80 :
				return;
		}
	}
}





void far mode_of_7580b()
{
int ch;


	while(8257)
	{
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("   7580B");
		goto_row_col(win_y_low + 3,win_x_low + 1);
		printf("     F1 Special");
		goto_row_col(win_y_low + 4,win_x_low + 1);
		printf("     F2 Normal");
		goto_row_col(win_y_low + 5,win_x_low + 1);
		printf("     F3 Expanded");

		goto_row_col(win_y_low + 3 + type_of_7580b,win_x_low + 8);
		putchar('>');

		ch = getch();

		if(ch != 0)
			continue;

		ch = getch();


		switch(ch)
		{
			case 0x3b :
			case 0x3c :
			case 0x3d :
			{
				type_of_7580b = ch - 0x3b;
				type_of_plotter = ch - 0x3b + 2;

				hp_scale = paper_scale[size_of_paper]
					[type_of_plotter];
				hp_x_offset = offset_table[size_of_paper]
					[type_of_plotter].x_offset;
				hp_y_offset = offset_table[size_of_paper]
					[type_of_plotter].y_offset;


/*
fprintf(stdprn,"7580B : X offset = %d , Y offset = %d\n",hp_x_offset,
	hp_y_offset);
fprintf(stdprn,"scale = %f\n\n",hp_scale);
fflush(stdprn);
*/

				break;
			}


			case 75 :
			case 77 :
			case 80 :
				return;
		}
	}

}



void far plotter_type()
{
int ch;


	while(736)
	{
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("   Emulation");
		goto_row_col(win_y_low + 3,win_x_low + 1);
		printf("     F1 7475A");
		goto_row_col(win_y_low + 4,win_x_low + 1);
		printf("     F2 7550A");
		goto_row_col(win_y_low + 5,win_x_low + 1);
		printf("     F3 7580B");


		if(type_of_plotter < 2)
			goto_row_col(win_y_low + 3 + type_of_plotter,
				win_x_low + 8);
		else
			goto_row_col(win_y_low + 5,win_x_low + 8);


		putchar('>');


		ch = getch();

		if(ch != 0)
			continue;

		ch = getch();

		switch(ch)
		{
			case 0x3b : /* 7475A */
			case 0x3c : /* 7550A */
			{
				type_of_plotter = ch - 0x3b;

				hp_scale = paper_scale[size_of_paper]
					[type_of_plotter];
				hp_x_offset = offset_table[size_of_paper]
					[type_of_plotter].x_offset;
				hp_y_offset = offset_table[size_of_paper]
					[type_of_plotter].y_offset;


/*
fprintf(stdprn,"OTHER 2 plotters : X offset = %d , Y offset = %d\n",hp_x_offset,
	hp_y_offset);
fprintf(stdprn,"scale = %f\n\n",hp_scale);
fflush(stdprn);
*/

				break;
			}

			case 0x3d : /* 7580B */
			{
				type_of_plotter = 2;
				mode_of_7580b();

				break;
			}


			case 75 :
			case 77 :
			case 80 :
				return;
		}
	}

}





void far hpgl_parameters()
{
int ch;


	while(999)
	{
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("   Change Parameters");
		goto_row_col(win_y_low + 3,win_x_low + 1);
		printf("    F1 Destination");
		goto_row_col(win_y_low + 4,win_x_low + 1);
		printf("    F2 Paper Size");
		goto_row_col(win_y_low + 5,win_x_low + 1);
		printf("    F3 Emulation");


		ch = getch();

		if(ch != 0)
			continue;


		ch = getch();


		switch(ch)
		{
			case 0x3b : /*output device*/
			{
				enter_device();

				break;
			}


			case 0x3c : /*paper size*/
			{
				paper_size();

				break;
			}


			case 0x3d : /*which plotter type*/
			{
				plotter_type();

				break;
			}


			case 75 :
			case 77 :
			case 80 :
				return;
		}
	}
}




void far generate_hardcopy()
{
	switch(last_device)
	{
		case 0x3b : /*laser*/
		{
			run_pp(LASER);
			break;
		}

		case 0x3c : /*dot matrix*/
		{
			run_pp(DOT_MATRIX);
			break;
		}

		case 0x3d : /*file*/
		{
			just_make_file();
			break;
		}

		case 0x3e : /*plotter*/
		{
			sprintf(hp_output_name,"d135hpgl.plt");
			hp_output = fopen(hp_output_name,"w");

			create_hp_file('f');   /*read from EMS pages 
				 ( 't' reads from pixel save file) */ 

			fclose(hp_output);

			system("copy d135hpgl.plt  lpt1");

			break;
		}
	}
}





void far hpgl_menu()
{
int ch;


	hp_scale = paper_scale[size_of_paper][type_of_plotter];
	hp_x_offset = offset_table[size_of_paper][type_of_plotter].x_offset;
	hp_y_offset = offset_table[size_of_paper][type_of_plotter].y_offset;



	while(9)
	{
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("  Hardcopy Parameters");
		goto_row_col(win_y_low + 3,win_x_low + 1);
		printf("    F1 Generate Plot");
		goto_row_col(win_y_low + 4,win_x_low + 1);
		printf("    F2 Change Parameters");



		ch = getch();

		if(ch != 0)
			continue;

		ch = getch();

		switch(ch)
		{
			case 0x3b :  /*make the file*/
			{
				generate_hardcopy();
				break;
			}

			case 0x3c : /*enter hpgl parameters*/
			{
				hpgl_parameters();
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