#include <time.h>
#include <stdio.h>

#include "./sizecalc/sizeCalc.h"

int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("Usage: %s <folder_path>\n", argv[0]);
		return 1;
	}

	char *folderPath = argv[1];

	clock_t begin = clock();

	MeasureSize(folderPath);

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Took time: %lf", time_spent);

	return 0;
}
