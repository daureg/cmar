#include "cmar.h"

bool read_matrix(const char* name, mat* db, mat* labels, mat* features) {
	mat_t* matfp;
	size_t size = 0;
	matvar_t* var;
	matfp = Mat_Open(name, MAT_ACC_RDONLY);

	if (NULL == matfp) {
		fprintf(stderr,"Error opening MAT file %s.", name);
		return false;
	}

	var = Mat_VarRead(matfp, "db");
	if (NULL == var) {
		fprintf(stderr,"Error reading variable db.");
		Mat_Close(matfp);
		return false;
	}

	db->rows = var->dims[0];
	db->cols = var->dims[1];
	size = var->data_size * db->rows * db->cols;
	db->data = malloc(size);
	memcpy(db->data, var->data, size);
	Mat_VarFree(var);

	var = Mat_VarRead(matfp, "labels");
	if (NULL == var) {
		fprintf(stderr,"Error reading variable labels.");
		Mat_Close(matfp);
		return false;
	}

	labels->rows = var->dims[0];
	labels->cols = var->dims[1];
	size = var->data_size * labels->rows * labels->cols;
	labels->data = malloc(size);
	memcpy(labels->data, var->data, size);
	Mat_VarFree(var);

	var = Mat_VarRead(matfp, "features");
	if (NULL == var) {
		fprintf(stderr,"Error reading variable features.");
		Mat_Close(matfp);
		return false;
	}
	features->rows = var->dims[0];
	features->cols = var->dims[1];
	size = var->data_size * features->rows * features->cols;
	features->data = malloc(size);
	memcpy(features->data, var->data, size);
	Mat_VarFree(var);

	Mat_Close(matfp);
	return true;
}

fptree* init_tree(mat* features, int min_supp) {
	fptree* tree = malloc(sizeof(fptree));
	tree->nodes = 0;
	tree->pool = ms_create(sizeof(fpnode), 1<<17);
	tree->root.feature = -1;
	tree->root.support[0] = 0;
	tree->root.support[1] = 0;
	tree->root.parent = NULL;
	tree->root.next = NULL;
	int nb_features = features->rows;
	int feature, total_support;
	int* feats = features->data;
	int largest_feature = feats[nb_features+0];
	for (int i = 1; i < nb_features; i++) {
		if (feats[nb_features+i] > largest_feature)
			largest_feature = feats[nb_features+i];
	}
	printf("max:%d\n",largest_feature);
	tree->heads = (fphead*)malloc((largest_feature+1)*sizeof(fphead));
	tree->nb_heads = largest_feature+1;
	for (int i = 0; i < nb_features; i++) {
		tree->heads[i].feature = -1;
		feature = feats[nb_features+i];
		total_support = feats[i];
		if (total_support >= min_supp) {
			tree->heads[feature].feature = feature;
			tree->heads[feature].support[0] = feats[2*nb_features+i];
			tree->heads[feature].support[1] = total_support - tree->heads[feature].support[0];
			tree->heads[feature].list = NULL;
			//printf("%d: %d+%d=%d\n", feature, tree->heads[feature].support[0], tree->heads[feature].support[1], total_support);
		}
	}
	return tree;
}
void free_tree(fptree* tree) {
	free(tree->heads);
	ms_delete(tree->pool);
	free(tree);
}
size_t rewrite_transaction(mat* db, mat* features, int min_supp, int pos, int* transac) {
	int nb_features = features->rows;
	int* farray = features->data;
	size_t s = 0;
	int col, feature;
	memset(transac, -1, (db->cols)*sizeof(int));
	for (int i = 0; i < nb_features; i++) {
		feature = farray[nb_features+i];
		if (farray[i] < min_supp) {
			return s;
		}
		col = feature/MAX_UNIQUE_FEATURES_VALUES;
		if (((int*)db->data)[col*db->rows+pos] == feature) {
			transac[s]=feature;
			s++;
		}
	}
	return s;
}
fpnode* find_elsewhere(fpnode* start, fpnode* from) {
	while (start->parent != from) {
		start = start->next;
		if (start==NULL)
			return NULL;
	}
	return start;
}
void insert_item(fptree* tree, int* transac, int class, int trsize) {
	//printf("insert (%d, %d)\n",class, trsize);
	int f = 0;
	bool can_go_deeper = true;
	fpnode* node = &(tree->root);
	fpnode* down;
	fpnode* child;
	while (can_go_deeper) {
		node->support[class]++;
		if (f>=trsize) {
			return;
		}
		down = tree->heads[transac[f]].list;
		if (down==NULL)
			break;
		down = find_elsewhere(down, node);
		can_go_deeper = down != NULL && down->parent == node;
		if (can_go_deeper) {
			//printf("go deeper…\n");
			node = down;
			f++;
		}
	}
	//printf("reach level %d, time to insert\n", f);
	for (; f < trsize; f++) {
		child =ms_alloc(tree->pool);
		tree->nodes++;
		child->feature = transac[f];
		child->parent = node;
		child->support[0] = child->support[1] = 0;
		child->support[class]++;
		child->next = tree->heads[transac[f]].list;
		tree->heads[transac[f]].list = child;
		//printf("create %d --> %d\n", node->feature, child->feature);
		node = child;
	}
}
static void indent (int k)
{ while (--k >= 0) printf("   "); }
void fpt_show (fptree *fpt, int ind)
{                               /* --- show a freq. pattern tree */
  int    i, k;                  /* loop variable */
  fpnode *node;                 /* to traverse the node lists */

  printf("\n"); indent(ind); printf("------nb nodes:%d\n",fpt->nodes);
  indent(ind);                  /* indent the output line */
  printf("* (%d,%d)\n", fpt->root.support[0], fpt->root.support[1]);
  int heads[] = {20, 31, 10, 4, 3, 11};
  for (int i = 1; i < fpt->nb_heads; i++) {  /* traverse the items */
	  if (fpt->heads[i].feature <1)
		  continue;
    indent(ind);                /* indent the output line */
    k = fpt->heads[i].feature;     /* print item and support */
    if (k < 0) printf("%04x", k & 0xffff);
    else       printf("%d", k);
    printf("(%d,%d):", fpt->heads[i].support[0], fpt->heads[i].support[1]);
    for (node = fpt->heads[i].list; node; node = node->next) {
      printf(" %d,%d", node->support[0], node->support[1]);/* print the node support */
      k = node->parent->feature;     /* get the parent item/list */
      if (k < 0) printf("[*]"); /* print a root node indicator */
      else { k = fpt->heads[k].feature;
             printf("[%d]", k); }
    }                           /* print the item in the parent */
    printf("\n");               /* terminate line after each node */
  }
  indent(ind); printf("------\n");
}  /* fpt_show() */
