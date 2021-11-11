#ifndef RB_TREE_H
#define RB_TREE_H

typedef struct my_tree {
	struct my_tree *left;
	struct my_tree *right;
	struct my_tree *parent;
	void *data;
	char color; //red - 1, black - 0
} rb_tree;

rb_tree *create_root();
void print_tree(rb_tree *root, int deep);
float ft_cmp_increase(void *data1, void *data2);
rb_tree *find_node_max(rb_tree *root);
rb_tree *find_node_min(rb_tree *root);
rb_tree *find_min_id(rb_tree *root, void *data);
rb_tree *find_max_with_min_id(rb_tree *root);
void rb_tree_insert(rb_tree **root, void *data, float (*cmp)(void *, void *));
rb_tree *find_node(rb_tree *root, void *data, float (*cmp)(void *, void *));
void rb_tree_delete(rb_tree **root, void *data, float (*ft_cmp)(void *, void *));
int rb_tree_black_hight(rb_tree *root);
int rb_tree_hight(rb_tree *root);

#endif
