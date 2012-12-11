#include "cmar.h"
int main(int argc,char** argv) {
	if (argc != 2)
		return EXIT_FAILURE;
	mat* db = malloc(sizeof(mat));
	mat* features = malloc(sizeof(mat));
	mat* labels = malloc(sizeof(mat));

	
	if (!read_matrix(argv[1], db, labels, features))
		return EXIT_FAILURE;
	for (int i = 0; i < 5; i++) {
		printf("%d\n", ((char*)labels->data)[i]);
	}
	int* array = db->data;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%d\t", array[j*db->rows+i]);
		}
		printf("\n");
	}

	fptree* t = init_tree(features, 20);
	printf("%d\n", t->heads[1291].support[1]);
	
	int* trans = malloc(db->cols*sizeof(int));
	if (trans==NULL) {
		return false;
	}
	size_t trsize;
	for (int i = 0; i <db->rows; i++) {
		trsize = rewrite_transaction(db, features, 20, i, trans);
		insert_item(t, trans, ((int_fast8_t*)labels->data)[i], trsize);
#if 0
		for (int j = 0; j < db->cols; j++) {
			printf("%d ",trans[j]);
		}
		printf("\n");
#endif
	}
	free(trans);
	//fpt_show(t, 0);
	free_tree(t);
	free(db->data); free(db);
	free(features->data); free(features);
	free(labels->data); free(labels);
	return EXIT_SUCCESS;
}
