
#include "c:\emulate\d135\corecode\my_globs.h"
#include "ctype.h"
#include "string.h"
#include "math.h"


#define TRUE 1
#define FALSE 0

#define PEPE 0
#define PRESTO 1
#define VX1024 2


/*software clipping stuff*/
int far soft_x_lower_clip,far soft_y_lower_clip,soft_x_upper_clip,
	soft_y_upper_clip;


#define ZOOM_Y_OFF 0
extern int far last_mouse_x;
extern int far last_neg_mouse_y;
extern int far last_mouse_y;
extern int far save_system_color;


extern int far number_of_segs_visible();


extern void far read_lock();
extern void far read_config_file();


extern void far restore_int9();
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
unsigned int far m_dash_pattern = 0xffff;
char far a_char_str[5];
int far the_color = 15;
int far pattern_num = 0;
int far is_eof = 9;
int far ch,far drive_ok;
char far test_name[100];
int far current_color = 7;

short far which_vextrix_board;

short far monotronix[20] = {0x19,0x14,0x16,0x41,0x41,0x19,0x40,0x41,0xcc,
				0x0f,0,0,0,0,0,0,0,0,0,0};

int far rgb[5];



float far res_factor = 1.0;
int far my_invert_y = 896;


/*dummy variables for read_config_file() to access*/
char far which_high_res;
char far ems_mode;
char far have_display_list;


/*1024 x 1024 version of vectrix graphics calls*/



void far my_rdmask(i)
int i;
{

}

void far my_wrmask(i)
int i;
{

}




unsigned char far comp_outcode(x,y)
int x,y;
{
unsigned char temp;

	temp = 0;

	if(x > soft_x_upper_clip)
		temp |= 2;

	if(x < soft_x_lower_clip)
		temp |= 1;

	if(y > soft_y_upper_clip)
		temp |= 8;

	if(y < soft_y_lower_clip)
		temp |= 4;


	return(temp);
}


unsigned char far accept_check(outcode1,outcode2)
unsigned char outcode1,outcode2;
{
	if((outcode1 == 0) && (outcode2 == 0))
		return(TRUE);
	else
		return(FALSE);
}



unsigned char far reject_check(outcode1,outcode2)
unsigned char outcode1,outcode2;
{
	if((outcode1 & outcode2) == 0)
		return(FALSE);
	else
		return(TRUE);
}




void far clip_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
unsigned char outcode1,outcode2;
float xmax,xmin,ymax,ymin;
float float_x1,float_x2,float_y1,float_y2;
char done,accept;
unsigned char swap_outcodes;
float swap_temp;

	done = accept = FALSE;

	xmax = (float)soft_x_upper_clip;
	xmin = (float)soft_x_lower_clip;
	ymax = (float)soft_y_upper_clip;
	ymin = (float)soft_y_lower_clip;

	float_x1 = (float)x1;
	float_x2 = (float)x2;
	float_y1 = (float)y1;
	float_y2 = (float)y2;

	while(done == FALSE)
	{
		x1 = (int)float_x1;
		x2 = (int)float_x2;
		y1 = (int)float_y1;
		y2 = (int)float_y2;

		outcode1 = comp_outcode(x1,y1);
		outcode2 = comp_outcode(x2,y2);

		if(reject_check(outcode1,outcode2) == TRUE)
			return;

		accept = accept_check(outcode1,outcode2);

		if(accept == TRUE)
			break;

		if(outcode1 == 0)    /*then swap end points & outcodes*/
		{
			swap_outcodes = outcode1;
			outcode1 = outcode2;
			outcode2 = swap_outcodes;

			swap_temp = float_x1;
			float_x1 = float_x2;
			float_x2 = swap_temp;

			swap_temp = float_y1;
			float_y1 = float_y2;
			float_y2 = swap_temp;
		}

		if((outcode1 & 8) != 0)   /*divide line at top of window*/
		{
			float_x1 = float_x1 + (float_x2 - float_x1)*
				(ymax - float_y1)/(float_y2 - float_y1);
			float_y1 = ymax;
		}
		else if((outcode1 & 4) != 0)  /*divide line at bottom of window*/
		{
			float_x1 = float_x1 + (float_x2 - float_x1)*
				(ymin - float_y1)/(float_y2 - float_y1);
			float_y1 = ymin;
		}
		else if((outcode1 & 2) != 0)  /*divide line at right edge*/
		{
			float_y1 = float_y1 + (float_y2 - float_y1)*
				(xmax - float_x1)/(float_x2 - float_x1);
			float_x1 = xmax;
		}
		else if((outcode1 & 1) != 0)  /*divide line at left edge*/
		{
			float_y1 = float_y1 + (float_y2 - float_y1)*
				(xmin - float_x1)/(float_x2 - float_x1);
			float_x1 = xmin;
		}
	}


	if(accept == TRUE)
		lines((int)float_x1,(int)float_y1,(int)float_x2,(int)float_y2);
}








