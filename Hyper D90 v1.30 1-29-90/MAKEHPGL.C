#include "stdio.h"
#include "c:\emulate\clipper\expanded.h"
#include "c:\emulate\clipper\pw_lib.h"

#undef TRUE
#undef FALSE

#define TRUE  't'
#define FALSE  'f'


#define END_OF_FILE 0x7000
#define DLEND_FOUND 0x1000
#define DLINT_FOUND 0x2000
#define DL_REAL_END 0x1000


#define PEN_UP  0
#define PEN_DOWN  1



extern short far *current_pointer_to_dl;
extern short far *physical_pages_pointer;
extern int far current_logical_page;
extern int far handle;
extern char far hp_output_name[];

extern void far error_logger(char *);
extern char far diag_string[];


extern FILE far *hp_output;
extern FILE far *pixel_ems_save_file;



char far hp_out_string[222];
char far hp_write_error = FALSE;


char far read_ems_save_file = FALSE;
short far ems_page_data[45];
short far hp_ems_page_offset = 0;
char far is_eof_pixel_file = FALSE;
short far op_code;

int far current_hp_logical_page;




/*scale & Xlate vars*/
int far hp_x_offset = 0;
int far hp_y_offset = 0;
float far hp_scale = 1.0;



/* hpgl output control variables */
int far current_pixel_color = 1;
int far last_pixel_x = 0, far last_pixel_y = 0;
int far current_line_style = 0;
int far pen_status = PEN_UP;





void far write_hp_file(the_string)
char far the_string[];
{
	if(hp_write_error == FALSE)
	{
		fprintf(hp_output,"%s\n",the_string);

		if(ferror(hp_output) != 0)
		{
			error_logger("Ran out of room on the drive While Creating\n");
			sprintf(diag_string,"HPGL file: %s\n",hp_output_name);
			error_logger(diag_string);
			error_logger("NOTHING More will be stored!!!\n\n");

			hp_write_error = TRUE;
		}
	}
}





/*  
  return value of 0 indicates NO errors at all 
    (other values indicate what went wrong OR the reason in a DLINT() command)
*/

#define NO_PROBLEM  0
#define RESERVED  1
#define UNUSED 2




typedef int (far *POINTER_FUNC)();



int far reserved_func()
{
	return(RESERVED);
}

int far unused_func()
{
	return(UNUSED);
}



int far noop_func();
int far call_func();
int far linpat_func();
int far color_func();
int far dlint_func();
int far vwmatx3_func();
int far window_func();
int far viewport_func();
int far move3_func();
int far draw3_func();
int far move3r_func();
int far draw3r_func();
int far gtext_func();
int far vect3_func();
int far vect2_func();
int far move2_func();
int far slpat32_func();
int far flood_func();
int far ret_func();
int far dlend_func();
int far wrmask_func();



int far point_func();  /* 8 */
int far circle_func();  /* 18 */





