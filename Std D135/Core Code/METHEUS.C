#include "omegacol.h"
#include "c:\emulate\d135\corecode\my_globs.h"
#include "ctype.h"


#define TRUE 1
#define FALSE 0


#define ZOOM_Y_OFF 5
extern int far last_mouse_x;
extern int far last_neg_mouse_y;
extern int far last_mouse_y;
extern int far save_system_color;
extern int far init_baud_rate;


extern int far number_of_segs_visible();


extern void far restore_int9();
extern void far read_config_file();
extern void far read_lock();


extern void far asterisk();
extern void far plus();
extern void far dot();


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



float far res_factor = 1.0;
int far my_invert_y = 896;




/*dummy variables for read_config_file() to access*/
char far ems_mode;
char far have_display_list;
char far which_high_res;






/*1024 x 768 version of metheus graphics calls*/


void far my_rdmask(i)
int i;
{

}

void far my_wrmask(i)
int i;
{

}







void far my_circle(radius,x,y)
int radius,x,y;
{
	if(complement_on == 'f')
	{
		ccircle(x,y,radius);
	}
	else
	{   /*draw an XOR diamond*/
		movp1(x,y + radius);
		movp2(x + radius,y);
		xdraw();

		movp1(x,y - radius);
		movp2(x + radius,y);
		xdraw();

		movp1(x,y - radius);
		movp2(x - radius,y);
		xdraw();

		movp1(x,y + radius);
		movp2(x - radius,y);
		xdraw();
	}
}





/*opens & inits metheus board & reads metheus.set file*/
int far init_artist10()
{
int i;


	read_lock();
	read_config_file();


	if(omega_open(NULL) == 0)  /*error*/
	{
		printf("ERROR:  Metheus Micro-Code Has Not Beeen Loaded\n");
		printf("CAN'T Continue \n");

		restore_int9();

		exit(1);
	}

	omega_close();
	omega_open(NULL);

	ini();
	clipen(1);

	return(0);
}



/*closes metheus board*/
void far close_board()
{
	omega_close();
}




/*test the artist board & fill board_string*/
void far test_board()
{
	release_version();

	sprintf(board_string,"Metheus 1104");
}





/*sets up a rectanglular clipping window*/
void far my_set_viewport(x1,y1,x2,y2,my_which_page) 
int x1,y1,x2,y2,my_which_page;
{
int temp_y1;


	temp_y1 = y1;

	x1 = (int)(x1/5.33333333333) + 512;
	x2 = (int)(x2/5.33333333333) + 512;
	y1 = 768 - (int)((y2 + 2048)/5.333333333333);
	y2 = 768 - (int)((temp_y1 + 2048)/5.333333333333);



	if(my_which_page == SVWPT_PAGE_1)
	{
		crect(0,0,1023,767);
	}
	else
	{
		if(num_pages == 2)
		{
			if(my_which_page == 2)    /*"both pages"*/
			{
				crect(0,0,1023,767);
			}
			else
				crect(x1,y1,x2,y2);
		}
		else
			crect(x1,y1,x2,y2);
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
	setcol(i & 0x0f);
	the_color = i & 0x0f;
	current_color = i & 0x0f;
}


/*changes data in a color register*/
void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	cmap(the_register & 0x0f,red << 2,green << 2,blue << 2);
}


/*sets up the dashed line pattern*/
void far my_dash_pattern(i)
unsigned int i;
{
	switch(i)
	{
		case 0xffff : /*solid*/
		{
			pattern_num = 0;
			break;
		}

		case 0xeeee : /*dashed*/
		{
			pattern_num = 1;
			break;
		}

		case 0xffb6 : /*phantom*/
		{
			pattern_num = 4;
			break;
		}

		case 0xfff6 : /*centerline*/
		{
			pattern_num = 3;
			break;
		}
	}


	if(m_dash_enable == 't')
	{
		m_dash_pattern = (pattern_num & 0x07) | 8;
		patern(m_dash_pattern);
	}

}


/*determines whether lines are dashed or not*/
void far my_dash_enable(i)
int i;
{
	if(i == 0)    /*disable dashing*/
	{
		m_dash_enable = 'f';
		m_dash_pattern = 8;
		patern(m_dash_pattern);
	}
	else if(i == 4)   /*enable dashing*/
	{
		m_dash_enable = 't';
		m_dash_pattern = (pattern_num & 0x07) | 8;
		patern(m_dash_pattern);
	}

}




/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	movp1(x1,y1);
	movp2(x2,y2);

	if(complement_on == 't')
	{
		xdraw();
	}
	else
		draw();

}


/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{
	a_char_str[0] = the_char;
	a_char_str[1] = 0x1b;
	movp1((int)(x*1.2) + 150,(int)(y*1.5) + 20);
	chars1(1,a_char_str);
}



/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{
	setcsz(size,size);
}





/*clears a page of the display*/
void far clear_page(my_which_page)
int my_which_page;
{
	setcol(0)
	clear();
	setcol((char)the_color)
}