void far set_soft_clipping(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	soft_x_lower_clip = x1;
	soft_y_lower_clip = y1;
	soft_x_upper_clip = x2;
	soft_y_upper_clip = y2;
}



void far clear_text_buffer()
{
int row,col;

	for(row = 0 ; row < 64 ; row++)
		for(col = 0 ; col < 128 ; col++)
			pputc(row,col,' ');
}





void far my_circle(radius,x,y)
int radius,x,y;
{
	cir(x,y,radius);
}





int far init_artist10()
{
	read_lock();
	read_config_file();

	cold();
	init(&monotronix[0]);
	chr(1);
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


	sprintf(board_string,"Vectrix Pepe");
}





/*sets up a rectanglular clipping window*/
void far my_set_viewport(x1,y1,x2,y2,my_which_page) 
int x1,y1,x2,y2,my_which_page;
{
	x1 = (int)(x1/5.33333333333) + 512;
	x2 = (int)(x2/5.33333333333) + 512;
	y1 = (int)((y1 + 2048)/5.333333333333);
	y2 = (int)((y2 + 2048)/5.333333333333);



	if(my_which_page == SVWPT_PAGE_1)
	{
		set_soft_clipping(0,0,1023,1023);
	}
	else
	{
		if(num_pages == 2)
		{
			if(my_which_page == 2)    /*"both pages"*/
			{
				set_soft_clipping(0,0,1023,1023);
			}
			else
				set_soft_clipping(x1,4*y1/3,x2,
					4*y2/3);
		}
		else
			set_soft_clipping(x1,4*y1/3,x2,4*y2/3);
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
	color(i & 0x0f);
	the_color = i & 0x0f;
	current_color = i & 0x0f;
}


/*changes data in a color register*/
void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	rgb[0] = (red/4) | ((green/4) << 8);
	rgb[1] = blue/4;
	lut(the_register & 0x0f,1,&rgb[0]);
}


/*sets up the dashed line pattern*/
void far my_dash_pattern(i)
unsigned int i;
{
	m_dash_pattern = i;

	if(m_dash_enable == 't')
		pats(m_dash_pattern);
}



/*determines whether lines are dashed or not*/
void far my_dash_enable(i)
int i;
{
	if(i == 0)    /*disable dashing*/
	{
		m_dash_enable = 'f';
		pats(0xffff);
	}
	else if(i == 4)   /*enable dashing*/
	{
		m_dash_enable = 't';
		pats(m_dash_pattern);
	}

}




/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	lines(x1,y1,x2,y2);
}


/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{
	pputc(y/8,x/8,the_char);
}



/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{

}





/*clears a page of the display*/
void far clear_page(my_which_page)
int my_which_page;
{
	chr(0);
	clear_text_buffer();
	erase(0);
	chr(1);
}


/*clear screen in fast mode*/
void far clear_screen()
{
	chr(0);
	clear_text_buffer();
	erase(0);
	chr(1);
}



