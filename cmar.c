#include "cmar.h"

bool read_matrix(const char* name) {
	mat_t* matfp;
	matfp = Mat_Open(name, MAT_ACC_RDONLY);

	if (NULL == matfp) {
		fprintf(stderr,"Error opening MAT file %s", name);
		return EXIT_FAILURE;
	}

	Mat_Close(matfp);
	return EXIT_SUCCESS;
}
