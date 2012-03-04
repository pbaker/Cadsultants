#include "c:\emulate\d135\corecode\my_globs.h"
#include "ctype.h"
#include "c:\emulate\clipper\pw_lib.h"
#include "math.h"

#define TRUE 1
#define FALSE 0

#define ZOOM_Y_OFF 0

extern int far save_system_color;


extern int far last_mouse_x;
extern int far last_neg_mouse_y;
extern int far last_mouse_y;


extern void far restore_int9();
extern void far read_config_file();
extern void far read_lock();

extern void far asterisk();
extern void far plus();
extern void far dot();
extern int far init_baud_rate;
extern void far clear_display_page();
extern char far path_error_log[];
extern void far goto_row_col(int,int);

extern void far release_version();

int far x,far y;
char far pen_press;

char far complement_on = 'f';
char far m_dash_enable = 'f';
char far m_dash_pattern = 8;
char far a_char_str[5];
int far the_color = 15;
int far pattern_num = 0;
int far is_eof = 9;
int far ch,far drive_ok;
char far test_name[100];
int far current_color = 7;

FILE far *config_file_ptr;



int far io_masks[2] = {0x0f,0xf0};

float far res_factor = 1.0;
int far my_invert_y = 896;



#define RES_1024_768 3
#define RES_1280_1024 4

char far which_high_res;
char far ems_mode;
char far have_display_list;


/*1024 x 768 version of clipper graphics calls*/




void far my_rdmask(which_page)
int which_page;
{
	rdmask(io_masks[which_page]);
}



void far my_wrmask(which_page)
int which_page;
{
	wrmask(io_masks[which_page]);
}




void far my_circle(radius,x,y)
int radius,x,y;
{
	prmfill(0);
	move2(x,y);
	circle(radius);
}






/*opens & inits pixel clipper board & reads work.cfg file*/
int far init_artist10()
{
int i;

	read_lock();
	read_config_file();

	i = pw_setup(10,6,0x330,NULL);

	if(i != 0)
	{
		printf("ERROR DURING pw_setup = %d\n",i);
		printf("CAN'T Continue \n");

		restore_int9();

		exit(1);
	}

	num_pages = 2;
	which_page = 0;

	pw_dl_init(NULL,0,NULL,0);

	mdiden2();
	mdiden3();
	vwiden3();

	mdorg2(0,0);
	mdorg3(0,0,0);

	vwrpt3(0,0,0);
	gtsize(40,40);
	rdmask(0x0f);
	wrmask(0x0f);
	linpat(0);
	window(-2048,2048,-2048,2048);


	return(0);
}



/*closes metheus board*/
void far close_board()
{

}




/*test the artist board & fill board_string*/
void far test_board()
{
	release_version();

	sprintf(board_string,"PixelWorks Clipper");
}




/*sets up a rectanglular clipping window*/
void far my_set_viewport(x1,y1,x2,y2,my_which_page) 
int x1,y1,x2,y2,my_which_page;
{
	window(x1,x2,y1,y2);

	if(which_high_res == RES_1024_768)
	{
		x1 = (int)(x1/5.33333333333) + 512;
		x2 = (int)(x2/5.33333333333) + 512;
		y1 = (int)((y1 + 2048)/5.333333333333);
		y2 = (int)((y2 + 2048)/5.333333333333);
	}
	else
	{
		x1 = (int)(x1*1.25/5.33333333333) + 640;
		x2 = (int)(x2*1.25/5.33333333333) + 640;
		y1 = (int)((y1 + 2048)/5.12*1.28);
		y2 = (int)((y2 + 2048)/5.12*1.28);
	}


	if(my_which_page == SVWPT_PAGE_1)
	{
		if(which_high_res == RES_1024_768)
			viewport(128,895,0,767);
		else
			viewport(160,1120,0,1023);
	}
	else
	{
		if(num_pages == 2)
		{
			if(my_which_page == 2)    /*"both pages"*/
			{
				if(which_high_res == RES_1024_768)
					viewport(128,895,0,767);
				else
					viewport(160,1120,0,1023);
			}
			else
				viewport(x1,x2,y1,y2);
		}
		else
			viewport(x1,x2,y1,y2);
	}
}




/*locks the drawing within the rectangular clipping window*/
void far my_lock_viewport()
{

}