POINTER_FUNC interpreter_pointers[256] = 
	{
		noop_func,		/* 0 */
		reserved_func,		/* 1 */
		reserved_func,		/* 2 */
		reserved_func,		/* 3 */
		unused_func,		/* 4 */  
		unused_func,		/* 5 */
		color_func,		/* 6 */
		flood_func,		/* 7 */
		point_func,		/* 8 */
		unused_func,		/* 9 */
		reserved_func,		/* A */
		reserved_func,		/* B */
		unused_func,		/* C */
		unused_func,		/* D */
		unused_func,		/* E */
		reserved_func,		/* F */

		move2_func,		/* 10 */
		unused_func,		/* 11 */
		move3_func,		/* 12 */
		move3r_func,		/* 13 */
		reserved_func,		/* 14 */
		reserved_func,		/* 15 */
		reserved_func,		/* 16 */
		reserved_func,		/* 17 */
		circle_func,		/* 18 */
		unused_func,		/* 19 */
		reserved_func,		/* 1A */
		reserved_func,		/* 1B */
		unused_func,		/* 1C */
		unused_func,		/* 1D */
		unused_func,		/* 1E */
		unused_func,		/* 1F */

		unused_func,		/* 20 */
		unused_func,		/* 21 */
		unused_func,		/* 22 */
		unused_func,		/* 23 */
		reserved_func,		/* 24 */
		reserved_func,		/* 25 */
		reserved_func,		/* 26 */
		reserved_func,		/* 27 */
		unused_func,		/* 28 */
		unused_func,		/* 29 */
		draw3_func,		/* 2A */
		draw3r_func,		/* 2B */
		unused_func,		/* 2C */
		unused_func,		/* 2D */
		unused_func,		/* 2E */
		unused_func,		/* 2F */

		unused_func,		/* 30 */
		unused_func,		/* 31 */
		unused_func,		/* 32 */
		unused_func,		/* 33 */
		unused_func,		/* 34 */
		unused_func,		/* 35 */
		reserved_func,		/* 36 */
		reserved_func,		/* 37 */
		reserved_func,		/* 38 */
		reserved_func,		/* 39 */
		reserved_func,		/* 3A */
		reserved_func,		/* 3B */
		reserved_func,		/* 3C */
		reserved_func,		/* 3D */
		reserved_func,		/* 3E */
		reserved_func,		/* 3F */

		reserved_func,		/* 40 */
		unused_func,		/* 41 */
		unused_func,		/* 42 */
		unused_func,		/* 43 */
		unused_func,		/* 44 */
		reserved_func,		/* 45 */
		reserved_func,		/* 46 */
		reserved_func,		/* 47 */
		vect2_func,		/* 48 */
		unused_func,		/* 49 */
		vect3_func,		/* 4A */
		unused_func,		/* 4B */
		reserved_func,		/* 4C */
		reserved_func,		/* 4D */
		reserved_func,		/* 4E */
		reserved_func,		/* 4F */

		reserved_func,		/* 50 */
		reserved_func,		/* 51 */
		reserved_func,		/* 52 */
		unused_func,		/* 53 */
		unused_func,		/* 54 */
		unused_func,		/* 55 */
		reserved_func,		/* 56 */
		reserved_func,		/* 57 */
		reserved_func,		/* 58 */
		reserved_func,		/* 59 */
		unused_func,		/* 5A */
		unused_func,		/* 5B */
		slpat32_func,		/* 5C */
		unused_func,		/* 5D */
		unused_func,		/* 5E */
		unused_func,		/* 5F */

		reserved_func,		/* 60 */
		reserved_func,		/* 61 */
		reserved_func,		/* 62 */
		reserved_func,		/* 63 */
		reserved_func,		/* 64 */
		unused_func,		/* 65 */
		reserved_func,		/* 66 */
		reserved_func,		/* 67 */
		reserved_func,		/* 68 */
		unused_func,		/* 69 */
		reserved_func,		/* 6A */
		unused_func,		/* 6B */
		reserved_func,		/* 6C */
		reserved_func,		/* 6D */
		reserved_func,		/* 6E */
		reserved_func,		/* 6F */

		reserved_func,		/* 70 */
		reserved_func,		/* 71 */
		reserved_func,		/* 72 */
		unused_func,		/* 73 */
		unused_func,		/* 74 */
		dlint_func,		/* 75 */
		unused_func,		/* 76 */
		unused_func,		/* 77 */
		unused_func,		/* 78 */
		dlend_func,		/* 79 */
		unused_func,		/* 7A */
		call_func,		/* 7B */
		ret_func,		/* 7C */
		unused_func,		/* 7D */
		unused_func,		/* 7E */
		unused_func,		/* 7F */

		gtext_func,		/* 80 */
		unused_func,		/* 81 */
		unused_func,		/* 82 */
		unused_func,		/* 83 */
		unused_func,		/* 84 */
		unused_func,		/* 85 */
		unused_func,		/* 86 */
		unused_func,		/* 87 */
		reserved_func,		/* 88 */
		unused_func,		/* 89 */
		unused_func,		/* 8A */
		unused_func,		/* 8B */
		unused_func,		/* 8C */
		unused_func,		/* 8D */
		unused_func,		/* 8E */
		reserved_func,		/* 8F */

		unused_func,		/* 90 */
		unused_func,		/* 91 */
		unused_func,		/* 92 */
		unused_func,		/* 93 */
		unused_func,		/* 94 */
		unused_func,		/* 95 */
		unused_func,		/* 96 */
		unused_func,		/* 97 */
		unused_func,		/* 98 */
		unused_func,		/* 99 */
		unused_func,		/* 9A */
		unused_func,		/* 9B */
		unused_func,		/* 9C */
		unused_func,		/* 9D */
		reserved_func,		/* 9E */
		reserved_func,		/* 9F */

		unused_func,		/* A0 */
		unused_func,		/* A1 */
		reserved_func,		/* A2 */
		unused_func,		/* A3 */
		unused_func,		/* A4 */
		unused_func,		/* A5 */
		unused_func,		/* A6 */
		vwmatx3_func,		/* A7 */
		unused_func,		/* A8 */
		unused_func,		/* A9 */
		unused_func,		/* AA */
		unused_func,		/* AB */
		unused_func,		/* AC */
		unused_func,		/* AD */
		unused_func,		/* AE */
		unused_func,		/* AF */

		reserved_func,		/* B0 */
		unused_func,		/* B1 */
		viewport_func,		/* B2 */
		window_func,		/* B3 */
		unused_func,		/* B4 */
		reserved_func,		/* B5 */
		reserved_func,		/* B6 */
		reserved_func,		/* B7 */
		reserved_func,		/* B8 */
		reserved_func,		/* B9 */
		reserved_func,		/* BA */
		reserved_func,		/* BB */
		unused_func,		/* BC */
		unused_func,		/* BD */
		unused_func,		/* BE */
		unused_func,		/* BF */

		unused_func,		/* C0 */
		unused_func,		/* C1 */
		unused_func,		/* C2 */
		unused_func,		/* C3 */
		unused_func,		/* C4 */
		unused_func,		/* C5 */
		unused_func,		/* C6 */
		unused_func,		/* C7 */
		unused_func,		/* C8 */
		unused_func,		/* C9 */
		unused_func,		/* CA */
		unused_func,		/* CB */
		unused_func,		/* CC */
		unused_func,		/* CD */
		unused_func,		/* CE */
		unused_func,		/* CF */

		unused_func,		/* D0 */
		reserved_func,		/* D1 */
		unused_func,		/* D2 */
		unused_func,		/* D3 */
		unused_func,		/* D4 */
		unused_func,		/* D5 */
		unused_func,		/* D6 */
		unused_func,		/* D7 */
		unused_func,		/* D8 */
		unused_func,		/* D9 */
		unused_func,		/* DA */
		unused_func,		/* DB */
		unused_func,		/* DC */
		unused_func,		/* DD */
		unused_func,		/* DE */
		unused_func,		/* DF */

		unused_func,		/* E0 */
		unused_func,		/* E1 */
		unused_func,		/* E2 */
		unused_func,		/* E3 */
		unused_func,		/* E4 */
		unused_func,		/* E5 */
		unused_func,		/* E6 */
		unused_func,		/* E7 */
		wrmask_func,		/* E8 */
		unused_func,		/* E9 */
		linpat_func,		/* EA */
		unused_func,		/* EB */
		unused_func,		/* EC */
		reserved_func,		/* ED */
		unused_func,		/* EE */
		reserved_func,		/* EF */

		unused_func,		/* F0 */
		unused_func,		/* F1 */
		reserved_func,		/* F2 */
		reserved_func,		/* F3 */
		unused_func,		/* F4 */
		unused_func,		/* F5 */
		reserved_func,		/* F6 */
		reserved_func,		/* F7 */
		unused_func,		/* F8 */
		unused_func,		/* F9 */
		unused_func,		/* FA */
		unused_func,		/* FB */
		unused_func,		/* FC */
		reserved_func,		/* FD */
		reserved_func,		/* FE */
		unused_func		/* FF */
	};