/*sets drawing mode (normal or complement)*/
void far draw_mode(i)
{
	if(i == 3)
		modes(0x03);
	else if(i == 0)
		modes(0);
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


	x_origin = headers_segs[which_cseg].x_origin + 2048;
	y_origin = headers_segs[which_cseg].y_origin + 2048;
	z_origin = headers_segs[which_cseg].z_origin + 2048;

	x1 += 2048;
	y1 += 2048;
	z1 += 2048;

	x2 += 2048;
	y2 += 2048;
	z2 += 2048;



	if(headers_segs[which_cseg].Xform_enable == 't')
	{
		memcpy(&matrix[0][0],&hcrst[0][0],sizeof(matrix));

/*		sprintf(diag_string,"******MATRIX = %f %f %f %f\n %f %f %f %f\n",
			matrix[0][0],matrix[1][0],matrix[2][0],matrix[3][0],
			matrix[0][1],matrix[1][1],matrix[2][1],matrix[3][1]);
		my_diag(diag_string);
*/

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

	
		x1 = (int) (my_xyz_prime[0]/1.33333) + 
			x_origin;
		y1 = (int) (my_xyz_prime[1]/1.33333) +
			y_origin;

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
	
		x2 = (int)(my_xyz_prime[0]/1.333333) +
			x_origin;
		y2 = (int) (my_xyz_prime[1]/1.33333) +
			y_origin;	
	}
	else
	{
		my_xyz_prime[0] = (float)(x1 - x_origin)
			/1.333333;
		x1 = (int)my_xyz_prime[0] + 
			x_origin;

		my_xyz_prime[0] = (float)(x2 - x_origin)
			/1.333333;
		x2 = (int)my_xyz_prime[0] +
			x_origin;


		my_xyz_prime[0] = (float)(y1 - y_origin)
			/1.333333;
		y1 = (int)my_xyz_prime[0] + 
			y_origin;

		my_xyz_prime[0] = (float)(y2 - y_origin)
			/1.333333;
		y2 = (int)my_xyz_prime[0] +
			y_origin;

	}

	x1 /= 4;
	x2 /= 4;
	y1 = (y1 - 512)/3;
	y2 = (y2 - 512)/3;


	if((which_page == 1) && (num_pages == 2))
	{
		/*switch to 2nd planes of display*/
	}

/*	sprintf(diag_string,"******LINXY(%d %d %d %d)\n",x1,y1,x2,y2);
	my_diag(diag_string);
*/


	clip_line(x1,y1,x2,y2);
}








void far my_char(ch,x1,y1,size,rot)
char ch;
int x1,y1,size,rot;
{
int row,col,z1;
int x_origin,y_origin,z_origin;


	if((no_draw_flag == 't') && (black == 'f'))
		return;

	if(headers_segs[which_cseg].visibility == 'f')
		return;


	x_origin = headers_segs[which_cseg].x_origin + 2048;
	y_origin = headers_segs[which_cseg].y_origin + 2048;
	z_origin = headers_segs[which_cseg].z_origin + 2048;

	x1 += 2048;
	y1 += 2048;
	z1 = my_z + 2048;


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

		x_y_z[0] = (float)(x1 - x_origin);
		x_y_z[1] = (float)(y1 - y_origin);
		x_y_z[2] = (float)(z1 - z_origin);
		x_y_z[3] = 1.0;
	
		x_y_z_prime[0] = 0.0;
		x_y_z_prime[1] = 0.0;
		x_y_z_prime[2] = 0.0;

		for(col = 0 ; col < 3 ; col++)
			for(row = 0 ; row < 4 ; row++)
				x_y_z_prime[col] += 
					hcrst[row][col] * x_y_z[row];

	
		x1 = (int) (x_y_z_prime[0]/1.333333) + x_origin;
		y1 = (int) (x_y_z_prime[1]/1.333333) + y_origin;
	}
	else
	{
		x_y_z_prime[0] = (float)(x1 - x_origin)/1.333333;
		x1 = (int)x_y_z_prime[0] + x_origin;


		x_y_z_prime[0] = (float)(y1 - y_origin)/1.333333;
		y1 = (int)x_y_z_prime[0] + y_origin;
	}


	x1 /= 4;
	y1 = (y1 - 512)/3;


        if((ch == '*') || (ch == '.') || (ch == '+'))
        {
		if(ch == '*')
		{
			if(comp_outcode(x1,y1) == 0)
        	        	asterisk(x1,y1);
		}
		else if(ch == '.')
		{
			my_x += 48;

			if(comp_outcode(x1,y1) == 0)
				dot(x1,y1);
		}
		else
		{
			if(comp_outcode(x1,y1) == 0)
				plus(x1,y1);
		}
		

                return;
        }


/*	y1 = y1 + char_y_off;
	x1 += char_x_off;
*/
	/*switch display planes if necessary*/
		
	/*output the character somehow*/
}






void far my_pan_box(x,y)
int x,y;
{
float current_hcrst_scale;


	goto_row_col(win_y_low + 1,win_x_low + 35);
	printf("X = %-5d ",new_x/4);
	goto_row_col(win_y_low + 2,win_x_low + 35);
	printf("Y = %-5d ",-new_y/4);


	if(number_of_segs_visible() > 2)
		return;


	current_hcrst_scale = sqrt(hcrst[0][0]*hcrst[0][0] +
				   hcrst[1][0]*hcrst[1][0] +
				    hcrst[2][0]*hcrst[2][0]);


	x = 3*((int)((x - last_mouse_x)*current_hcrst_scale) + 2048)/16 + 128;
	y = 1024 - ((int)((y - last_mouse_y)*current_hcrst_scale) + 2048)/4;

	draw_mode(3);
	my_color(save_system_color);

	
	my_line_xy(x - 383,y - 511 + ZOOM_Y_OFF,
		x - 383,y + 511 + ZOOM_Y_OFF);

	my_line_xy(x - 383,y - 511 + ZOOM_Y_OFF,
		x + 383,y - 511 + ZOOM_Y_OFF);

	my_line_xy(x + 383,y - 511 + ZOOM_Y_OFF,
		x + 383,y + 511 + ZOOM_Y_OFF);

	my_line_xy(x - 383,y + 511  + ZOOM_Y_OFF,
		x + 383,y + 511 + ZOOM_Y_OFF);

	my_color(current_color);
	draw_mode(0);
}




