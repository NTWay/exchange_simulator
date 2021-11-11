#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct my_tree {
	struct my_tree *left;
	struct my_tree *right;
	struct my_tree *parent;
	void *data;
	char color; //red - 1, black - 0
} rb_tree;

typedef struct f_order {
	int 	oid;
	char 	side;
	int 	qty;
	float	price;
} t_order;

#define EPS 0.001
#define BLACK 0
#define RED 1
#define NIL &leaf
rb_tree leaf = {NIL, NIL, NULL, BLACK, 0};

rb_tree *create_root();
rb_tree *create_node(void *data);
void 	print_tree(rb_tree *root, int deep);
void	left_rotate(rb_tree **root, rb_tree *x);
void	right_rotate(rb_tree **root, rb_tree *x);
float 	ft_cmp_increase(void *o1, void *o2);
rb_tree *find_node_max(rb_tree *root);
rb_tree *find_node_min(rb_tree *root);
rb_tree *tree_successor(rb_tree *root);
rb_tree *tree_predecessor(rb_tree *root);
rb_tree *find_node(rb_tree *root, void *data, float (*cmp)(void *, void *));
rb_tree *find_min_id(rb_tree *root, void *data); //O(n) rewrite!!! Must be O(logN)
void	delete_node(rb_tree *node);
void	rb_insert_fixup(rb_tree **root, rb_tree *new);
void	rb_tree_insert(rb_tree **root, void *data, float (*cmp)(void *, void *));
void 	rb_transplant(rb_tree **root, rb_tree *u, rb_tree *v);
rb_tree *tree_min(rb_tree *root);
void	rb_delete_fixup(rb_tree **root, rb_tree *x);
void	rb_tree_delete(rb_tree **root, void *data, float (*ft_cmp)(void *, void *));
int 	rb_tree_black_hight(rb_tree *root);
int 	rb_tree_hight(rb_tree *root);



rb_tree *create_root()
{
	rb_tree *root = NIL;
	return root;
}

rb_tree *create_node(void *data)
{
	rb_tree *root = malloc(sizeof(rb_tree));
	if(!root)
		return NULL;
	root->data = data;
	root->parent= NIL;
	root->left = NIL;
	root->right = NIL;
	root->color = RED;
	return root;
}

void print_tree(rb_tree *root, int deep)
{
	int i;
	if (root != NIL && root != NULL) {
		print_tree(root->right, deep + 1);

		for(i = 1; i < deep; i++)
			printf("   ");
		t_order *data = root->data;
		if (root->color == RED)
			printf("%d %d %.2f RED\n", data->oid, data->qty, data->price);
		else
			printf("%d %d %.2f BLACK\n", data->oid, data->qty, data->price);

		print_tree(root->left, deep + 1);
	}
}

void	left_rotate(rb_tree **root, rb_tree *x)
{
	rb_tree *y = x->right;

	x->right = y->left;
	if (y->left != NIL)
		y->left->parent = x;
	y->parent = x->parent;
	if (x->parent == NIL)
		*root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;
	y->left = x;
	x->parent = y;
}

void	right_rotate(rb_tree **root, rb_tree *x)
{
	rb_tree *y = x->left;

	x->left = y->right;
	if (y->right != NIL)
		y->right->parent = x;
	y->parent = x->parent;
	if (x->parent == NIL)
		*root = y;
	else if (x == x->parent->right)
		x->parent->right = y;
	else
		x->parent->left = y;
	y->right = x;
	x->parent = y;
}

float ft_cmp_increase(void *o1, void *o2)//function to sort orders in increasing order
{
	t_order *d1 = o1, *d2 = o2;
	if (fabsf(d1->price - d2->price) < EPS)
		return d1->oid - d2->oid;
	return d1->price - d2->price;
}

rb_tree *find_min_id(rb_tree *root, void *data) //O(n) rewrite!!! Must be O(log N)
{
	rb_tree *res = NULL;
	t_order *key = data, *tmp_data = root->data;
	if (root && root != NIL) {
		if (root->left != NIL)
			res = find_min_id(root->left, data);
		if (!res && fabs(key->price - tmp_data->price) < EPS)
			res = root;
		if (!res && root->right != NIL)
			res = find_min_id(root->right, data);
	}
	return res;
}

rb_tree *find_max_with_min_id(rb_tree *root) //O(log N)
{
	rb_tree *max = find_node_max(root);
	if (!max)
		return NULL;
	rb_tree *left = tree_predecessor(max);
	if (left == NIL)
		return max;
	t_order *data_max = max->data, *data_left = left->data;
	while (left && fabs(data_left->price - data_max->price) < EPS) {
		max = left;
		left = tree_predecessor(max);
		if (left == NIL)
			left = NULL;
		else {
			data_max = max->data;
			data_left = left->data;
		}
	}
	return max;
}

rb_tree *find_node_max(rb_tree *root)
{
	rb_tree *max = root;
	while (max->right != NIL)
		max = max->right;
	if (max == NIL) {
		//printf("Tree is empty\n");
		return NULL;
	}
	return max;
}

rb_tree *find_node_min(rb_tree *root)
{
	rb_tree *min = root;
	while (min->left != NIL)
		min = min->left;
	if (min == NIL) {
		//printf("Tree is empty\n");
		return NULL;
	}
	return min;
}

rb_tree *tree_successor(rb_tree *root)
{
	if (root->right != NIL)
		return find_node_min(root->right);
	rb_tree *y = root->parent;
	while (y != NIL && root == y->right) {
		root = y;
		y = y->parent;
	}
	return y;
}

