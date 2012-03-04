/* MAKEHPGL.C   generates the HPGL file */


#include "my_globs.h"
#include "expanded.h"



/* 
#define I_HAVE_DIAGS
*/




#define TRUE  't'
#define FALSE  'f'
#define END_OF_FILE 0x7000

/*
#define INTERP_A_SUB_TIME 0x1000
#define XFORM_ENABLE_INTERRUPT 0x2000
#define XFORM_DISABLE_INTERRUPT 0x3000
#define INSERT_2D_XLATE_INTERRUPT 0x4000
#define DL_BRANCH_INTERRUPT 0x5000
#define END_OF_SEGMENT_INTERRUPT 0x6000
#define MATRIX_SUB_INTERRUPT 0x7000
*/

#define ABS_X_OFFSET 2048
#define ABS_Y_OFFSET 2048
#define D135_CORRECTION  1.333333333


#define PEN_UP  0
#define PEN_DOWN  1




extern int  segment_nums[];
extern char  use_hardcopy_colors;


extern int  hp_x_offset;
extern int  hp_y_offset;
extern float  hp_scale;




extern short  *physical_pages_pointer;
extern int  current_logical_page;
extern int  handle;
extern char  hp_output_name[];
extern void  error_logger();
extern char  diag_string[];
extern FILE  *hp_output;
extern FILE  *pixel_ems_save_file;



/*
struct ems_position
{
	int logical_page_num,offset;
};
*/


extern struct ems_position  start_segment_ptrs[];



int  hpgl_x,  hpgl_y;
char  hp_out_string[222];
char  hp_write_error = FALSE;
char  read_ems_save_file = FALSE;
short  ems_page_data[45];
short  hp_ems_page_offset = 0;
char  is_eof_pixel_file = FALSE;
short  op_code;
unsigned long  current_line_style = 0xffffffffL;
int  current_hp_logical_page;

int  current_hardcopy_color = 1;
char  pen_up_down = PEN_UP;





void  write_hp_file(the_string)
char  the_string[];
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


typedef int (*POINTER_FUNC)();



int  reserved_func()
{
	return(RESERVED);
}

int  unused_func()
{
	return(UNUSED);
}



int  noop_func();
int  call_func();
int  linpat_func();
int  color_func();
int  dlint_func();
int  vwmatx3_func();
int  window_func();
int  viewport_func();
int  move3_func();
int  draw3_func();
int  move3r_func();
int  draw3r_func();
int  gtext_func();
int  vect3_func();
int  vect2_func();
int  move2_func();
int  slpat32_func();
int  flood_func();
int  ret_func();
int  dlend_func();
int  wrmask_func();



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
		unused_func,		/* 8 */
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
		unused_func,		/* 18 */
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





void  read_saved_ems_page(number)
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




void  ems_page_read_more(number)
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








int  noop_func()
{
#ifdef I_HAVE_DIAGS
	printf("noop_func()\n");
#endif

	return(NO_PROBLEM);
}



int  call_func()
{
int i;
unsigned long physical_address,color_sub_lookup;
unsigned int high_part,low_part;
short  *color_table_address;
int color_temp;



#ifdef I_HAVE_DIAGS
	printf("call_func()\n");
#endif


	ems_page_read_more(2);


	high_part = ems_page_data[1];
	low_part = ems_page_data[0];

	physical_address = ((unsigned long)high_part << 16) + 
		(unsigned long)low_part;



	if(use_hardcopy_colors == 't')
	{
		for(i = 0 ; i < 16 ; i++)
		{
			color_table_address = &(sub_struct_ptr ->
				color_sub_table[i][0]);

			color_sub_lookup = (unsigned long)
			    color_table_address;


			if(color_sub_lookup == physical_address)
			{
				color_temp = current_hardcopy_color;
				current_hardcopy_color = i % 7 + 1;

				if(color_temp != current_hardcopy_color)
				{
					sprintf(hp_out_string,"SP%d;",
						current_hardcopy_color);
					write_hp_file(hp_out_string);
				}


				return(NO_PROBLEM);
			}
		}
	}



	/*need to know sub 0  from sub 1 & 2  (maybe sub 4 for the grids?) */

	
	if(current_line_style != 0xffffffffL)
	{
		write_hp_file("LT;");
		current_line_style = 0xffffffffL;
	}

	


	return(NO_PROBLEM);
}