void far my_zoom_box(y)
int y;
{
float zoom_factor,last_zoom_factor;
int row,col;


	if(number_of_segs_visible() > 2)
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
		my_line_xy(512 - (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0/zoom_factor),
			512 + (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0/zoom_factor));

		my_line_xy(512 - (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0/zoom_factor),
			512 - (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0/zoom_factor));

		my_line_xy(512 - (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0/zoom_factor),
			512 + (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0/zoom_factor));

		my_line_xy(512 + (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0/zoom_factor),
			512 + (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0/zoom_factor));



/*   inner cross   */
		my_line_xy(512,
			512 + ZOOM_Y_OFF - (int)(512.0/zoom_factor),
			512,
			512 + ZOOM_Y_OFF + (int)(512.0/zoom_factor));

		my_line_xy(512 - (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF,
			512 + (int)(383.0/zoom_factor),
			512 + ZOOM_Y_OFF);


		my_circle((int)(27.0/zoom_factor),512,512 + ZOOM_Y_OFF);


#ifdef jerjriejfjiejir
/*inner box*/			
		my_line_xy(512 - (int)(25.0/zoom_factor),
			383 + ZOOM_Y_OFF - (int)(25.0/zoom_factor),
			512 + (int)(25.0/zoom_factor),
			383 + ZOOM_Y_OFF - (int)(25.0/zoom_factor));

		my_line_xy(512 - (int)(25.0/zoom_factor),
			383 + ZOOM_Y_OFF - (int)(25.0/zoom_factor),
			512 - (int)(25.0/zoom_factor),
			383 + ZOOM_Y_OFF + (int)(25.0/zoom_factor));

		my_line_xy(512 - (int)(25.0/zoom_factor),
			383 + ZOOM_Y_OFF + (int)(25.0/zoom_factor),
			512 + (int)(25.0/zoom_factor),
			383 + ZOOM_Y_OFF + (int)(25.0/zoom_factor));

		my_line_xy(512 + (int)(25.0/zoom_factor),
			383 + ZOOM_Y_OFF + (int)(25.0/zoom_factor),
			512 + (int)(25.0/zoom_factor),
			383 + ZOOM_Y_OFF - (int)(25.0/zoom_factor));
#endif

	}
	else  /*zoom out*/
	{
		y = -y;

		zoom_factor = (0.91356*y + 2047.0)/2048.0;


/*box*/
		my_line_xy(512 - (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0*zoom_factor),
			512 + (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0*zoom_factor));

		my_line_xy(512 - (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0*zoom_factor),
			512 - (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0*zoom_factor));

		my_line_xy(512 - (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0*zoom_factor),
			512 + (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0*zoom_factor));

		my_line_xy(512 + (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0*zoom_factor),
			512 + (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0*zoom_factor));


/*outer cross*/
		my_line_xy(512 - (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0*zoom_factor),
			512 - (int)(383.0*zoom_factor) - 2000,
			512 + ZOOM_Y_OFF - (int)(512.0*zoom_factor) - 2666);

		my_line_xy(512 - (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0*zoom_factor),
			512 - (int)(383.0*zoom_factor) - 2000,
			512 + ZOOM_Y_OFF + (int)(512.0*zoom_factor) + 2666);

		my_line_xy(512 + (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF - (int)(512.0*zoom_factor),
			512 + (int)(383.0*zoom_factor) + 2000,
			512 + ZOOM_Y_OFF - (int)(512.0*zoom_factor) - 2666);

		my_line_xy(512 + (int)(383.0*zoom_factor),
			512 + ZOOM_Y_OFF + (int)(512.0*zoom_factor),
			512 + (int)(383.0*zoom_factor) + 2000,
			512 + ZOOM_Y_OFF + (int)(512.0*zoom_factor) + 2666);

	}    /*end else zoom out*/


	my_color(current_color);
	draw_mode(0);
}