rb_tree *tree_predecessor(rb_tree *root)
{
	if (root->left != NIL)
		return find_node_max(root->left);
	rb_tree *y = root->parent;
	while (y != NIL && root == y->left) {
		root = y;
		y = y->parent;
	}
	return y;
}

rb_tree *find_node(rb_tree *root, void *data, float (*cmp)(void *, void *))
{
	rb_tree *z = root;
	while (z != NIL && cmp(data, z->data)) {
		if (cmp(data, z->data) < 0)
			z = z->left;
		else 
			z = z->right;
	}
	if (z == NIL) {
	//	printf("No such node\n");
		return NULL;
	}
	return z;
}

void	delete_node(rb_tree *node)
{
	//free(node->data); // free in read_from_file
	free(node);
}

void	rb_insert_fixup(rb_tree **root, rb_tree *new)
{
	rb_tree *uncle;
	while (new->parent->color == RED)
	{
		if ((new->parent == new->parent->parent->left)) { 
			/*if new on the left*/
			uncle = new->parent->parent->right;
			if (uncle && uncle->color == RED) {
				uncle->color = BLACK;
				uncle->parent->color = RED;
				new->parent->color = BLACK;
				new = new->parent->parent;
			} else if (new->parent->right == new) {
				new = new->parent;
				left_rotate(root, new);	
			} else {
				new->parent->color = BLACK;
				right_rotate(root, new->parent->parent);
				new->parent->right->color = RED;
			}
		} else { 
			/*new on the right side*/
			uncle = new->parent->parent->left;
			if (uncle && uncle->color == RED) {
				uncle->color = BLACK;
				uncle->parent->color = RED;
				new->parent->color = BLACK;
				new = new->parent->parent;
			} else if (new->parent->left == new) {
				new = new->parent;
				right_rotate(root, new);	
			} else {
				new->parent->color = BLACK;
				left_rotate(root, new->parent->parent);
				new->parent->left->color = RED;
			}
		}
		//else   no grandparent
	}
	(*root)->color = BLACK;
}

void	rb_tree_insert(rb_tree **root, void *data, float (*cmp)(void *, void *))
{
	rb_tree *x = *root;
	rb_tree *y = NIL;
	rb_tree *new = create_node(data);
	while (x != NIL) {
		y = x;
		if (cmp(new->data, x->data) < 0)
			x = x->left;
		else 
			x = x->right;
	}
	new->parent = y;
	if (y == NIL)  /*if new - root*/
		*root = new;
	else if ((*cmp)(new->data, y->data) < 0) /*if x not root*/
		y->left = new;
	else 
		y->right = new;
	new->left = NIL;
	new->right = NIL;
	new->color = RED;
	rb_insert_fixup(root, new);
}

void 	rb_transplant(rb_tree **root, rb_tree *u, rb_tree *v)
{
	if (u->parent == NIL)
		*root = v;
	else if (u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	v->parent = u->parent;
}

rb_tree *tree_min(rb_tree *root)
{
	while (root->left != NIL)
		root = root->left;
	return root;
}

void	rb_delete_fixup(rb_tree **root, rb_tree *x)
{
	rb_tree *w;
	while (x != *root && x->color == BLACK) {
		if (x == x->parent->left) {
			w = x->parent->right;//brother RED
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				left_rotate(root, x->parent);
				w = x->parent->right;
			}
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->parent;
			} else if (w->right->color == BLACK) {
				w->left->color = BLACK;
				w->color = RED;
				right_rotate(root, w);
				w = x->parent->right; 
			} else { //brothers right son - RED
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				left_rotate(root, x->parent);
				x = *root;
			}
		} else { //reverse
			w = x->parent->left;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				right_rotate(root, x->parent);
				w = x->parent->left;
			}
			if (w->right->color == BLACK && w->left->color == BLACK) {
				w->color = RED;
				x = x->parent;
			} else if (w->left->color == BLACK) {
				w->right->color = BLACK;
				w->color = RED;
				left_rotate(root, w);
				w = x->parent->left; 
			} else {
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				right_rotate(root, x->parent);
				x = *root;
			}
		}
	}
	x->color = BLACK;
}

void	rb_tree_delete(rb_tree **root, void *data, float (*ft_cmp)(void *, void *))
{
	rb_tree *z, *y, *x;
	z = find_node(*root, data, ft_cmp);
	y = z;
	char orig_color = y->color;
	if (z->left == NIL) {
		x = z->right;
		rb_transplant(root, z, z->right);
	} else if (z->right == NIL) {
		x = z->left;
		rb_transplant(root, z, z->left);
	}
	else {
		y = tree_min(z->right);
		orig_color = y->color;
		x = y->right;
		if (y->parent == z)
			x->parent = y;
		else {
			rb_transplant(root, y, y->right);
			y->right = z->right;
			y->right->parent = y;
			y->color = z->color;
		}
		rb_transplant(root, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	delete_node(z);
	if (orig_color == BLACK)
		rb_delete_fixup(root, x);
}

int rb_tree_black_hight(rb_tree *root)
{
	int count = 0;
	while (root->left != NIL) {
		if (root->color == BLACK)
			++count;
		root = root->left;
	}
	return count + 1; //+leaf
}

int rb_tree_hight(rb_tree *root)
{
	int count_r = 0, count_l = 0;
	if (root == NIL)
		return 1;
	count_r = rb_tree_hight(root->right) + 1;
	count_l = rb_tree_hight(root->left) + 1;
	return count_r > count_l ? count_r : count_l; //+leaf
}