int  linpat_func()
{
#ifdef I_HAVE_DIAGS
	printf("linpat_func()\n");
#endif


	ems_page_read_more(1);


	if(ems_page_data[0] == 0)
	{
		write_hp_file("LT;");
	}
	else
	{
		if(current_line_style == 0xffffffffL)  /*solid*/
			write_hp_file("LT;");
		else if(current_line_style == 0xf0f0f0f0L)  /*dashed*/
			write_hp_file("LT2;");
		else if(current_line_style == 0xfff0f0f0L)  /*phantom*/
			write_hp_file("LT6;");
		else if(current_line_style == 0xfffff0f0L)  /*center line*/
			write_hp_file("LT5;");
	}

	return(NO_PROBLEM);
}



int  color_func()
{
int color_temp;


#ifdef I_HAVE_DIAGS
	printf("color_func()\n");
#endif


	ems_page_read_more(1);


	if(use_hardcopy_colors == 't')
	{
		color_temp = current_hardcopy_color;
		current_hardcopy_color = ((ems_page_data[0] & 0xf) % 7) + 1;

		if(color_temp != current_hardcopy_color)
		{
			sprintf(hp_out_string,"SP%d;",current_hardcopy_color);
			write_hp_file(hp_out_string);
		}
	}


	return(NO_PROBLEM);
}



int  dlint_func()
{
	ems_page_read_more(1);

#ifdef I_HAVE_DIAGS
	printf("dlint_func(%d)\n",ems_page_data[0]);
#endif


	return(ems_page_data[0]);
}


int  vwmatx3_func()
{
#ifdef I_HAVE_DIAGS
	printf("vwmatx3_func()\n");
#endif



	ems_page_read_more(16);

	return(NO_PROBLEM);
}



int  window_func()
{
#ifdef I_HAVE_DIAGS
	printf("window_func()\n");
#endif



	ems_page_read_more(4);

	return(NO_PROBLEM);
}



int  viewport_func()
{
#ifdef I_HAVE_DIAGS
	printf("viewport_func()\n");
#endif



	ems_page_read_more(4);

	return(NO_PROBLEM);
}





int  move2_func()
{
#ifdef I_HAVE_DIAGS
	printf("move2_func()\n");
#endif

	ems_page_read_more(2);

	if(pen_up_down == PEN_DOWN)
	{
		pen_up_down = PEN_UP;
		write_hp_file("PU;");
	}
	

	sprintf(hp_out_string,"PA%d,%d;",
		(int)((ems_page_data[0]/2 + ABS_X_OFFSET)*hp_scale) + 
			hp_x_offset,
		(int)((ems_page_data[1]/2 + ABS_Y_OFFSET)*hp_scale) +
			hp_y_offset);

	hpgl_x = ems_page_data[0];
	hpgl_y = ems_page_data[1];

	write_hp_file(hp_out_string);

	return(NO_PROBLEM);
}







int  move3_func()
{
#ifdef I_HAVE_DIAGS
	printf("move3_func()\n");
#endif


	ems_page_read_more(3);


	if(pen_up_down == PEN_DOWN)
	{
		pen_up_down = PEN_UP;
		write_hp_file("PU;");
	}


	sprintf(hp_out_string,"PA%d,%d;",
		(int)((ems_page_data[0]/2.0  + ABS_X_OFFSET)*hp_scale + 0.5) +
			hp_x_offset,
		(int)((ems_page_data[1]/2.0  + ABS_Y_OFFSET)*hp_scale + 0.5) +
			hp_y_offset);

	hpgl_x = ems_page_data[0];
	hpgl_y = ems_page_data[1];


	write_hp_file(hp_out_string);

	return(NO_PROBLEM);
}