void far read_saved_ems_page(number)
int number;
{
int i;

	if(is_eof_pixel_file == TRUE)
	{
		ems_page_data[i] = END_OF_FILE;
		return;
	}


	for(i = 0 ; i < number ; i++)
	{
		if(fread(ems_page_data + i,sizeof(short),1,
			pixel_ems_save_file) == 0)
		{
			ems_page_data[i] = END_OF_FILE;
			is_eof_pixel_file = TRUE;		
			return;
		}
	}
}




void far ems_page_read_more(number)
int number;
{
int i;

	if(read_ems_save_file == TRUE)
	{
		read_saved_ems_page(number);
		return;
	}


	for(i = 0 ; i < number ; i++)
	{
		ems_page_data[i] = *(physical_pages_pointer + 
			hp_ems_page_offset + i);
	}


	hp_ems_page_offset += number;
}








int far noop_func()
{
	return(NO_PROBLEM);
}



int far call_func()
{
	ems_page_read_more(2);

	return(UNUSED);
}



int far linpat_func()
{
	ems_page_read_more(1);


	if(current_line_style == ems_page_data[0])
		return(NO_PROBLEM);


	current_line_style = ems_page_data[0];

	if(current_pixel_color > 0)
	{
		if(current_line_style == 0)  /*solid*/
			write_hp_file("LT;");
		else if(current_line_style == 1)  /*dashed*/
			write_hp_file("LT2;");
		else if(current_line_style == 4)  /*phantom*/
			write_hp_file("LT6;");
		else if(current_line_style == 2)  /*center line*/
			write_hp_file("LT5;");
	}

	return(NO_PROBLEM);
}



