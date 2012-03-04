#include "stdio.h"

FILE *fopen(),*out_ptr,*com_ptr = NULL;


extern void far open_megatek();
extern void far open_host();
extern void far ugout();
extern void far megatek_out();
extern int far read_megatek_buffer();
extern int far read_host_buffer();

void write_megatek(ch)
int ch;
{

}


int read_megatek()
{
int ch;

	ch = read_megatek_buffer();

	while(ch == -1)
		ch = read_megatek_buffer();

	return(ch & 0x3f);
}


void print_binary(number)
unsigned int number;
{
unsigned int mask;

	for(mask = 0x8000 ; mask > 0 ; mask >>= 1)
		if((number & mask) != 0)
			putc('1',out_ptr);
		else
			putc('0',out_ptr);

	putc('\n',out_ptr);
}


unsigned int comp16_3(first,second,third)
unsigned char first,second,third;
{
unsigned int i;

	i = (int)first & 0x0f;
	i <<= 6;
	i |= (int)second & 0x3f;
	i <<= 6;
	i |= (int)third & 0x3f;
	return(i);
}


void write_insert() /*1*/
{
unsigned int response,most_sig,least_sig;
unsigned char most1,most2,most3,least1,least2,least3;

	printf("Write At Insert Pointer\n");
	printf("Enter Word To Write: (Most-Sig.-Half  Least-Sig.-Half)  ");
	scanf("%u%u",&most_sig,&least_sig);
	fprintf(out_ptr,"Write At Insert Pointer: Most Sig. = %u Least Sig. = %u\n",
			most_sig,least_sig);
	fflush(out_ptr);

	most1 = most_sig >> 12;
	most2 = (most_sig >> 6) & 0x3f;
	most3 = most_sig & 0x3f;

	least1 = least_sig >> 12;
	least2 = (least_sig >> 6) & 0x3f;
	least3 = least_sig & 0x3f;

	write_megatek(0x6f); /*op code*/

	write_megatek(most1 | 32);
	write_megatek(most2);
	write_megatek(most3);

	write_megatek(least1);
	write_megatek(least2);
	write_megatek(least3);

	response = read_megatek(); 
	response = read_megatek(); /*check_sum*/
	fprintf(out_ptr,"    Check Sum = %u\n",response);
	fflush(out_ptr);
	response = read_megatek();

}



void write_spec_addr() /*2*/
{
unsigned int response,address,most_sig_data,least_sig_data;
unsigned char address1,address2,address3,most1,most2,most3,
		least1,least2,least3;

	printf("Write Specified Address\n");
	printf("Enter (Address  Most-Sig.-Data  Least-Sig.-Data)  ");
	scanf("%u%u%u",&address,&most_sig_data,&least_sig_data);
	fprintf(out_ptr,"Write Spec. Addr.: Addr = %u Most Sig. = %u Least Sig. = %u\n",
			address,most_sig_data,least_sig_data);
	fflush(out_ptr);

	address1 = address >> 12;
	address2 = (address >> 6) & 0x3f;
	address3 = address & 0x3f;

	write_megatek(0x6f); /*op code*/

	write_megatek(address1 | 16);
	write_megatek(address2);
	write_megatek(address3);

	most1 = most_sig_data >> 12;
	most2 = (most_sig_data >> 6) & 0x3f;
	most3 = most_sig_data & 0x3f;

	write_megatek(most1);
	write_megatek(most2);
	write_megatek(most3);

	least1 = least_sig_data >> 12;
	least2 = (least_sig_data >> 6) & 0x3f;
	least3 = least_sig_data & 0x3f;

	write_megatek(least1);
	write_megatek(least2);
	write_megatek(least3);

	response = read_megatek();
	response = read_megatek(); /*check sum*/
	fprintf(out_ptr,"    Check Sum = %u\n",response);
	fflush(out_ptr);
	response = read_megatek();
}


void read_spec_addr() /*3*/
{
unsigned int address,most_sig_data,least_sig_data;
unsigned char address1,address2,address3,most1,most2,most3,
		least1,least2,least3,a_char;

	printf("Read Specified Address\n");
	printf("Enter Address To Read From ");
	scanf("%u",&address);
	fprintf(out_ptr,"Read Spec. Address: Addr. = %u\n",address);

	address1 = address >> 12;
	address2 = (address >> 6) & 0x3f;
	address3 = address & 0x3f;

	write_megatek(0x6f);	/*op code*/

	write_megatek(address1);
	write_megatek(address2);
	write_megatek(address3);

	a_char = read_megatek(); /*GS*/

	most1 = read_megatek();
	most2 = read_megatek();
	most3 = read_megatek();

	most_sig_data = comp16_3(most1,most2,most3);

	least1 = read_megatek();
	least2 = read_megatek();
	least3 = read_megatek();

	a_char = read_megatek(); /*0x0d*/

	least_sig_data = comp16_3(least1,least2,least3);

	printf("data = %u  %u\n",most_sig_data,least_sig_data);

	fprintf(out_ptr,"    Most Sig Data = %u = ",most_sig_data);
	print_binary(most_sig_data);

	fprintf(out_ptr,"    least Sig Data = %u = ",least_sig_data);
	print_binary(least_sig_data);

	fflush(out_ptr);
}


