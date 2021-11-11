#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "RB_tree.h"

#define MCOO 10000	//max count of orders
#define MLOS 50		//max lenght of string

int number_of_trade = 0;
FILE *f_out;

typedef struct f_order {
	int 	oid;
	char 	side;
	int 	qty;
	float	price;
} t_order;

char **ft_split(char *s, char sep);
//split string and return array of words which seppareted by sepparator sep

void print_order(t_order *order);
//output of order (for test)

void free_arr_of_strs(char **str);
//free memory for file reading 

void *line_handle(char *str); 
//read request and return pointer on Order OR Cancel OID

int correct_length(char *s, int len);
//return 1 if '\n' in str else return 0

void output_trade(char side, int oid1, int oid2, int qty, float price);
//print in file "my_output.txt"

int make_trade_B(t_order *order, rb_tree **find_root, float (*cmp)(void *, void *));
/********************************************************************
*handle Buy order													*
*find node with price that less or equal new orders price in tree	*
*search : O(log N) / del : O(log N)									*
********************************************************************/

int make_trade_S(t_order *order, rb_tree **find_root, float (*cmp)(void *, void *));
/********************************************************************
*handle Sell order													*
*find max node with min OID in tree  								*
*search : O(log N) / del : O(log N)									*
********************************************************************/

void handle_order(t_order *order, rb_tree **add_root, rb_tree **find_root, \
					t_order **hashmap, float (*cmp)(void *, void *), char flag);
int	cancel_order(rb_tree **root, t_order *order, float (*cmp)(void *, void *));
//return 1 if order was dellited from tree, 0 else 

void read_file(char *f1, char *f2)
{
	char	buf[MLOS + 1];				//lets think that maxlen of order string in file(MLOS) = 50
	char	cor_len;					//flag for checking lenght of input str
	rb_tree *s_root = create_root();	//tree of SELL orders
	rb_tree *b_root = create_root();	//tree of BUY orders
	t_order	**hashmap;					//max count of orders(MCOO) - 10000 : array of orders 
	FILE *fd = fopen(f1, "r");
	f_out = fopen(f2, "w");
	if (!f_out) {
		perror(f2);
		exit(1);
	}

	if (!fd) {
		perror(f1);
		exit(1);
	} else {
		t_order *order;
		hashmap = malloc(sizeof(t_order) * MCOO + 1);
		if (!hashmap) {
			fprintf(stderr, "not enough memory\n");
			exit(2);
		}
		for (int i = 0; i <= MCOO; ++i)
			hashmap[i] = NULL;

		while (fgets(buf, sizeof(buf), fd)) {
			cor_len = correct_length(buf, sizeof(buf));
			if (buf[0] == 'O' && cor_len) {
				order = line_handle(buf);
				if (order->side == 'S') 
					//handle Sell orders
					handle_order(order, &s_root, &b_root, hashmap, &ft_cmp_increase, order->side);
				else
					//handle Buy orders
					handle_order(order, &b_root, &s_root, hashmap, &ft_cmp_increase, order->side);
			} else if (buf[0] == 'C' && cor_len) {
				//del order with this OID O(log N) : del from hashmap and tree
				int *id = line_handle(buf);
				//order with this id does not exist
				if (!hashmap[*id])
					continue ;

				if (cancel_order(&b_root, hashmap[*id], &ft_cmp_increase)){
					free(hashmap[*id]);
					hashmap[*id] = NULL;
				} else if (cancel_order(&s_root, hashmap[*id], &ft_cmp_increase)) {
					free(hashmap[*id]);
					hashmap[*id] = NULL;
				}
				free(id);
			} else {
				fprintf(stderr, "line incorrect\n");
				continue ;
			}
		}
		printf("SELL tree:\n");
		print_tree(s_root, 1);
		printf("\n");
		printf("BUY tree:\n");
		print_tree(b_root, 1);
	}
	if (fclose(fd)) { 
		fprintf(stderr, "file not closed\n");
		return ;
	}
}

int main(int argc, char **argv)
{
	if (argc == 2) {
		read_file(argv[1], "my_output.txt");
	}
	return 0;
}

int	cancel_order(rb_tree **root, t_order *order, float (*cmp)(void *, void *))
{
	if (find_node(*root, (void *)order, cmp)) {
		fprintf(f_out, "X,%d\n", order->oid);
		rb_tree_delete(root, (void *)order, cmp);
		return 1;
	}
	return 0;
}