int far color_func()
{
	ems_page_read_more(1);


	if(current_pixel_color == (ems_page_data[0] & 0x0f))
		return(NO_PROBLEM);


	current_pixel_color = ems_page_data[0] & 0x0f;

	if(current_pixel_color > 0)
	{
		sprintf(hp_out_string,"SP%d;",((current_pixel_color - 1) % 
			7) + 1);
		write_hp_file(hp_out_string);
	}

	return(NO_PROBLEM);
}


int far dlint_func()
{
	ems_page_read_more(1);

	return(DLINT_FOUND);
}


int far vwmatx3_func()
{
	ems_page_read_more(16);

	return(UNUSED);
}



int far window_func()
{
	ems_page_read_more(4);

	return(UNUSED);
}



int far viewport_func()
{
	ems_page_read_more(4);

	return(UNUSED);
}



int far move2_func()
{
	ems_page_read_more(2);


	if((ems_page_data[0] == last_pixel_x) &&      /*no actual movement*/
		(ems_page_data[1] == last_pixel_y))
	{
		return(NO_PROBLEM);
	}
	


	last_pixel_x = ems_page_data[0];
	last_pixel_y = ems_page_data[1];


	if(current_pixel_color > 0)
	{
		if(pen_status == PEN_DOWN)
		{
			sprintf(hp_out_string,"PU; PA%d,%d;",
				(int)(ems_page_data[0]*hp_scale) + hp_x_offset,
				(int)(ems_page_data[1]*hp_scale) + hp_y_offset);
			write_hp_file(hp_out_string);

			pen_status = PEN_UP;
		}
		else
		{
			sprintf(hp_out_string,"PA%d,%d;",
				(int)(ems_page_data[0]*hp_scale) + hp_x_offset,
				(int)(ems_page_data[1]*hp_scale) + hp_y_offset);
			write_hp_file(hp_out_string);
		}
	}


	return(NO_PROBLEM);
}




int far move3_func()
{
	ems_page_read_more(3);

	return(UNUSED);
}


int far move3r_func()
{
	ems_page_read_more(3);

	return(UNUSED);
}



int far draw3_func()
{
	ems_page_read_more(3);

	return(UNUSED);
}



int far draw3r_func()
{
	ems_page_read_more(3);


	return(UNUSED);
}



int far gtext_func()
{
int i;

	/* read until you see the end marker */

	ems_page_read_more(1);
	i = 0;

	while(((ems_page_data[0] & 0xff) != 0) && 
		((ems_page_data[0] & 0xff00) != 0))
	{
		i++;
		ems_page_read_more(1);
	}

	return(NO_PROBLEM);
}




int far vect2_func()
{
	ems_page_read_more(4);


/*
fprintf(stdprn,"\nVECT2(%d,%d,%d,%d)\n",ems_page_data[0],ems_page_data[1],
	ems_page_data[2],ems_page_data[3]);
fprintf(stdprn,"hp scale = %f  , X offset = %d  , Y offset = %d\n",
		hp_scale,hp_x_offset,hp_y_offset);
fflush(stdprn);
*/


	if(current_pixel_color == 0)   /* ems color is BLACK */
	{
		pen_status = PEN_DOWN;
		last_pixel_x = ems_page_data[2];
		last_pixel_y = ems_page_data[3];
		return(NO_PROBLEM);
	}
		


	if((last_pixel_x == ems_page_data[0]) &&    /*vect2 start is the same*/
		(last_pixel_y == ems_page_data[1]))  /*as last pixel position*/
	{
		if((ems_page_data[0] == ems_page_data[2]) && 
			(ems_page_data[1] == ems_page_data[3]))
		{
			if(pen_status == PEN_UP)
			{
				write_hp_file("PD;");
				pen_status = PEN_DOWN;
			}
		}
		else
		{
			if(pen_status == PEN_UP)
			{
				pen_status = PEN_DOWN;
				write_hp_file("PD;");
			}

			sprintf(hp_out_string,"PA%d,%d;",
				(int)(ems_page_data[2]*hp_scale) + hp_x_offset,
				(int)(ems_page_data[3]*hp_scale) + hp_y_offset);
			write_hp_file(hp_out_string);


			last_pixel_x = ems_page_data[2];
			last_pixel_y = ems_page_data[3];
		}
	}
	else   /*vect2 start is different than last pixel position*/
	{
		if(pen_status == PEN_DOWN)
			write_hp_file("PU;");

	/* the move part */
		sprintf(hp_out_string,"PA%d,%d;",
			(int)(ems_page_data[0]*hp_scale) + hp_x_offset,
			(int)(ems_page_data[1]*hp_scale) + hp_y_offset);
		write_hp_file(hp_out_string);


		write_hp_file("PD;");
	/* the draw part */
		sprintf(hp_out_string,"PA%d,%d;",
			(int)(ems_page_data[2]*hp_scale) + hp_x_offset,
			(int)(ems_page_data[3]*hp_scale) + hp_y_offset);
		write_hp_file(hp_out_string);


		last_pixel_x = ems_page_data[2];
		last_pixel_y = ems_page_data[3];
		pen_status = PEN_DOWN;
	}


	return(NO_PROBLEM);
}




