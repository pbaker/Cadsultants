#include "stdio.h"
#include "memory.h"
#include "c:\emulate\common\display.h"
#include "expanded.h"


#define TRUE  't'
#define FALSE  'f'


#define END_OF_PAGE 79
#define END_OF_DL  75


#define X_CENTER 0
#define Y_CENTER 0

#define A_SIZE_SCALE 2.533


extern char far *ems_read_pointer;
extern int far ems_read_page;

extern int far current_logical_page;


extern FILE far *hp_plotter;
extern int far plot_x_offset;
extern int far plot_y_offset;
extern float far plot_scale_factor;


/*error logger stuff*/
extern void far error_logger();
extern char far diag_string[];


extern struct a_word far read_element;
extern void far get_word();



struct a_word far read_plot_value;
char far read_display_end = FALSE;
char far current_dash_pattern = 0;
int far last_plot_x;
int far last_plot_y;
char far plot_file_string[100];
char far plot_file_write_error = FALSE;





void far insert_plot_file()
{
	if(plot_file_write_error == FALSE)
	{
		fprintf(hp_plotter,"%s",plot_file_string);

		if(ferror(hp_plotter) != 0)
		{
			plot_file_write_error = TRUE;

			sprintf(diag_string,"******ERROR***** RAN OUT OF DISK SPACE WHILE\n");
			error_logger(diag_string);

			sprintf(diag_string,"MAKING THE HP PLOTTER FILE\n");
			error_logger(diag_string);

			sprintf(diag_string,"NOTHING MORE WILL BE STORED\n");
			error_logger(diag_string);
		}
	}
}




void far read_plot_word()
{
	get_word();
	memcpy(&read_plot_value,&read_element,SIZE_A_WORD);
}





void far plot_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	x1 = (int)((x1 - X_CENTER)*plot_scale_factor) + X_CENTER + plot_x_offset;
	y1 = (int)((y1 - Y_CENTER)*plot_scale_factor) + Y_CENTER + plot_y_offset;

	x2 = (int)((x2 - X_CENTER)*plot_scale_factor) + X_CENTER + plot_x_offset;
	y2 = (int)((y2 - Y_CENTER)*plot_scale_factor) + Y_CENTER + plot_y_offset;

	if((x1 == last_plot_x) && (y1 == last_plot_y))
	{
		sprintf(plot_file_string,"PA %d, %d;\n",x2,y2);
		insert_plot_file();
	}
	else
	{
		sprintf(plot_file_string,"PU;\nPA %d, %d;\nPD;\nPA %d, %d;\n",
			x1,y1,x2,y2);
		insert_plot_file();
	}

	last_plot_x = x2;
	last_plot_y = y2;
}






/*

void far plot_move(x,y)
int x,y;
{
	x = (int)((x - X_CENTER)*plot_scale_factor) + X_CENTER + plot_x_offset;
	y = (int)((y - Y_CENTER)*plot_scale_factor) + Y_CENTER + plot_y_offset;
	
	fprintf(hp_plotter,"PU;\nPA %d, %d\n",x,y);
}
*/








void far make_plot_file()
{
int row,col,i,j;


	last_plot_x = last_plot_y = 32000;

	sprintf(plot_file_string,"DF;\nPA0,0;\nLT;\nPU;\nSP2;\n");
	insert_plot_file();

	current_dash_pattern = 0;

/*scale up like Paul does*/
	plot_scale_factor *= A_SIZE_SCALE;


	ems_read_page = 0;
	ems_read_pointer = (char far *)start_of_page_frame_0;
	map_handle_page(0,0,handle);
	

	for(read_plot_word() ; read_plot_value.op_code != END_OF_DL ;
		read_plot_word())
	{
		switch(read_plot_value.op_code)
		{
			case 1 : /*plot a line*/
			{
				if(read_plot_value.font !=
					current_dash_pattern)
				{
					current_dash_pattern =
						read_plot_value.
							font;

					if(read_plot_value.font
						== 0)
					{
					    /*solid*/
						sprintf(
						    plot_file_string,
							"LT;\n");

						insert_plot_file();
					}
					else if(read_plot_value
						.font == 1)
					{
					    /*dashed*/
						sprintf(
						    plot_file_string,
							"LT2;\n");

						insert_plot_file();
					}
					else if(read_plot_value
						.font == 2)
					{
					    /*centerline*/
						sprintf(
						    plot_file_string,
							"LT5;\n");

						insert_plot_file();
					}
					else if(read_plot_value
						.font == 4)
					{
					    /*phantom*/
						sprintf(
						    plot_file_string,
							"LT6;\n");

						insert_plot_file();
					}
				}

				plot_line(read_plot_value.arg1,
					read_plot_value.arg2,
					read_plot_value.arg3,
					read_plot_value.arg4);

				break;
			}

			case 4 : /*plot a character*/
			{
	
				break;   /*may not work right*/


				sprintf(plot_file_string,
					"PU;\nSM%c;\nPA%d, %d;\nSM;\n",
					read_plot_value.a_char,
					read_plot_value.arg1,
					read_plot_value.arg2);

				insert_plot_file();

				last_plot_x = read_plot_value.
					arg1;
				last_plot_y = read_plot_value.
					arg2;

				break;
			}

		}
	}


	sprintf(plot_file_string,"PU;\nSP0;\nSP;\n");
	insert_plot_file();

	
	map_handle_page(current_logical_page,0,handle);
}
