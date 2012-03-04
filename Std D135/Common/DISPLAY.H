#define HAVE_DISP_FILE

struct a_word
{
	char op_code,a_char,color,font;
	int arg1,arg2,arg3,arg4;
};


#define SIZE_A_WORD sizeof(struct a_word)