void free_arr_of_strs(char **str)
{
	int i = 0;
	while (str[i])
		free(str[i++]);
	free(str);
}

void *line_handle(char *str)
{
	char sep = ',';
	int count = 0;
	char **arr_of_strs = ft_split(str, sep);
	while (arr_of_strs[count++])
		;
	if (arr_of_strs[0][0] == 'O' && count - 1 == 5) {
		//order 
		t_order *order = malloc(sizeof(order));
		order->oid = atoi(arr_of_strs[1]);
		order->side = arr_of_strs[2][0];
		order->qty = atoi(arr_of_strs[3]);
		order->price = atof(arr_of_strs[4]);
		free_arr_of_strs(arr_of_strs);
		return order;
	} else if (arr_of_strs[0][0] == 'C' && count - 1 == 2) {
		//cancel
		int *id = malloc(sizeof(int));
		*id = atoi(arr_of_strs[1]);
		return id;
	}
	return NULL;
}

void print_order(t_order *order)
{
	if (!order)
		return ;
	printf("Order : ID - %d\n", order->oid);
	printf("        Side - %c\n", order->side);
	printf("        QTY - %d\n", order->qty);
	printf("        Price - %.2f\n", order->price);
}

int correct_length(char *s, int len)//return 1 if '\n' in str else return 0
{
	int i = 0;
	while (s[i] && i < len) {
		if (s[i] == '\n')
			return 1;
		i++;
	}
	return 0;
}

void output_trade(char side, int oid1, int oid2, int qty, float price)
{
	fprintf(f_out, "T,%d,%c,%d,%d,%d,%.2f\n", ++number_of_trade, side, oid1, oid2, qty, price);
}

int make_trade_B(t_order *order, rb_tree **find_root, float (*cmp)(void *, void *))
{
	rb_tree *cmp_node;
	t_order *in_tree;
		do {
		cmp_node = find_node_min(*find_root);
		if (cmp_node) {
			in_tree = cmp_node->data;
			if (in_tree->price <= order->price) {
				if (order->qty < in_tree->qty) {
					in_tree->qty -= order->qty;
					output_trade('S', in_tree->oid, order->oid, order->qty, in_tree->price);
					order->qty = 0;
				} else {
					output_trade('S', in_tree->oid, order->oid, in_tree->qty, in_tree->price);
					order->qty -= in_tree->qty;
					rb_tree_delete(find_root, in_tree, cmp);
				}
			}
		}
	} while (order->qty && cmp_node && in_tree->price <= order->price);
	if (order->qty) 
		return 1;
	return 0;
}

int make_trade_S(t_order *order, rb_tree **find_root, float (*cmp)(void *, void *))
{
	rb_tree *cmp_node;
	t_order *in_tree;
		
		do {
		cmp_node = find_max_with_min_id(*find_root); //O(log N)
		if (cmp_node) {
			in_tree = cmp_node->data;
			if (in_tree->price >= order->price) {
				if (order->qty < in_tree->qty) {
					in_tree->qty -= order->qty;
					output_trade('B', in_tree->oid, order->oid, order->qty, in_tree->price);
					order->qty = 0;
				} else {
					output_trade('B', in_tree->oid, order->oid, in_tree->qty, in_tree->price);
					order->qty -= in_tree->qty;
					rb_tree_delete(find_root, in_tree, cmp);
				}
			}
		}
	} while (order->qty && cmp_node && in_tree->price >= order->price);
	if (order->qty) 
		return 1;
	return 0;
}

void handle_order(t_order *order, rb_tree **add_root, rb_tree **find_root, \
					t_order **hashmap, float (*cmp)(void *, void *), char flag)
{
	char need_to_add = 1;
	if (hashmap[order->oid]) {
		printf("order with id %d already exist\n", order->oid);
		return ;
	}
	if (flag == 'S')
		need_to_add = make_trade_S(order, find_root, cmp);
	else if (flag == 'B')
		need_to_add = make_trade_B(order, find_root, cmp);
	if (need_to_add) {	//in BUY tree not exist new order 
		rb_tree_insert(add_root, (void *)order, cmp);
		hashmap[order->oid] = malloc(sizeof(t_order));
		hashmap[order->oid] = order;
	}
}