int  move3r_func()
{
#ifdef I_HAVE_DIAGS
	printf("move3r_func()\n");
#endif


	ems_page_read_more(3);


	if(pen_up_down == PEN_DOWN)
	{
		pen_up_down = PEN_UP;
		write_hp_file("PU;");
	}


	hpgl_x += ems_page_data[0];
	hpgl_y += ems_page_data[1];

	sprintf(hp_out_string,"PA%d,%d;",
		(int)((hpgl_x/2.0 + ABS_X_OFFSET)*hp_scale + 0.5) + hp_x_offset,
		(int)((hpgl_y/2.0 + ABS_Y_OFFSET)*hp_scale + 0.5) + hp_y_offset
			);

	write_hp_file(hp_out_string);

	return(NO_PROBLEM);
}




int  draw3_func()
{
#ifdef I_HAVE_DIAGS
	printf("draw3_func()\n");
#endif


	ems_page_read_more(3);


	if(pen_up_down == PEN_UP)
	{
		pen_up_down = PEN_DOWN;
		write_hp_file("PD;");
	}



	sprintf(hp_out_string,"PA%d,%d;",
		(int)((ems_page_data[0]/2.0 + ABS_X_OFFSET)*hp_scale + 0.5) +
			hp_x_offset,
		(int)((ems_page_data[1]/2.0 + ABS_Y_OFFSET)*hp_scale + 0.5) +
			hp_y_offset);


	hpgl_x = ems_page_data[0];
	hpgl_y = ems_page_data[1];

	write_hp_file(hp_out_string);

	return(NO_PROBLEM);
}




int  draw3r_func()
{
#ifdef I_HAVE_DIAGS
	printf("draw3r_func()\n");
#endif



	ems_page_read_more(3);


	if(pen_up_down == PEN_UP)
	{
		pen_up_down = PEN_DOWN;
		write_hp_file("PD;");
	}



	hpgl_x += ems_page_data[0];
	hpgl_y += ems_page_data[1];

	sprintf(hp_out_string,"PA%d,%d;",
		(int)((hpgl_x/2.0 + ABS_X_OFFSET)*hp_scale + 0.5) + hp_x_offset,
		(int)((hpgl_y/2.0 + ABS_Y_OFFSET)*hp_scale + 0.5) + hp_y_offset
			);


	write_hp_file(hp_out_string);

	return(NO_PROBLEM);
}



