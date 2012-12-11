#ifndef __CMAR
#define __CMAR

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <matio.h>
#include "memsys.h"
static const unsigned short int MAX_UNIQUE_FEATURES_VALUES = 430;
typedef struct _mat {
	int rows;
	int cols;
	void* data;
} mat;
typedef struct _fpnode {
	int feature;
	int support[2];
	struct _fpnode* next;
	struct _fpnode* parent;
} fpnode;
typedef struct _fphead {
	int feature;
	int support[2];
	fpnode* list;
} fphead;
typedef struct _fptree {
	int nodes;
	MEMSYS* pool;
	fpnode root;
	fphead* heads;
	int nb_heads;
} fptree;
bool read_matrix(const char* name, mat* db, mat* labels, mat* features);
fptree* init_tree(mat* features, int min_supp);
void free_tree(fptree* tree);
size_t rewrite_transaction(mat* db, mat* features, int min_supp, int pos, int* transac);
void insert_item(fptree* tree, int* transac, int class, int trsize);
void fpt_show (fptree *fpt, int ind);

#endif
