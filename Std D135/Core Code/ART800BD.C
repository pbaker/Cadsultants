#include "c:\emulate\d135\corecode\my_globs.h"
#include "math.h"


/*February 8, 1989 made place_char() put more space between rows of text*/
/*February 10, 1989 Made place_char add the which_page*768 to y's, instead
of the caller adding it in*/


#define ZOOM_Y_OFF 0
extern int far last_mouse_x;
extern int far last_neg_mouse_y;
extern int far last_mouse_y;
extern int far save_system_color;


extern int far number_of_segs_visible();


extern void far asterisk();
extern void far plus();
extern void far dot();
extern void far release_version();

extern void far restore_int9();

int far my_invert_y = 896;

int far current_color = 7;
float far x_res_factor = 1.28;
float far y_res_factor = 1.371;
float far res_factor = 1.28;

/*800 x 560 version of artist 10 series graphics calls*/




void far my_rdmask(i)
int i;
{

}

void far my_wrmask(i)
int i;
{

}




void far my_single_pixel(x,y)
int x,y;
{
	if(num_pages != 2)
	        plotxy((int)(x/x_res_factor),(int)(y/y_res_factor));
	else
	        plotxy((int)(x/x_res_factor),(int)((y + which_page*768)/
			y_res_factor));
}


void far my_circle(radius,x,y)
int radius,x,y;
{
	if(num_pages != 2)
		lincir((int)(x/x_res_factor),(int)(y/y_res_factor),
			(int)(radius/x_res_factor));
	else
		lincir((int)(x/x_res_factor),(int)((y + which_page*768)/
			y_res_factor),(int)(radius/x_res_factor));
}




void far close_board()
{

}



/*test the artist board & fill board_string*/
void far test_board()
{
unsigned int the_port,port_byte;

/*sets version_str[] for displaying version# & date*/
	release_version(); 


	the_port = 0x03e2;

	outp(the_port,0x00);
	port_byte = inp(the_port) & 0x0f;


	switch(port_byte)
	{
		case 0x03 :
		{
			sprintf(board_string,"Artist 10E  800 x 560");
			num_pages = 2;
			rotate_mode = 'm';
			break;
		}

		case 0x0f :
		{
			sprintf(board_string,"Artist 10  800 x 560");
			num_pages = 2;
			rotate_mode = 'm';
			break;
		}

		case 0x0c :
		{
			sprintf(board_string,"Artist 10/16  800 x 560");
			num_pages = 1;
			rotate_mode = 'm';
			break;
		}

		default :
		{
			 printf("Invalid Controller Board\n");
			 restore_int9();
			 exit();
		}
	}

	outp(the_port,0x0d);	
	port_byte = inp(the_port) & 0x0f;

	if(port_byte != 0x07)
	{
		printf("Invalid Graphics Controller Board!!!\n");
		restore_int9();
		exit();
	}

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
		svwpt(0,0,(int)(1023/x_res_factor),(int)(767/y_res_factor));
	else
	{
		if(num_pages == 2)
		{
			if(my_which_page == 2)    /*both pages*/
				svwpt(0,0,(int)(1023/x_res_factor),
					(int)((767 + 768)/y_res_factor));
			else
				svwpt((int)(x1/x_res_factor),(int)((y1 + 
					my_which_page*768)/y_res_factor),
					(int)(x2/x_res_factor),
					(int)((y2 + my_which_page*768)/
					y_res_factor));
		}
		else
			svwpt((int)(x1/x_res_factor),(int)(y1/y_res_factor),
				(int)(x2/x_res_factor),(int)(y2/y_res_factor));
	}
}



/*locks the drawing within the rectangular clipping window*/
void far my_lock_viewport()
{
	lokvwp();
}


/*scrolls between one page and the other*/
void far my_scroll(my_which_page)
int my_which_page;
{
	if(num_pages == 2)
		scrola(0,(int)(my_which_page*768/y_res_factor + 0.5));
	else
		scrola(0,0);
}