int  gtext_func()
{
int i;

#ifdef I_HAVE_DIAGS
	printf("gtext_func()\n");
#endif


	/* read until you see the end marker */
	/* use label buffer HPGL commands */

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



int  vect2_func()
{
#ifdef I_HAVE_DIAGS
	printf("vect2_func()\n");
#endif


/*

 NEVER IN A D135 SEGMENT 


	ems_page_read_more(4);

	sprintf(hp_out_string,"PU; PA%d,%d; PD; PA%d,%d;",ems_page_data[0],
		ems_page_data[1],ems_page_data[2],ems_page_data[3]);
	write_hp_file(hp_out_string);

*/


	return(UNUSED);
}




int  vect3_func()
{
#ifdef I_HAVE_DIAGS
	printf("vect3_func()\n");
#endif


/*

 NEVER IN A D135 SEGMENT 


	ems_page_read_more(6);

	sprintf(hp_out_string,"PU; PA%d,%d; PD; PA%d,%d;",ems_page_data[0] ,
		ems_page_data[1] ,ems_page_data[3] ,ems_page_data[4] );
	write_hp_file(hp_out_string);
*/


	return(UNUSED);
}





int  slpat32_func()
{
unsigned int low_part,high_part;
unsigned long temp;


#ifdef I_HAVE_DIAGS
	printf("slpat32_func()\n");
#endif

	ems_page_read_more(3);


/* CASTING  SIGNED integer to UNSIGNED long  SCREWS up the sign bit, etc, etc*/
/* SO COPY them into UNSIGNED integers FIRST  WITH NO CASTING AT ALL*/
/* THEN  CAST THE UNSIGNED integers UNSIGNED longs  AND IT WORKS!!!! */

	low_part = ems_page_data[2];
	high_part = ems_page_data[1];


	temp = current_line_style;

	current_line_style = (((unsigned long)high_part) << 16) +
		(unsigned long)low_part;


	if(temp != current_line_style)
	{
		if(current_line_style == 0xffffffffL)  /*solid*/
			write_hp_file("LT;");
		else if(current_line_style == 0xf0f0f0f0L)  /*dashed*/
			write_hp_file("LT2;");
		else if(current_line_style == 0xfff0f0f0L)  /*phantom*/
			write_hp_file("LT6;");
		else if(current_line_style == 0xfffff0f0L)  /*center line*/
			write_hp_file("LT5;");
	}


	return(NO_PROBLEM);
}



int  flood_func()
{
#ifdef I_HAVE_DIAGS
	printf("flood_func()\n");
#endif



	ems_page_read_more(1);

	return(NO_PROBLEM);
}



int  ret_func()
{
#ifdef I_HAVE_DIAGS
	printf("ret_func()\n");
#endif


	return(NO_PROBLEM);
}



int  dlend_func()
{
#ifdef I_HAVE_DIAGS
	printf("dlend_func()\n");
#endif


	return(UNUSED);
}



int  wrmask_func()
{
#ifdef I_HAVE_DIAGS
	printf("wrmask_func()\n");
#endif


	ems_page_read_more(1);

	return(NO_PROBLEM);
}











int  pixel_parse(the_op_code)
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



	if(result >= 0x1000)  /*ONE OF MY dlint() REASONS*/
	{
		if(read_ems_save_file == FALSE)
		{
			switch(result & 0xf000)
			{
				case INTERP_A_SUB_TIME :
				{
					return(TRUE);
				}
	

				case DL_BRANCH_INTERRUPT :
				{
			/*switch EMS page*/
					current_hp_logical_page = result -
						DL_BRANCH_INTERRUPT;
					save_map_handle_page = 'f';
					map_handle_page(current_hp_logical_page
						,0,handle);
					hp_ems_page_offset = 0;

					return(TRUE);
				}

	
				case END_OF_SEGMENT_INTERRUPT :
					return(FALSE);


				case MATRIX_SUB_INTERRUPT :
					return(TRUE);



				case XFORM_ENABLE_INTERRUPT :
				case XFORM_DISABLE_INTERRUPT :
				case INSERT_2D_XLATE_INTERRUPT :
					return(TRUE);



				default :
				{
					sprintf(diag_string,
					    "UNKNOWN DLINT RESPONSE = %x\n",
						result);
					error_logger(diag_string);

					return(FALSE);
				}
			}   /*end switch() */
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
	else   /*NOT ONE OF MY dlint() REASONS (check for result code) */
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






void  create_hp_file(read_a_file)
char read_a_file;
{
int segment_number;

/* convert D90 HP values to the Right D135 values */

	hpgl_x = hpgl_y = 0;

	hp_scale = hp_scale/D135_CORRECTION;
	hp_x_offset = (int)(hp_x_offset/D135_CORRECTION);
	hp_y_offset = (int)(hp_y_offset/D135_CORRECTION);


	hp_write_error = FALSE;
	read_ems_save_file = read_a_file;
	is_eof_pixel_file = FALSE;

	current_line_style = 0xffffffffL;
	current_hardcopy_color = 1;
	pen_up_down = PEN_UP;

	current_hp_logical_page = 0;


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

	/* save last page before processing display lists */
		save_map_handle_page = 't';
		map_handle_page(0,0,handle);

		write_hp_file("PU;\nSP1;\nLT;");

		for(segment_number = 1 ; segment_nums[segment_number] != -1 ;
			segment_number++)
		{
			current_hp_logical_page = start_segment_ptrs[
				segment_number].logical_page_num;
			save_map_handle_page = 'f';
			map_handle_page(current_hp_logical_page,0,handle);
			hp_ems_page_offset = 0;

			while(pixel_parse(*(physical_pages_pointer + 
				hp_ems_page_offset)) == TRUE)
					;
		}

		write_hp_file("SP0;\nSP;\nPU;");

		save_map_handle_page = 'f';
		map_handle_page(current_logical_page,0,handle);
	}
}