/*scrolls between one page and the other*/
void far my_scroll(my_which_page)
int my_which_page;
{

}


/*selects a color register*/
void far my_color(i)
int i;
{
	color((i & 0x0f) | ((i & 0x0f) << 4));
	current_color = i & 0x0f;
}


/*changes data in a color register*/
void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	lut8(the_register & 0x0f,(red << 2) & 0xff,(green << 2) & 0xff,
		(blue << 2) & 0xff);
	lut8((the_register & 0x0f) << 4,(red << 2) & 0xff,(green << 2) & 0xff,
		(blue << 2) & 0xff);
}


/*sets up the dashed line pattern*/
void far my_dash_pattern(i)
unsigned int i;
{
	switch(i)
	{
		case 0xffff : /*solid*/
		{
			slpat32(1,0xffffffff);
			break;
		}

		case 0xeeee : /*dashed*/
		{
			slpat32(1,0xf0f0f0f0);
			break;
		}

		case 0xffb6 : /*phantom*/
		{
			slpat32(1,0xfff0f0f0);
			break;
		}

		case 0xfff6 : /*centerline*/
		{
			slpat32(1,0xfffff0f0);
			break;
		}
	}
}


/*determines whether lines are dashed or not*/
void far my_dash_enable(i)
int i;
{

	if(i == 0)
	{
		linpat(0);
	}
	else if(i == 4)
		linpat(1);
}




/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	vect3(x1,y1,0,x2,y2,0);
}


/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{
	if((the_char >= 32) && (the_char <= 127))
	{
		move2(5*x - 2048,2035 - 6*y);
		a_char_str[0] = the_char;
		a_char_str[1] = '\0';
		gtext(a_char_str);
	}
}



/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{
	if(size == 0)
	{
		gtsize(40,40);
	}
	else
		gtsize(80,80);
}





/*clears a page of the display*/
void far clear_page(my_which_page)
int my_which_page;
{
	flood(0);
}


/*clear screen in fast mode*/
void far clear_screen()
{
	flood(0);
}



/*sets drawing mode (normal or complement)*/
void far draw_mode(i)
{
	if(i == 3)
	{
		linfun(11);
	}
	else if(i == 0)
		linfun(12);
}








void far my_line3(x1,y1,z1,x2,y2,z2)
int x1,y1,z1,x2,y2,z2;
{
int row,col;
float matrix[4][4];
float my_xyz[4];
float my_xyz_prime[4];
int x_origin,y_origin,z_origin;

/*	sprintf(diag_string,"*******MY_LINE3(%d %d %d %d %d %d)\n",
		x1,y1,z1,x2,y2,z2);
	my_diag(diag_string);
*/

	if((no_draw_flag == 't') && (black == 'f'))
		return;

	if(headers_segs[which_cseg].visibility == 'f')
		return;

	if(headers_segs[which_cseg].Xform_enable == 't')
	{
		memcpy(&matrix[0][0],&hcrst[0][0],sizeof(matrix));

/*		sprintf(diag_string,"******MATRIX = %f %f %f %f\n %f %f %f %f\n",
			matrix[0][0],matrix[1][0],matrix[2][0],matrix[3][0],
			matrix[0][1],matrix[1][1],matrix[2][1],matrix[3][1]);
		my_diag(diag_string);
*/
		x_origin = headers_segs[which_cseg].x_origin;
		y_origin = headers_segs[which_cseg].y_origin;
		z_origin = headers_segs[which_cseg].z_origin;

		my_xyz[0] = (float)(x1 - x_origin);
		my_xyz[1] = (float)(y1 - y_origin);
		my_xyz[2] = (float)(z1 - z_origin);
		my_xyz[3] = 1.0;

		my_xyz_prime[0] = 0.0;
		my_xyz_prime[1] = 0.0;
		my_xyz_prime[2] = 0.0;

		for(col = 0 ; col < 2 ; col++)
			for(row = 0 ; row < 4 ; row++)
				my_xyz_prime[col] += 
					matrix[row][col] * my_xyz[row];

	
		x1 = (int)my_xyz_prime[0] + x_origin;
		y1 = (int)my_xyz_prime[1] + y_origin;

		my_xyz[0] = (float)(x2 - x_origin);
		my_xyz[1] = (float)(y2 - y_origin);
		my_xyz[2] = (float)(z2 - z_origin);
		my_xyz[3] = 1.0;	

		my_xyz_prime[0] = 0.0;
		my_xyz_prime[1] = 0.0;
		my_xyz_prime[2] = 0.0;

		for(col = 0 ; col < 2 ; col++)
			for(row = 0 ; row < 4 ; row++)
				my_xyz_prime[col] +=
					matrix[row][col] * my_xyz[row];
	
		x2 = (int)my_xyz_prime[0] + x_origin;
		y2 = (int)my_xyz_prime[1] + y_origin;	
	}


	vect3(x1,y1,0,x2,y2,0);
}