/*selects a color register*/
void far my_color(i)
int i;
{
	current_color = i & 0x0f;
	scolr(i & 0x0f);
}


/*changes data in a color register*/
void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	usrcol(the_register & 0x0f,red,green,blue);
}


/*sets up the dashed line pattern*/
void far my_dash_pattern(i)
unsigned int i;
{
	usrlin(i);
	sltyp(line_type);
}


/*determines whether lines are dashed or not*/
void far my_dash_enable(i)
int i;
{
	sltyp(i);
}


/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	if(num_pages == 2)
	{
		linxy((int)(x1/x_res_factor),(int)((y1 + which_page*768)/
			y_res_factor),(int)(x2/x_res_factor),
			(int)((y2 + which_page*768)/y_res_factor));
	}
	else
		linxy((int)(x1/x_res_factor),(int)(y1/y_res_factor),
			(int)(x2/x_res_factor),(int)(y2/y_res_factor));
}







/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{
	if(num_pages == 2)
	{
		schar(the_char,x + 150,(int)(1.5*y) + (int)(which_page*768/
			y_res_factor),rotate);
	}
	else
		schar(the_char,x + 150,(int)(1.5*y),rotate);
}



/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{
	sizchr(8,8,size,size);
}





/*clears a page of the display*/
void far clear_page(my_which_page)
int my_which_page;
{
	if(num_pages == 2)
	{
		if(my_which_page == 2)     /*both pages*/
			clrwrd(0,0,(int)(1023/x_res_factor),(int)((767 + 768)/
				y_res_factor),0);
		else
			clrwrd(0,(int)(my_which_page*768/y_res_factor),
				(int)(1023/x_res_factor),
				(int)((767 + 768*my_which_page)/y_res_factor),
				0);
	}
	else
		clrwrd(0,0,(int)(1023/x_res_factor),(int)(767/y_res_factor),0);
}



/*clear screen in fast mode*/
void far clear_screen()
{
	gclrf();
}



/*sets drawing mode (normal or complement)*/
void far draw_mode(i)
{
	drwmod(i);
}








void far my_line3(x1,y1,z1,x2,y2,z2)
int x1,y1,z1,x2,y2,z2;
{
int row,col;
float matrix[4][4];
float my_xyz[4];
float my_xyz_prime[4];
int x_origin,y_origin,z_origin;

/*	printf("*******MY_LINE3(%d %d %d %d %d %d)\n",
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


/*right now   128 <= x <= 896   &    128 <= y <= 896   So y inversion uses 896*/


	y1 = my_invert_y - y1;
	y2 = my_invert_y - y2;



	if((which_page == 1) && (num_pages == 2))
	{
		y1 += 768;
		y2 += 768;
	}



/*	sprintf(diag_string,"******LINXY(%d %d %d %d)\n",x1,y1,x2,y2);
	my_diag(diag_string);
*/

	linxy((int)(x1/x_res_factor),(int)(y1/y_res_factor),
		(int)(x2/x_res_factor),(int)(y2/y_res_factor));	
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



/*printf("MY_CHAR() ch = %x = %c , x = %d , y = %d\n",ch,ch,x1,y1);
*/


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


/*right now   128 <= x <= 896   &    128 <= y <= 896   So y inversion uses 896*/

        if((ch == '*') || (ch == '.') || (ch == '+'))
        {
                y1 = my_invert_y - y1;

		if(ch == '*')
                	asterisk(x1,y1);
		else if(ch == '.')
		{
			my_x += 48;
			my_single_pixel(x1,y1);
		}
		else
			plus(x1,y1);

                return;
        }


	y1 = my_invert_y - y1 + char_y_off;
	x1 += char_x_off;

	if(num_pages == 2)
		y1 += which_page*768;


	schar(ch,(int)(x1/x_res_factor),(int)(y1/y_res_factor),rot);
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