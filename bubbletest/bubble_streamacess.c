#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <math.h>



int main()
{

	

	long bw_stream_size;  // double type occupies 8 Bytes
	//double sizeofdouble;
	printf("bubble stream program");
	bw_stream_size = 1280 * 1024; //1M space
	double *bw_data;
	double *mid;
	int i,j;
	double scalar;
	scalar = 1.1;
	bw_data = malloc(bw_stream_size*sizeof(double));
	mid=bw_data+bw_stream_size/2;
	
	printf("smash space is 1280 * 8 KB\n");
	for(j=0;j<1000;j++)
	{
		for(i=0;i<bw_stream_size/2;i++)
		{
			bw_data[i] = scalar * mid[i];
		}
		for(i=0;i<bw_stream_size/2;i++)
		{
			mid[i] = scalar * mid[i];
		}

	}
	
	return 0;
}
