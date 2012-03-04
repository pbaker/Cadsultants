

struct offset_values
{
	int x,y;
};



struct offset_values offset_table[3][5] = 
{
/* A */	 {
		{0,0},{0,0},{-4809,-4013},{-4741,-4013},{-4809,-4013}
	 },


/* B */  {
		{0,0},{0,0},{-7857,-5283},{-7513,-5283},{-7772,-5283}
	 },


/* C */  {
		{0,0},{0,0},{-10431,-8128},{-9889,-8128},{-10228,-8128}
	 }
};




main()
{
int row,col;

	for(row = 0 ; row < 3 ; row++)
	{
		printf("\n\nROW = %c\n\n",row + 'A');

		for(col = 0 ; col < 5 ; col++)
			printf("(%d,%d)  ",offset_table[row][col].x,
				offset_table[row][col].y);

	}
}