/*clear screen in fast mode*/
void far clear_screen()
{
	setcol(0)
	clear();
	setcol((char)the_color)
}



/*sets drawing mode (normal or complement)*/
void far draw_mode(i)
{
	if(i == 3)
		complement_on = 't';
	else if(i == 0)
		complement_on = 'f';
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
	y1 /= 4;
	y2 /= 4;

	y1 = my_invert_y - y1;
	y2 = my_invert_y - y2;

	if((which_page == 1) && (num_pages == 2))
	{
		/*switch to 2nd planes of display*/
	}

/*	sprintf(diag_string,"******LINXY(%d %d %d %d)\n",x1,y1,x2,y2);
	my_diag(diag_string);
*/

	movp1(x1,y1);
	movp2(x2,y2);

	if(complement_on == 't')
	{
		xdraw();
	}
	else
	{
		draw();
	}

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
	y1 /= 4;


        if((ch == '*') || (ch == '.') || (ch == '+'))
        {
                y1 = my_invert_y - y1;

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


	y1 = my_invert_y - y1 + char_y_off + 9;
	x1 += char_x_off;

	/*switch display planes if necessary*/
		

	a_char_str[0] = ch;
	a_char_str[1] = 0x1b;
	movp1(x1,y1);
	chars1(1,a_char_str);
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
	y = 3*((int)((y - last_mouse_y)*current_hcrst_scale) + 2048)/16;

	draw_mode(3);
	my_color(save_system_color);

	
	my_line_xy(x - 383,y - 383 + ZOOM_Y_OFF,
		x - 383,y + 383 + ZOOM_Y_OFF);

	my_line_xy(x - 383,y - 383 + ZOOM_Y_OFF,
		x + 383,y - 383 + ZOOM_Y_OFF);

	my_line_xy(x + 383,y - 383 + ZOOM_Y_OFF,
		x + 383,y + 383 + ZOOM_Y_OFF);

	my_line_xy(x - 383,y + 383  + ZOOM_Y_OFF,
		x + 383,y + 383 + ZOOM_Y_OFF);

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
			383 + ZOOM_Y_OFF - (int)(383.0/zoom_factor),
			512 + (int)(383.0/zoom_factor),
			383 + ZOOM_Y_OFF - (int)(383.0/zoom_factor));

		my_line_xy(512 - (int)(383.0/zoom_factor),
			383 + ZOOM_Y_OFF - (int)(383.0/zoom_factor),
			512 - (int)(383.0/zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0/zoom_factor));

		my_line_xy(512 - (int)(383.0/zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0/zoom_factor),
			512 + (int)(383.0/zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0/zoom_factor));

		my_line_xy(512 + (int)(383.0/zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0/zoom_factor),
			512 + (int)(383.0/zoom_factor),
			383 + ZOOM_Y_OFF - (int)(383.0/zoom_factor));



/*   inner cross   */
		my_line_xy(512,
			383 + ZOOM_Y_OFF - (int)(383.0/zoom_factor),
			512,
			383 + ZOOM_Y_OFF + (int)(383.0/zoom_factor));

		my_line_xy(512 - (int)(383.0/zoom_factor),
			383 + ZOOM_Y_OFF,
			512 + (int)(383.0/zoom_factor),
			383 + ZOOM_Y_OFF);


		my_circle((int)(27.0/zoom_factor),512,383 + ZOOM_Y_OFF);


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
			383 + ZOOM_Y_OFF - (int)(383.0*zoom_factor),
			512 + (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF - (int)(383.0*zoom_factor));

		my_line_xy(512 - (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF - (int)(383.0*zoom_factor),
			512 - (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0*zoom_factor));

		my_line_xy(512 - (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0*zoom_factor),
			512 + (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0*zoom_factor));

		my_line_xy(512 + (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0*zoom_factor),
			512 + (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF - (int)(383.0*zoom_factor));


/*outer cross*/
		my_line_xy(512 - (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF - (int)(383.0*zoom_factor),
			512 - (int)(383.0*zoom_factor) - 10000,
			383 + ZOOM_Y_OFF - (int)(383.0*zoom_factor) - 10000);

		my_line_xy(512 - (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0*zoom_factor),
			512 - (int)(383.0*zoom_factor) - 10000,
			383 + ZOOM_Y_OFF + (int)(383.0*zoom_factor) + 10000);

		my_line_xy(512 + (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF - (int)(383.0*zoom_factor),
			512 + (int)(383.0*zoom_factor) + 10000,
			383 + ZOOM_Y_OFF - (int)(383.0*zoom_factor) - 10000);

		my_line_xy(512 + (int)(383.0*zoom_factor),
			383 + ZOOM_Y_OFF + (int)(383.0*zoom_factor),
			512 + (int)(383.0*zoom_factor) + 10000,
			383 + ZOOM_Y_OFF + (int)(383.0*zoom_factor) + 10000);

	}    /*end else zoom out*/


	my_color(current_color);
	draw_mode(0);
}