void read_pos_id() /*4*/
{
unsigned int segment_num,id;
unsigned char a_char,seg_most,seg_least,id1,id2,id3;

	printf("Read Current Position ID\n");

	write_megatek(0x7c);  /*op code*/
	write_megatek(0x00);  /*reserved byte*/

	a_char = read_megatek(); /*GS*/
	a_char = read_megatek(); /*Valid Read Bit , Segment/Subroutine Bit*/

	if((a_char & 16) != 0)  /*Subroutine Read ID*/
	{
		printf("Subroutine");
		fprintf(out_ptr,"Read Pos. ID: Subroutine ");
	}
	else  /*Segment Read ID*/
	{
		printf("Segment");
		fprintf(out_ptr,"Read Pos. ID: Segment ");
	}

	seg_most = read_megatek();
	seg_least = read_megatek();

	id1 = read_megatek();
	id2 = read_megatek();
	id3 = read_megatek();

	a_char = read_megatek(); /*0x0d*/

	segment_num = ((seg_most & 0x3f) << 6) | (seg_least & 0x3f);
	id = comp16_3(id1,id2,id3);

	printf(" # = %u  ID = %u\n",segment_num,id);
	fprintf(out_ptr,"# = %u  ID = %u\n",segment_num,id);
	fflush(out_ptr);

}


void short_rel() /*5*/
{
	printf("Short Relative Vectors\n");
	fprintf(out_ptr,"Short Relative Vectors\n");
	fflush(out_ptr);

	write_megatek('P');  /*move 2D absolute*/

	write_megatek(0x00); /* x */
	write_megatek(0x00); /* coord */

	write_megatek(0x00); /* y */
	write_megatek(0x00); /* coord */


	write_megatek(0x59); /*short rel op code*/

	write_megatek(0x40);
	write_megatek(0x6C);
	write_megatek(0x7D);
	write_megatek(0x6D);
	write_megatek(0x6C);
	write_megatek(0x78);
	write_megatek(0x6C);
	write_megatek(0x6C);
	write_megatek(0x72);
	write_megatek(0x6B);
	write_megatek(0x6C);
	write_megatek(0x6C);
	write_megatek(0x69);
	write_megatek(0x6C);
	write_megatek(0x68);
	write_megatek(0x65);
	write_megatek(0x6C);
	write_megatek(0x63);
	write_megatek(0x63);
	write_megatek(0x6C);
	write_megatek(0x5E);
	write_megatek(0x5E);
	write_megatek(0x6C);
	write_megatek(0x5B);
	write_megatek(0x5A);
	write_megatek(0x6C);
	write_megatek(0x59);
	write_megatek(0x54);
	write_megatek(0x6C);
	write_megatek(0x55);
	write_megatek(0x50);
	write_megatek(0x6C);
	write_megatek(0x54);
	write_megatek(0x4A);
	write_megatek(0x6C);
	write_megatek(0x54);
	write_megatek(0x44);
	write_megatek(0x7C);
	write_megatek(0x53);
	write_megatek(0x3F);
	write_megatek(0x7C);
	write_megatek(0x53);
	write_megatek(0x79);
	write_megatek(0x7C);
	write_megatek(0x55);
	write_megatek(0x73);
	write_megatek(0x7C);
	write_megatek(0x57);
	write_megatek(0x6E);
	write_megatek(0x7C);
	write_megatek(0x5A);
	write_megatek(0x68);
	write_megatek(0x7C);
	write_megatek(0x5D);
	write_megatek(0x64);
	write_megatek(0x7C);
	write_megatek(0x60);
	write_megatek(0x60);
	write_megatek(0x7C);
	write_megatek(0x65);
	write_megatek(0x5C);
	write_megatek(0x7C);
	write_megatek(0x6A);
	write_megatek(0x59);
	write_megatek(0x7C);
	write_megatek(0x70);
	write_megatek(0x56);
	write_megatek(0x7C);
	write_megatek(0x74);
	write_megatek(0x55);
	write_megatek(0x7C);
	write_megatek(0x7A);
	write_megatek(0x53);
	write_megatek(0x78);
	write_megatek(0x40);
	write_megatek(0x53);
	write_megatek(0x78);
	write_megatek(0x46);
	write_megatek(0x53);
	write_megatek(0x78);
	write_megatek(0x4C);
	write_megatek(0x55);
	write_megatek(0x78);
	write_megatek(0x50);
	write_megatek(0x56);
	write_megatek(0x78);
	write_megatek(0x56);
	write_megatek(0x59);
	write_megatek(0x78);
	write_megatek(0x5B);
	write_megatek(0x5C);
	write_megatek(0x78);
	write_megatek(0x60);
	write_megatek(0x60);
	write_megatek(0x78);
	write_megatek(0x63);
	write_megatek(0x64);
	write_megatek(0x78);
	write_megatek(0x66);
	write_megatek(0x68);
	write_megatek(0x78);
	write_megatek(0x69);
	write_megatek(0x6E);
	write_megatek(0x78);
	write_megatek(0x6B);
	write_megatek(0x73);
	write_megatek(0x78);
	write_megatek(0x6D);
	write_megatek(0x79);
	write_megatek(0x78);
	write_megatek(0x6D);
	write_megatek(0x3F);
	write_megatek(0x68);
	write_megatek(0x6C);
	write_megatek(0x44);
	write_megatek(0x68);
	write_megatek(0x6C);
	write_megatek(0x4A);
	write_megatek(0x68);
	write_megatek(0x6B);
	write_megatek(0x50);
	write_megatek(0x68);
	write_megatek(0x67);
	write_megatek(0x54);
	write_megatek(0x68);
	write_megatek(0x65);
	write_megatek(0x5A);
	write_megatek(0x68);
	write_megatek(0x62);
	write_megatek(0x5E);
	write_megatek(0x68);
	write_megatek(0x5D);
	write_megatek(0x63);
	write_megatek(0x68);
	write_megatek(0x58);
	write_megatek(0x65);
	write_megatek(0x68);
	write_megatek(0x54);
	write_megatek(0x69);
	write_megatek(0x68);
	write_megatek(0x4E);
	write_megatek(0x6B);
	write_megatek(0x68);
	write_megatek(0x48);
	write_megatek(0x6C);
	write_megatek(0x68);
	write_megatek(0x43);
	write_megatek(0x6D);
	write_megatek(0x40);

}