void far my_char(ch,x1,y1,size,rot)
char ch;
int x1,y1,size,rot;
{
int row,col,z1;


	z1 = my_z;

	if((no_draw_flag == 't') && (black == 'f'))
		return;

	if(headers_segs[which_cseg].visibility == 'f')
		return;

#ifdef HAVE_DIAGS
printf("MY_CHAR() ch = %x = %c , x = %d , y = %d\n",ch,ch,x1,y1);
#endif

        if(ch == '*')
        {
                x1 += 12;
                y1 += 24;
        }
	else if(ch == '+')
	{
		x1 += 12;
		y1 += 18;
	}
	else if(ch == '.')
		x1 += 16;


	if(headers_segs[which_cseg].Xform_enable == 't')
	{
#ifdef HAVE_DIAGS
printf("HCRST == %f  %f  %f  %f\n%f  %f  %f  %f\n",hcrst[0][0],hcrst[1][0],
	hcrst[2][0],hcrst[3][0],hcrst[0][1],hcrst[1][1],hcrst[2][1],hcrst[3][1]);
#endif

		x_y_z[0] = (float)(x1 - headers_segs[which_cseg].x_origin);
		x_y_z[1] = (float)(y1 - headers_segs[which_cseg].y_origin);
		x_y_z[2] = (float)(z1 - headers_segs[which_cseg].z_origin);
		x_y_z[3] = 1.0;
	
		x_y_z_prime[0] = 0.0;
		x_y_z_prime[1] = 0.0;
		x_y_z_prime[2] = 0.0;

		for(col = 0 ; col < 3 ; col++)
			for(row = 0 ; row < 4 ; row++)
				x_y_z_prime[col] += 
					hcrst[row][col] * x_y_z[row];

	
		x1 = (int)x_y_z_prime[0] + headers_segs[which_cseg].x_origin;
		y1 = (int)x_y_z_prime[1] + headers_segs[which_cseg].y_origin;
	}



        if((ch == '*') || (ch == '.') || (ch == '+'))
        {
		if(ch == '*')
                	asterisk(x1,y1);
		else if(ch == '.')
		{
			my_x += 48;
			dot(x1,y1);
		}
		else
			plus(x1,y1);


                return;
        }
	
	

	a_char_str[0] = ch;
	a_char_str[1] = 0x00;
	move2(x1,y1);
	gtext(a_char_str);
}





void far my_pan_box(x,y)
int x,y;
{
float current_hcrst_scale;


	goto_row_col(win_y_low + 1,win_x_low + 35);
	printf("X = %-5d ",new_x/4);
	goto_row_col(win_y_low + 2,win_x_low + 35);
	printf("Y = %-5d ",-new_y/4);



	if(headers_segs[4].visibility == 't')
		return;




	current_hcrst_scale = sqrt(hcrst[0][0]*hcrst[0][0] +
				   hcrst[1][0]*hcrst[1][0] +
				    hcrst[2][0]*hcrst[2][0]);


	x = (int)((x - last_mouse_x)*current_hcrst_scale);
	y = (int)((y - last_mouse_y)*current_hcrst_scale);

	draw_mode(3);
	my_color(save_system_color);

	
	my_line_xy(x - 2040,y - 2040 + ZOOM_Y_OFF,
		x - 2040,y + 2040 + ZOOM_Y_OFF);

	my_line_xy(x - 2040,y - 2040 + ZOOM_Y_OFF,
		x + 2040,y - 2040 + ZOOM_Y_OFF);

	my_line_xy(x + 2040,y - 2040 + ZOOM_Y_OFF,
		x + 2040,y + 2040 + ZOOM_Y_OFF);

	my_line_xy(x - 2040,y + 2040  + ZOOM_Y_OFF,
		x + 2040,y + 2040 + ZOOM_Y_OFF);

	my_color(current_color);
	draw_mode(0);
}




