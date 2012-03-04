#include "stdio.h"
#include "display.h"

#define TRUE 1
#define FALSE 0

extern FILE far *display_file;
extern FILE far *hp_plotter;
extern int far plot_x_offset;
extern int far plot_y_offset;
extern float far plot_scale_factor;

/*error logger stuff*/
extern void far error_logger();
extern char far diag_string[];


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
	if(fread(&read_plot_value,SIZE_A_WORD,1,display_file) == 0)
		read_display_end = TRUE;
}




void far plot_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	x1 = (int)((x1 - 2048)*plot_scale_factor) + 2048 + plot_x_offset;
	y1 = (int)((y1 - 1536)*plot_scale_factor) + 1536 + plot_y_offset;

	x2 = (int)((x2 - 2048)*plot_scale_factor) + 2048 + plot_x_offset;
	y2 = (int)((y2 - 1536)*plot_scale_factor) + 1536 + plot_y_offset;

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
	x = (int)((x - 2048)*plot_scale_factor) + 2048 + plot_x_offset;
	y = (int)((y - 1536)*plot_scale_factor) + 1536 + plot_y_offset;
	
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
	plot_scale_factor *= 3.628571429;



	fseek(display_file,0L,SEEK_SET);
	read_display_end = FALSE;

	while(read_display_end == FALSE)
	{
		read_plot_word();
		if(read_display_end == TRUE)
			break;

		switch(read_plot_value.op_code)
		{
			case 1 : /*plot a line*/
			{
				if(read_plot_value.font !=
					current_dash_pattern)
				{
					current_dash_pattern = read_plot_value.
						font;

					if(read_plot_value.font == 0)
					{
					/*solid*/
						sprintf(plot_file_string,
							"LT;\n");

						insert_plot_file();
					}
					else if(read_plot_value.font == 1)
					{
					/*dashed*/
						sprintf(plot_file_string,
							"LT2;\n");

						insert_plot_file();
					}
					else if(read_plot_value.font == 2)
					{
					/*centerline*/
						sprintf(plot_file_string,
							"LT5;\n");

						insert_plot_file();
					}
					else if(read_plot_value.font == 4)
					{
					/*phantom*/
						sprintf(plot_file_string,
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
				sprintf(plot_file_string,
					"PU;\nSM%c;\nPA%d, %d;\nSM;\n",
					read_plot_value.a_char,
					read_plot_value.arg1,
					read_plot_value.arg2);

				insert_plot_file();

				last_plot_x = read_plot_value.arg1;
				last_plot_y = read_plot_value.arg2;

				break;
			}

		}
	
	}


	fseek(display_file,0L,SEEK_END);
	
}
