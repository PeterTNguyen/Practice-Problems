#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( int argc, char *argv[] )
{
	
	uint32_t N, a, b, switchCount = 0;
	int i, j;
	int asdf;
	uint32_t * switches;

	//open file reader
	FILE *file = fopen( argv[1], "r" );
	if ( file == 0 )
    {
        printf( "Could not open file\n" );
    }
    else
    {
    	clock_t start = clock(), diff;	//start clock
    	fscanf(file, "%i", &N);	//Read first line for max range

    	//Allocate switches array
    	switches = (uint32_t*)malloc(N * sizeof(uint32_t));
    	for(i = 0 ; i < N; i++)
    		switches[i] = 0;

    	asdf = 0;
		while (fscanf(file, "%i %i", &a, &b) != EOF)	//Read lines til EOF
		{												//store inputs into a and b respectively
			printf("%i\n", asdf++);
			if(a > b)
			{
				a = a + b;
				b = a - b;
				a = a - b;
			}

			for(i = a; i <=b; i++)
				switches[i]++;
		}

		//Report final switch count
		for(i = 0; i < N; i++)
		{
			switchCount += switches[i] & 1;
		}
		printf("There are %i switches currently on.\n", switchCount);

		//Report Time taken
		diff = clock() - start;
		int msec = diff * 1000 / CLOCKS_PER_SEC;
		printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
    	fclose(file);
    }

	return 0;
}