void far my_zoom_box(y)
int y;
{
float zoom_factor,last_zoom_factor;
int row,col;


	if(headers_segs[4].visibility == 't')
	{
		goto_row_col(win_y_low + 1,win_x_low + 35);
		printf("X = %-5d ",new_x/4);
		goto_row_col(win_y_low + 2,win_x_low + 35);
		printf("Y = %-5d ",-new_y/4);

		return;
	}



	draw_mode(3);
	my_color(save_system_color);



	if((-y) >= 1)  /*zoom in*/
	{
		last_zoom_factor =(9.1356*(-last_neg_mouse_y) + 2038.8644)/2048.0;

		zoom_factor = (9.1356*(-y) + 2038.8644)/2048.0/
			last_zoom_factor;

/*outer box*/
		my_line_xy( - (int)(2040.0/zoom_factor),
			 - (int)(2040.0/zoom_factor),
			(int)(2040.0/zoom_factor),
			- (int)(2040.0/zoom_factor));

		my_line_xy( - (int)(2040.0/zoom_factor),
			- (int)(2040.0/zoom_factor),
			 - (int)(2040.0/zoom_factor),
			 (int)(2040.0/zoom_factor));

		my_line_xy(- (int)(2040.0/zoom_factor),
			 (int)(2040.0/zoom_factor),
			(int)(2040.0/zoom_factor),
			(int)(2040.0/zoom_factor));

		my_line_xy((int)(2040.0/zoom_factor),
			(int)(2040.0/zoom_factor),
			(int)(2040.0/zoom_factor),
			 - (int)(2040.0/zoom_factor));



/*   inner cross   */
		my_line_xy(0,
			- (int)(2040.0/zoom_factor),
			0,
			(int)(2040.0/zoom_factor));

		my_line_xy( - (int)(2040.0/zoom_factor),
			0,
			(int)(2040.0/zoom_factor),
			0);


		my_circle((int)(111.0/zoom_factor),0,0);

	}
	else  /*zoom out*/
	{
		y = -y;

		zoom_factor = (0.91356*y + 2047.0)/2048.0;


/*box*/
		my_line_xy(- (int)(2040.0*zoom_factor),
			 - (int)(2040.0*zoom_factor),
			(int)(2040.0*zoom_factor),
			 - (int)(2040.0*zoom_factor));

		my_line_xy( - (int)(2040.0*zoom_factor),
			 - (int)(2040.0*zoom_factor),
			 - (int)(2040.0*zoom_factor),
			 (int)(2040.0*zoom_factor));

		my_line_xy(- (int)(2040.0*zoom_factor),
			 (int)(2040.0*zoom_factor),
			 (int)(2040.0*zoom_factor),
			 (int)(2040.0*zoom_factor));

		my_line_xy((int)(2040.0*zoom_factor),
			 (int)(2040.0*zoom_factor),
			 (int)(2040.0*zoom_factor),
			 - (int)(2040.0*zoom_factor));


/*outer cross*/
		my_line_xy( - (int)(2040.0*zoom_factor),
			- (int)(2040.0*zoom_factor),
			- (int)(2040.0*zoom_factor) - 10000,
			- (int)(2040.0*zoom_factor) - 10000);

		my_line_xy( - (int)(2040.0*zoom_factor),
			 (int)(2040.0*zoom_factor),
			 - (int)(2040.0*zoom_factor) - 10000,
			(int)(2040.0*zoom_factor) + 10000);

		my_line_xy((int)(2040.0*zoom_factor),
			 - (int)(2040.0*zoom_factor),
			 (int)(2040.0*zoom_factor) + 10000,
			 - (int)(2040.0*zoom_factor) - 10000);

		my_line_xy((int)(2040.0*zoom_factor),
			(int)(2040.0*zoom_factor),
			(int)(2040.0*zoom_factor) + 10000,
			(int)(2040.0*zoom_factor) + 10000);

	}    /*end else zoom out*/


	my_color(current_color);
	draw_mode(0);
}

