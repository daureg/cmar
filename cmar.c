#include "cmar.h"

bool read_matrix(const char* name) {
	mat_t* matfp;
	matvar_t *data;
	matfp = Mat_Open(name, MAT_ACC_RDONLY);

	if (NULL == matfp) {
		fprintf(stderr,"Error opening MAT file %s", name);
		return EXIT_FAILURE;
	}

	data = Mat_VarRead(matfp, "data_filled");
	if (NULL == data) {
		fprintf(stderr,"Error reading variable %s", "data_filled");
		Mat_Close(matfp);
	}
	printf("%s: %dx%d of %d\n", data->name, data->dims[0],data->dims[1], data->data_size);
	size_t cols=data->dims[1];
	double* array = data->data;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 3; j++) {
			printf("%lf\t", array[i*cols+j]);
		}
		printf("\n");
	}
	Mat_VarFree(data);

	Mat_Close(matfp);
	return EXIT_SUCCESS;
}