void open_seg()
{
unsigned char most_seg,least_seg;
unsigned int segment;

	printf("Open Segment\n");
	printf("Enter Segment # ");
	scanf("%u",&segment);
	fprintf(out_ptr,"Open Segment %u\n",segment);
	fflush(out_ptr);

	most_seg = segment >> 6;
	least_seg = segment & 0x3f;

	write_megatek(0x40); /*op code*/
	write_megatek(0x07); /*use display 0 only,transform segment,is
				visible*/
	write_megatek(most_seg);
	write_megatek(least_seg);
}


void close_seg()
{
	printf("Close Segment\n");
	fprintf(out_ptr,"Close Segment\n");
	fflush(out_ptr);
	write_megatek(0x44);
}


void open_sub()
{
unsigned char most_sub,least_sub;
unsigned int subroutine;

	printf("Open Subroutine\n");
	printf("Enter Subroutine # ");
	scanf("%u",&subroutine);
	fprintf(out_ptr,"Open Subroutine %u\n",subroutine);
	fflush(out_ptr);

	most_sub = subroutine >> 6;
	least_sub = subroutine & 0x3f;

	write_megatek(0x60); /*op code*/
	write_megatek(most_sub);
	write_megatek(least_sub);
}



void close_sub()
{
	printf("Close Subroutine\n");
	fprintf(out_ptr,"Close Subroutine\n");
	fflush(out_ptr);
	write_megatek(0x64); /*op code*/
}


