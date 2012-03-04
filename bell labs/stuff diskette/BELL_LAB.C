#include "stdio.h"

FILE *fopen(),*out_ptr,*com_ptr = NULL;
char a_file[50],show_flag = 't';

extern void far open_megatek();
extern void far open_host();
extern void far ugout();
extern void far megatek_out();
extern int far read_megatek_buffer();
extern int far read_host_buffer();

void write_megatek(ch)
int ch;
{
	megatek_out(ch | 0x40);
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







void display_binary(number)
unsigned int number;
{
unsigned int mask;

	for(mask = 0x8000 ; mask > 0 ; mask >>= 1)
		if((number & mask) != 0)
			putchar('1');
		else
			putchar('0');

	putchar('\n');
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

	fprintf(out_ptr,"    MSD = %6u = ",most_sig_data);
	print_binary(most_sig_data);

	fprintf(out_ptr,"    LSD = %6u = ",least_sig_data);
	print_binary(least_sig_data);

	
}


void old_read_pos_id() /*4*/
{
unsigned int segment_num,id;
unsigned char a_char,seg_most,seg_least,id1,id2,id3;

	printf("Old Read Current Position ID\n");

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
	

}





void new_read_pos_id() /*4*/
{
unsigned int segment_num;
unsigned long id;
unsigned char a_char,seg_most,seg_least,id1,id2,id3,id4;

	printf("New Read Current Position ID\n");

	write_megatek(0x5b);  /*op code*/
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
	id4 = read_megatek();

	a_char = read_megatek(); /*0x0d*/

	segment_num = ((seg_most & 0x3f) << 6) | (seg_least & 0x3f);

	id = (unsigned long)(id1 & 0x03);
	id <<= 6;
	id |= (unsigned long)(id2 & 0x3f);
	id <<= 6;
	id |= (unsigned long)(id3 & 0x3f);
	id <<= 6;
	id |= (unsigned long)(id4 & 0x3f);

	printf(" # = %u  ID = %lu\n",segment_num,id);
	fprintf(out_ptr,"# = %u  ID = %lu\n",segment_num,id);
}


void short_rel() /*5*/
{
	printf("Short Relative Vectors\n");
	fprintf(out_ptr,"Short Relative Vectors\n");
	

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
	
	write_megatek(0x64); /*op code*/
}




void resets() /*23*/
{
int selection;

reset_again:

	printf("\n\nSystem Resets Menu: Enter Selection\n\n");
	printf("      0) Software Reset No EDLM\n");
	printf("      1) Hardware Reset No EDLM\n");
	printf("      2) Software Reset Into EDLM\n");
	printf("      3) Hardware Reset Into EDLM\n\n");
	printf("      ? : ");

	scanf("%d",&selection);



	switch(selection)
	{
		case 0 : /*software reset no EDLM*/
		{
			printf("Software Reset No EDLM\n");
			fprintf(out_ptr,"Software Reset No EDLM\n");
			write_megatek(0x6e); /*op code*/
			write_megatek(0x02);	     
			break;
		}

		case 1 : /*hardware reset no EDLM*/
		{
			printf("Hardware Reset No EDLM\n");
			fprintf(out_ptr,"Hardware Reset No EDLM\n");
			write_megatek(0x6e); /*op code*/
			write_megatek(0x03);
			break;
		}

		case 2 : /*software reset into EDLM*/
		{
			printf("Software Reset Into EDLM\n");
			fprintf(out_ptr,"Software Reset Into EDLM\n");
			write_megatek(0x6e); /*op code*/
			write_megatek(0x06);
			break;
		}

		case 3 : /*hardware reset into EDLM*/
		{
			printf("Hardware Reset Into EDLM\n");
			fprintf(out_ptr,"Hardware Reset Into EDLM\n");
			write_megatek(0x6e); /*op code*/
			write_megatek(0x07);
			break;
		}

		default : 
		{
			printf("Invalid Selection\n");
			goto reset_again;
		}
	}
}





void clear_seg() /*24*/
{
unsigned int segment_num;
unsigned char most_seg,least_seg;

	printf("Enter Segment # To Be Cleared: ");
	scanf("%d",&segment_num);
	fprintf(out_ptr,"Clear Segment # %d\n",segment_num);

	most_seg = segment_num >> 6;
	least_seg = segment_num & 0x3f;

	write_megatek(0x42);  /*op code*/
	write_megatek(most_seg);
	write_megatek(least_seg);
}






void append_seg() /*25*/
{
unsigned int segment_num;
unsigned char most_seg,least_seg;

	printf("Enter Segment # To Append To: ");
	scanf("%d",&segment_num);
	fprintf(out_ptr,"Append Segment # %d\n",segment_num);

	most_seg = segment_num >> 6;
	least_seg = segment_num & 0x3f;

	write_megatek(0x41);  /*op code*/
	write_megatek(most_seg);
	write_megatek(least_seg);
}







void read_many_addr() /*26*/
{
unsigned int address,most_sig_data,least_sig_data,start_address,end_address;
unsigned char address1,address2,address3,most1,most2,most3,
		least1,least2,least3,a_char;

	printf("Read Many Addresses\n");
	printf("Enter Starting Address To Read From: ");
	scanf("%u",&start_address);
	printf("Enter Number Of Words To Read: ");
	scanf("%u",&end_address);
	printf("\n\n");

	fprintf(out_ptr,"Read Many Addresses:\nStart Addr. = %u , # Words = %u\n\n",
		start_address,end_address);

	end_address += start_address;

	for(address = start_address ; address <= end_address ; address++)
	{
		printf("Address = %u \n ",address);
		fprintf(out_ptr,"Address = %u \n",address);

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


		printf("    MSD = %6u = ",most_sig_data);
		display_binary(most_sig_data);

		printf("    LSD = %6u = ",least_sig_data);
		display_binary(least_sig_data);


		if((most_sig_data & 0xff00) == 0xfc00) /*its a jump*/
		{
			printf("Its A Jump/Absolute/Relative\n");
			fprintf(out_ptr,"Its A Jump/Absolute/Relative\n");
		}

		fprintf(out_ptr,"    MSD = %6u = ",most_sig_data);
		print_binary(most_sig_data);

		fprintf(out_ptr,"    LSD = %6u = ",least_sig_data);
		print_binary(least_sig_data);

	}

	printf("\n\nEnd Of Read Many Addresses\n");
	fprintf(out_ptr,"\n\nEnd Of Read Many Addresses\n");
}



void clear_sub()
{
unsigned int sub_num;
unsigned char most_sub,least_sub;

	printf("Clear Subroutine\n");
	printf("Enter Subroutine's #: ");
	scanf("%d",&sub_num);
	fprintf(out_ptr,"Clear Subroutine # %d\n",sub_num);

	most_sub = (sub_num >> 6) & 0x3f;
	least_sub = sub_num & 0x3f;

	write_megatek(0x62); /*op code*/
	write_megatek(most_sub);
	write_megatek(least_sub);
}



void append_sub()
{
unsigned int sub_num;
unsigned char most_sub,least_sub;

	printf("Append Subroutine\nEnter Subroutine's #: ");
	scanf("%d",&sub_num);
	fprintf(out_ptr,"Append Subroutine # %d\n",sub_num);

	most_sub = (sub_num >> 6) & 0x3f;
	least_sub = sub_num & 0x3f;

	write_megatek(0x61); /*op code*/
	write_megatek(most_sub);
	write_megatek(least_sub);
}



void delete_sub()
{
unsigned int sub_num;
unsigned char most_sub,least_sub;


	printf("Delete Subroutine\nEnter Subroutine's #: ");
	scanf("%d",&sub_num);
	fprintf(out_ptr,"Delete Subroutine # %d\n",sub_num);

	most_sub = (sub_num >> 6) & 0x3f;
	least_sub = sub_num & 0x3f;

	write_megatek(0x63); /*op code*/
	write_megatek(most_sub);
	write_megatek(least_sub);
}



void delete_seg()
{
unsigned int seg_num;
unsigned char most_seg,least_seg;


	printf("Delete Segment\nEnter Segment's #: ");
	scanf("%d",&seg_num);
	fprintf(out_ptr,"Delete Segment # %d\n",seg_num);

	most_seg = (seg_num >> 6) & 0x3f;
	least_seg = seg_num & 0x3f;

	write_megatek(0x43); /*op code*/
	write_megatek(most_seg);
	write_megatek(least_seg);
}




void jump_ext()
{
unsigned int most_sig,least_sig;
long address;
int ch,newline;

	newline = getchar();
        printf("Compute Address Of An Extended Jump\n");
	printf("Is it an absolute jump (y/n)? ");
	ch = getchar();
	newline = getchar();
	printf("\n");
	printf("Enter (Most Sig. Data  Least Sig. Data): ");
	scanf("%u%u",&most_sig,&least_sig);

	fprintf(out_ptr,"Compute Addr. Ext. Jump: MSD = %u , LSD = %u\n",
		most_sig,least_sig);

	if(ch == 'y')
		address = (((long)most_sig << 16) | (long)least_sig) &
			0x000fffff;
	else
	{
		address = (((long)most_sig << 16) | (long)least_sig) &
			0x000fffff;
		if((address & 0x00080000) != 0) /*its a negative displacement*/
		{
			address = ((~address) & 0x000fffff) + 1;
			address = -address;
		}
	}

	if(ch == 'y')
	{
		printf("Address = %ld\n",address);
		fprintf(out_ptr,"Address = %ld\n",address);
	}
	else
	{
		printf("Displacement = %ld\n",address);
		fprintf(out_ptr,"Displacement = %ld\n",address);
	}
}



void read_old_start()
{
unsigned char most_num,least_num,first,second,third,ch;
unsigned int seg_num,address;

	printf("Old Read Segment Start Address\nEnter Segment Number: ");
	fprintf(out_ptr,"Old Read Segment Start Address\n");

	scanf("%d",&seg_num);

	fprintf(out_ptr,"Segment # = %d\n",seg_num);

	most_num = (seg_num >> 6) & 0x3f;
	least_num = seg_num & 0x3f;

	
	write_megatek('{');  /*op code*/
	write_megatek(most_num);
	write_megatek(least_num);

	ch = read_megatek();  /*GS*/
	first = read_megatek();
	second = read_megatek();
	third = read_megatek();
	ch = read_megatek();  /*0x0d*/

	address = comp16_3(first,second,third);
	printf("Start Address = %u\n",address);
	fprintf(out_ptr,"Start Address = %u\n",address);
}



void read_new_start()
{
unsigned char most_num,least_num,first,second,third,fourth,ch;
unsigned int seg_num;
long address;

	printf("EDLM Read Segment Start Address\nEnter Segment Number: ");
	fprintf(out_ptr,"EDLM Read Segment Start Address\n");

	scanf("%d",&seg_num);

	fprintf(out_ptr,"Segment # = %d\n",seg_num);

	most_num = (seg_num >> 6) & 0x3f;
	least_num = seg_num & 0x3f;

	
	write_megatek('Z');  /*op code*/
	write_megatek(most_num);
	write_megatek(least_num);

	ch = read_megatek();  /*GS*/
	first = read_megatek();
	second = read_megatek();
	third = read_megatek();
	fourth = read_megatek();
	ch = read_megatek();  /*0x0d*/

	address = (long)(first & 0x03);
	address <<= 6;
	address |= (long)(second & 0x3f);
	address <<= 6;
	address |= (long)(third & 0x3f);
	address <<= 6;
	address |= (long)(fourth & 0x3f);

	printf("Start Address = %ld\n",address);
	fprintf(out_ptr,"Start Address = %ld\n",address);
}



void mem_menu()
{
int selection = 0;

	
	while(selection != 20)
	{
		printf("\n\nMemory Menu: Enter Selection\n\n");
		printf("      0) Read Position ID (Old Version)\n");
		printf("      1) Read Spec. Address\n");
		printf("      2) Read Many Addresses\n");
		printf("      3) Write At Insert Pointer\n");
		printf("      4) Write Spec. Address\n");
		printf("      5) Read Position ID (EDLM Version)\n");
		printf("      6) Read Segment Start Address (Old Version)\n");
		printf("      7) Read Segment Start Address (EDLM Version)\n");
		printf("      20) Exit This Sub Menu\n\n");
		printf("      ? : ");
		scanf("%d",&selection);
		printf("\n");

		switch(selection)
		{
			case 5 :
			{
				new_read_pos_id();
				break;
			}

			case 3 :
			{
				write_insert();
				break;
			}

			case 4 :
			{
				write_spec_addr();
				break;
			}

			case 1 :
			{
				read_spec_addr();
				break;
			}

			case 0 :
			{
				old_read_pos_id();
				break;
			}

			case 2 : 
			{
				read_many_addr();
				break;
			}

			case 6 : /*old read segment start address*/
			{
				read_old_start();
				break;
			}

			case 7 : /*EDLM read segment start address*/
			{
				read_new_start();
				break;
			}

			case 20 : break;

			default : printf("Invalid Selection\n");
			
		}
	}
}



void seg_menu()
{
int selection = 0;

	while(selection != 20)
	{
		printf("\n\nSegment Operations Menu: Enter Selection\n\n");
		printf("      0) Open Segment\n");
		printf("      1) Close Segment\n");
		printf("      2) Append Segment\n");
		printf("      3) Clear Segment\n");
		printf("      4) Delete Segment\n");
		printf("      20) Exit This Sub Menu\n\n");
		printf("      ? : ");

		scanf("%d",&selection);
		printf("\n");

		switch(selection)
		{
			case 0 :
			{
				open_seg();
				break;
			}

			case 1 :
			{
				close_seg();
				break;
			}

			case 3 : /*clear segment*/
			{
				clear_seg();
				break;
			}


			case 2 : /*append segment*/
			{
				append_seg();
				break;
			}

			case 4 : /*delete seg*/
			{
				delete_seg();
				break;
			}


			case 20 : break;

			default : printf("Invalid Selection\n");
		}
	}
}



void sub_menu()
{
int selection = 0;

	while(selection != 20)
	{
		printf("\n\nSubroutine Operations Menu: Enter Selection\n\n");
		printf("      0) Open Subroutine\n");
		printf("      1) Close Subroutine\n");
		printf("      2) Append Subroutine\n");
		printf("      3) Clear Subroutine\n");
		printf("      4) Delete Subroutine\n");
		printf("      20) Exit This Sub Menu\n\n");
		printf("      ? : ");

		scanf("%d",&selection);
		printf("\n");

		switch(selection)
		{
			case 0 :
			{
				open_sub();
				break;
			}

			case 1 :
			{
				close_sub();
				break;
			}


			case 3 : /*clear sub*/
			{
				clear_sub();
				break;
			}

			case 2 :  /*append sub*/
			{
				append_sub();
				break;
			}


			case 4 : /*delete sub*/
			{
				delete_sub();
				break;
			}


			case 20 : break;

			default : printf("Invalid Selection\n");
		}
	}
}



void mess_mon()
{
int selection;

mess_again:
		printf("\n\nMessage Monitor Menu: Enter Selection\n\n");
		printf("      0) Enter Message Monitor\n");
		printf("      1) Exit Message Monitor\n\n");
		printf("      ? : ");

		scanf("%d",&selection);
		printf("\n");

		switch(selection)
		{
			case 0 : /*Enter Message Monitor*/
			{
				fprintf(out_ptr,"Enter Message Monitor\n");
				megatek_out(0x10);
				megatek_out(0x0d);
				break;
			}		


			case 1 : /*Exit Message Monitor*/
			{
				fprintf(out_ptr,"Exit Message Monitor\n");
				megatek_out(0x1b);
				megatek_out('1');
				break;
			}

			default :
			{
			 	printf("Invalid Selection\n");
				goto mess_again;
			}
		}
}



void text_graphics()
{
int selection;

text_again:
		printf("\n\nText/Graphics Mode Menu: Enter Selection\n\n");
		printf("      0) Enter Graphics Mode\n");
		printf("      1) Exit Graphics Mode\n\n");
		printf("      ? : ");

		scanf("%d",&selection);
		printf("\n");

		switch(selection)
		{
			case 0 : /*enter graphics mode*/
			{
				fprintf(out_ptr,"Enter Graphics Mode\n");
				megatek_out(0x1d);
				break;
			}

			case 1 : /*exit graphics mode*/
			{
				fprintf(out_ptr,"Exit Graphics Mode\n");
				megatek_out(0x1f);
				break;
			}

			default :
			{ 
				printf("Invalid Selection\n");
				goto text_again;
			}
		}
}




void stat_show()
{
int selection;

show_again:
		printf("\n\nShow/No Show Data Menu: Enter Selection\n\n");
		printf("      0) Show Host & Megatek Data\n");
		printf("      1) Don't Show Data\n\n");
		printf("      ? : ");

		scanf("%d",&selection);
		printf("\n");

		switch(selection)
		{
			case 0 : /*show data*/
			{
				show_flag = 't';
				break;
			}


			case 1 : /*no show*/
			{
				show_flag = 'f';
				break;
			}

			default :
			{ 
				printf("Invalid Selection\n");
				goto show_again;
			}
		}
}




void com_menu()
{
int selection;
char ch;

com_again:
		printf("\n\nCom File Menu: Enter Selection\n\n");
		printf("      0) Open A Com File & Write To It\n");
		printf("      1) Close The Open Com File\n\n");
		printf("      ? : ");

		scanf("%d",&selection);
		printf("\n");

		switch(selection)
		{
			case 0 : /*open com file & write to it*/
			{
				if(com_ptr == NULL)
				{
					printf("Enter The Com File's Name: ");
					ch = getchar();
					scanf("%s",a_file);
					printf("Write Down The Com File's Name = %s\n",
						a_file);
					fprintf(out_ptr,"Open Com File: %s\n",
						a_file);

					com_ptr = fopen(a_file,"w");
				}
				else
					printf("A Com File Is Already Open!!, Ignored!\n");

				break;
			}

			case 1 : /*close com file*/
			{
				if(com_ptr != NULL)
				{
					printf("Closing Com File: %s\n",a_file);
					fprintf(out_ptr,"Closing Com File: %s\n",
						a_file);
					fclose(com_ptr);
					com_ptr = NULL;
				}
				else
					printf("No Open Com File!!, Ignored!!\n");

				break;
			}

			default :
			{ 
				printf("Invalid Selection\n");
				goto com_again;
			}
		}
}



main()
{
int selection = 0;
int i,data_char,ch,baud_rate,k;

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

			ugout(data_char);
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

			megatek_out(data_char);
		}



	}
	else
	{
	ch = getchar();
	selection = 0;

	while(selection != 20)
	{
		printf("\n\nEnter Selection\n");
		printf("  0) Memory Operations (Read Pos. ID, Read Mem.,Write Mem.,Read Many Addresses)\n");
		printf("  1) Segment Operations\n");
		printf("  2) Subroutine Operations\n");
		printf("  3) Message Monitor\n");
		printf("  4) Text/Graphics Mode\n");
		printf("  5) Show/NO Show Host & Megatek Data\n");
		printf("  6) Com File Open/Close\n");
		printf("  7) Short Relative Vectors\n");
		printf("  8) Send Escape Followed By a Charactor\n");
		printf("  9) Send Keyboard Data To The Megatek\n");
		printf("  10) Set Baud Rate\n");
		printf("  11) System Resets\n");
		printf("  12) Compute Ext. Jump Address/Offset\n");
		printf("  13) Convert Negative Unsigned Integer To Signed Integer\n");
		printf("  20) Exit Main Menu\n");
		printf("  21) Exit This Program\n\n");

		printf("  ? : ");

		scanf("%d",&selection);
		printf("\n");

		switch(selection)
		{
			case 0 : /*memory operation*/
			{
				mem_menu();
				break;
			}


			case 1 : /*segment menu*/
			{
				seg_menu();
				break;
			}


			case 2 : /*subroutine menu*/
			{
				sub_menu();
				break;
			}


			case 3 : /*message monitor*/
			{
				mess_mon();
				break;
			}


			case 4 : /*text/graphics*/
			{
				text_graphics();
				break;
			}


			case 5 : /*show/no show data*/
			{
				stat_show();
				break;
			}


			case 6 : /*com files*/
			{
				com_menu();
				break;
			}

			case 7 :
			{
				short_rel();
				break;
			}


			case 8 : /*send esc followed by a char to megatek*/
			{
				ch = getchar();
				printf("Enter The Charactor That Follows The Escape: ");
				ch = getchar();
				megatek_out(0x1b);
				megatek_out(ch);

				break;
			}

			case 9 : /*send keyboard input to megatek*/
			{
				ch = getchar();

				printf("Type On The The Keyboard & It Will Go To The Megatek.\n");
				printf("Type a tilde (~) to stop\n");

				ch = ' ';
				while(ch != '~')
				{
					ch = getchar();
					megatek_out(ch);
				}

				break;
			}


			case 10 : /*Enter baud rate*/
			{
		baud_again:
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
				{
					printf("\nInvalid baud rate indicator: %d\n",
							baud_rate);
					goto baud_again;
				}

				break;
			}


			case 11 : /*system resets*/
			{
				resets();
				break;
			}


			case 12 : /*compute extended jump address*/
			{
				jump_ext();
				break;
			}


			case 13 : /*convert negative unsigned to signed integer*/
			{
				printf("Enter Unsigned Negative Integer: ");
				scanf("%u",&k);
				printf("Signed Version = %d\n",k);
				break;
			}


			case 20 : break;

			case 21 : goto my_end;


			default : printf("\nInvalid Selection: %d\n",selection);
		}


/*		printf("\nHit Enter Key To Continue....\n");
		ch = getchar();
		ch = getchar();
*/
	}
	}
    }

my_end:
	fclose(out_ptr);

	if(com_ptr != NULL)
		fclose(com_ptr);
}

