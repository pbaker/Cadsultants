

#define SUB_HOLDER_DL 		0
#define SUB_HOLDER_MEGATEK 	1
#define SUB_HOLDER_BLT 		2



struct sub_holder
{
	short color_sub_table[50][10];
	short sub_calling_table[64][10];
	short our_versions_of_subs[10][100];
	short sub_display_lists[NUMBER_OF_SUBS][2*SUB_NUM_ELS];
	unsigned char my_ems_buffer[3][16390];
};