main()
{
int selection = 0;
char a_file[50],show_flag = 't';
int i,data_char,ch,baud_rate;

	printf("Enter File Name For Your Menu Command Output : ");
	scanf("%s",a_file);

	out_ptr = fopen(a_file,"w");

	open_host(6);
	open_host(6);
	open_megatek(6);
	open_megatek(6);

   while(1 == 1)
   {

	if(kbhit() == 0)
	{
		data_char = read_megatek_buffer();
		if(data_char != -1)
		{

			if(show_flag == 't')
			{
				printf("_%x ",data_char);
				if(com_ptr != NULL)
					fprintf(com_ptr,"_%x ",data_char);
			}


		}

		data_char = read_host_buffer();
		if(data_char != -1)
		{

			if(show_flag == 't')
			{
				printf("%x ",data_char);
				if(com_ptr != NULL)
					fprintf(com_ptr,"%x ",data_char);
			}


		}



	}
	else
	{
	ch = getchar();
	selection = 0;

	while(selection != 20)
	{
		printf("\n\nEnter a Selection:\n");
		printf("  1) Write at Insert pointer\n");
		printf("  2) Write Specified Address\n");
		printf("  3) Read Specified Address\n");
		printf("  4) Read Current Position ID\n");
		printf("  5) Short Relative Vector String\n");
		printf("  6) Open Segment\n");
		printf("  7) Close Segment\n");
		printf("  8) Open Subroutine\n");
		printf("  9) Close Subroutine\n");
		printf("  10) Enter Message Monitor\n");
		printf("  11) Send Escape A Charactor\n");
		printf("  12) Exit Message Monitor\n");
		printf("  13) Show Host & Megatek Data\n");
		printf("  14) No Show Data\n");
		printf("  15) Enter Graphics Mode\n");
		printf("  16) Exit Graphics Mode\n");
		printf("  17) Open Com File & Write To It\n");
		printf("  18) Close Com File\n");
		printf("  19) Send Keyboard Data To The Megatek\n");
		printf("  20) Exit Menu\n");
		printf("  21) Exit Program\n");
		printf("  22) Set Baud Rate\n\n");
		printf("  ? : ");

		scanf("%d",&selection);

		switch(selection)
		{
			case 1 :
			{
				write_insert();
				break;
			}

			case 2 :
			{
				write_spec_addr();
				break;
			}

			case 3 :
			{
				read_spec_addr();
				break;
			}

			case 4 :
			{
				read_pos_id();
				break;
			}

			case 5 :
			{
				short_rel();
				break;
			}

			case 6 :
			{
				open_seg();
				break;
			}

			case 7 :
			{
				close_seg();
				break;
			}


			case 8 :
			{
				open_sub();
				break;
			}

			case 9 :
			{
				close_sub();
				break;
			}


			case 10 : /*Enter Message Monitor*/
			{

				break;
			}		

			case 11 : /*Send Escape Sequence*/
			{
				printf("Send Escape Sequence: Enter A Charactor: ");
				ch = getchar();
				scanf("%c",&ch);


				break;
			}


			case 12 : /*Exit Message Monitor*/
			{

				break;
			}


			case 13 : /*show data*/
			{
				show_flag = 't';
				break;
			}


			case 14 : /*no show*/
			{
				show_flag = 'f';
				break;
			}

			case 15 : /*enter graphics mode*/
			{

				break;
			}

			case 16 : /*exit graphics mode*/
			{

				break;
			}


			case 17 : /*open com file & write to it*/
			{
				if(com_ptr == NULL)
				{
					printf("Enter The Com File's Name: ");
					ch = getchar();
					scanf("%s",a_file);
					printf("Write Down The Com File's Name = %s\n",
						a_file);

					com_ptr = fopen(a_file,"w");
				}
				else
					printf("A Com File Is Already Open!!, Ignored!\n");

				break;
			}

			case 18 : /*close com file*/
			{
				if(com_ptr != NULL)
				{
					printf("Closing Com File: %s\n",a_file);
					fclose(com_ptr);
					com_ptr = NULL;
				}
				else
					printf("No Open Com File!!, Ignored!!\n");

				break;
			}


			case 19 : /*send keyboard input to megatek*/
			{
				ch = getchar();

				printf("Type On The The Keyboard & It Will Go To The Megatek.\n");
				printf("Type a tilde (~) to stop\n");

				ch = ' ';
				while(ch != '~')
				{
					ch = getchar();

				}

				break;
			}

			case 20 : break;

			case 21 : goto my_end;

			case 22 : /*Enter baud rate*/
			{
				printf("Enter Baud Rate: (0 for 9600 or 1 for 19200) : ");
				scanf("%d",&baud_rate);

				if(baud_rate == 0)
				{
					open_host(12);
					open_megatek(12);
				}
				else if(baud_rate == 1)
				{
					open_host(6);
					open_megatek(6);
				}
				else
					printf("\nInvalid baud rate indicator: %d\n",
							baud_rate);

				break;
			}

			default : printf("\nInvalid Selection: %d\n",selection);
		}


		printf("\nHit Enter Key To Continue....\n");
		ch = getchar();
		ch = getchar();
	}
	}
    }

my_end:
	fclose(out_ptr);

	if(com_ptr != NULL)
		fclose(com_ptr);
}