int far vect3_func()
{
	ems_page_read_more(6);

	return(UNUSED);
}



int far slpat32_func()
{
	ems_page_read_more(3);

	return(UNUSED);
}



int far flood_func()
{
	ems_page_read_more(1);

	return(NO_PROBLEM);
}



int far ret_func()
{

	return(UNUSED);
}



int far dlend_func()
{
	return(DLEND_FOUND);
}



int far wrmask_func()
{
	ems_page_read_more(1);

	return(NO_PROBLEM);
}





int far point_func()  /* 8 */
{

	return(NO_PROBLEM);
}



int far circle_func()  /* 18 */
{
	ems_page_read_more(1);

	return(NO_PROBLEM);
}








int far pixel_parse(the_op_code)
int the_op_code;
{
int result;


	if((the_op_code < 0) || (the_op_code > 255))
	{
	/*handle invalid op code*/
		error_logger("making HPGL file: INVALID op code SEEN: QUITTING OUT\n"); 

		return(FALSE);
	}


	hp_ems_page_offset++;
	result = (*interpreter_pointers[the_op_code])();


	if(result == DLINT_FOUND)
		return(FALSE);
	else if(result == DLEND_FOUND)  /*Dlend_func return value*/
	{

		if(read_ems_save_file == FALSE)
		{
			current_hp_logical_page++;
			if(current_hp_logical_page > current_logical_page)
				return(FALSE);


			map_handle_page(current_hp_logical_page,0,handle);
			hp_ems_page_offset = 0;

			return(TRUE);
		}
		else  /*reading a pixel save file*/
		{
			switch(result & 0xf000)
			{
				case END_OF_FILE : 
				{
					return(FALSE);
				}
			}
		}
	}
	else   /*NOT A DLEND (check for result code) */
	{
		switch(result)
		{
			case RESERVED :
			{
				sprintf(diag_string,
					"making HPGL file Hit a Reserved op code %x\n",
						the_op_code);
				error_logger(diag_string);

				return(FALSE);
			}

			case UNUSED :
			{
				sprintf(diag_string,"making HPGL file Hit an Unused op code %x\n",
					the_op_code);
				error_logger(diag_string);

				return(FALSE);
			}

			case NO_PROBLEM :
				return(TRUE);


			default :
			{
				error_logger("making HPGL file Unknown Result\n");
				return(FALSE);
			}

		}   /*end switch() */

	}   /*end else for result code checking*/

}   /*end of pixel_parse()*/




void far create_hp_file(read_a_file)
char read_a_file;
{
	hp_write_error = FALSE;
	read_ems_save_file = read_a_file;
	is_eof_pixel_file = FALSE;
	current_line_style = 0;
	current_hp_logical_page = 0;
	current_pixel_color = 1;
	pen_status = PEN_UP;
	write_hp_file("PU;");


	if(read_a_file == TRUE)
	{
		while(fread(&op_code,sizeof(short),1,pixel_ems_save_file) > 0)
		{
			if(is_eof_pixel_file == TRUE)
				break;

			pixel_parse(op_code);
		}


		fclose(pixel_ems_save_file);
		fclose(hp_output);
	}
	else
	{
		pw_dl_init(current_pointer_to_dl,0,NULL,0);
		dlint(DL_REAL_END);
		pw_dl_init(NULL,0,NULL,0);


		current_hp_logical_page = 0;
		map_handle_page(current_hp_logical_page,0,handle);
		hp_ems_page_offset = 0;
		

		while(pixel_parse(*(physical_pages_pointer + 
			hp_ems_page_offset)) == TRUE)
				;


		write_hp_file("PU;\nSP0;\nSP;");

		map_handle_page(current_logical_page,0,handle);
	}
}

