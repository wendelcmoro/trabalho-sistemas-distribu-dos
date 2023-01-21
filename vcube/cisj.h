#ifndef _cisj_
#define _cisj_

#define POW_2(num) (1<<(num))
#define VALID_J(j, s) ((POW_2(s-1)) >= j)

/* |-- node_set.h */
typedef struct node_set {
	int* nodes;
	size_t size;
	size_t offset;
} node_set;

node_set* set_new(ssize_t size);
void set_insert(node_set* nodes, int node);
void set_merge(node_set* dest, node_set* source);
void set_free(node_set* nodes);
node_set* cis(int i, int s);

#endif