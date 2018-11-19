#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct node{
	int data;
	struct node *prev; // or parent
	struct node *left;
	struct node *right;
};

struct node* alloc(int x, struct node *p)
{
	struct node *tmp;

	tmp = (struct node*)malloc(sizeof(struct node));
	tmp->data = x;
	tmp->prev = p;
	tmp->left = NULL;
	tmp->right = NULL;

	return tmp;
}

bool is_left(struct node *curr)
{
	if (curr == NULL) {
		
		return false;
	}

	if (curr->prev == NULL) {

		return false;
	}

	if (curr->prev->left == curr) {

		return true;
	}

	return false;
}

void zag(struct node *curr, struct node **root) // single left rotation
{
	struct node *tmp;

	tmp = NULL;

	if (curr->right != NULL) {
		tmp = curr->right->left;
	}

	if (curr->prev != NULL) {
		if(is_left(curr) == true) {
			curr->prev->left = curr->right;
			curr->prev->left->prev = curr->prev;
			curr->prev = curr->prev->left;
		} else {
			curr->prev->right = curr->right;
			curr->prev->right->prev = curr->prev;
			curr->prev = curr->prev->right;
		}
		curr->prev->left = curr;
	} else {
		*root = curr->right;
		curr->prev = curr->right;
		curr->prev->left = curr;
		curr->prev->prev = NULL;
	}

	curr->right = tmp;
	if (curr->right != NULL) {
		curr->right->prev = curr;
	}

	return;
}

void zig(struct node *curr, struct node **root) // single right rotation
{
	struct node *tmp;
	struct node *l;

	tmp = NULL;
	l = curr->left;

	if (l != NULL) {
		tmp = l->right;
	}

	l->prev = curr->prev;

	if (curr->prev != NULL) {
		if (is_left(curr) == true) {
			l->prev->left = l;
		} else {
			l->prev->right = l;
		}
	} else {
		*root = l;
	}

	l->right = curr;
	curr->prev = l;

	curr->left = tmp;
	if (tmp != NULL) {
		tmp->prev = curr;
	}

	return;
}

void zig_zig(struct node *curr, struct node **root)
{
	zig(curr->prev->prev, root);
	zig(curr->prev, root);

	return;
}

void zag_zag(struct node *curr, struct node **root)
{
	zag(curr->prev->prev, root);
	zag(curr->prev, root);

	return;
}

void zig_zag(struct node *curr, struct node **root)
{
	zig(curr->prev, root);
	zag(curr->prev, root);

	return;
}

void zag_zig(struct node *curr, struct node **root)
{
	zag(curr->prev, root);
	zig(curr->prev, root);

	return;
}

void splay(struct node *curr, struct node **root)
{
	while (curr->prev != NULL) { // when curr->prev is NULL, it is root
		if (curr->prev->prev != NULL) {
			if (is_left(curr) == true) {
				if (is_left(curr->prev) == true) {
					zig_zig(curr, root);
				} else {
					zig_zag(curr, root);
				}
			} else {
				if (is_left(curr->prev) == true) {
					zag_zig(curr, root);
				} else {
					zag_zag(curr, root);
				}
			}
		} else if (curr->prev != NULL) {
			if (is_left(curr) == true) {
				zig(curr->prev, root);
			} else {
				zag(curr->prev, root);
			}
		}
	}

	return;
}

void insert(struct node **root, int x)
{
	if (*root == NULL) {
		*root = alloc(x, NULL);

		return;
	}

	struct node *curr;
	struct node *prev;

	curr = *root;
	prev = NULL;
	
	while (curr != NULL) {
		prev = curr;
		if (x == curr->data) {

			return;
		}
		if (x < curr->data) {
			curr = curr->left;
		} else {
			curr = curr->right;
		}
	}
	
	if (x < prev->data) {
		prev->left = alloc(x, prev);
		curr = prev->left;
	} else {
		prev->right = alloc(x, prev);
		curr = prev->right;
	}

	splay(curr, root);

	return;
}

void inorder_t(struct node *curr)
{
	if (curr == NULL) {
		return;
	}

	inorder_t(curr->left);
	printf("%d ", curr->data);
	inorder_t(curr->right);

	return;
}

void preorder_t(struct node *curr)
{
	if (curr == NULL) {
		return;
	}

	printf("%d ", curr->data);
	preorder_t(curr->left);
	preorder_t(curr->right);

	return;
}

bool find(struct node **root, int x) // finds and splays node(only if found)
{
	struct node *curr;

	curr = *root;

	while (curr != NULL) {
		if (curr->data == x) {
			splay(curr, root);

			return true;
		} else if (x < curr->data) {
			curr = curr->left;
		} else {
			curr = curr->right;
		}
	}

	return false;
}

struct node* inorder_pred(struct node *curr)
{
	if (curr->left == NULL) {
		
		return curr->prev;
	}
	
	struct node *t;

	t = curr->left;

	while (t->right != NULL) {
		t = t->right;
	}

	return t;
}

struct node* inorder_succ(struct node *curr)
{
	if (curr->right == NULL) {
		
		return NULL;
	}

	struct node *t;

	t = curr->right;

	while (t->left != NULL) {
		t = t->left;
	}

	return t;
}

int delete_root(struct node **root)
{
	int x;
	struct node *r; // replacement node

	if (((*root)->left == NULL) && ((*root)->right == NULL)) {
		x = (*root)->data;
		r = (*root);
		(*root) = NULL;
		free(r);

		return x;
	}

	r = inorder_succ(*root);

	if (r != NULL) {
		if (is_left(r) == true) {
			r->prev->left = NULL;
		} else {
			r->prev->right = NULL;
		}
	} else {
		r = inorder_pred(*root);
		if (r == (*root)->prev) {
			(*root)->prev = (*root)->right; // since there was no left child
		} else {
			if (is_left(r) == true) {
				r->prev->left = NULL;
			} else {
				r->prev->right = NULL;
			}
		}
	}

	x = (*root)->data;
	(*root)->data = r->data;

	free(r);

	return x;
}

bool delete(int x, struct node **root)
{
	if (*root == NULL) {
		printf("\nCannot delete empty tree...\n\tExiting!");
		exit(1);
	}

	if (find(root, x) == true) { 
		// placeholder
		delete_root(root);

		return true;
	}

	return false;
}

int main()
{
	int n;
	int x;
	int i;
	struct node *root;

	root = NULL;

	printf("\nEnter number of elements in splay tree: ");
	scanf("%d", &n);
	printf("\nEnter the elements: ");

	for (i = 0; i < n; ++i) {
		scanf("%d", &x);
		insert(&root, x);
	}
	printf("\nInorder: ");
	inorder_t(root);
	printf("\nPreorder: ");
	preorder_t(root);
	printf("\n");
	
	printf("\nNumber of find queries: ");
	scanf("%d", &n);

	for (i = 0; i < n; ++i) {
		scanf("%d", &x);
		if (find(&root, x) == true) {
			printf("\tFound!\n");
		} else {
			printf("\tNot found!\n");
		}

		printf("\nInorder: ");
		inorder_t(root);
		printf("\nPreorder: ");
		preorder_t(root);
		printf("\n");
	}
/* Deletion function not completely ready yet
	printf("\nNumber of deletions: ");
	scanf("%d", &n);

	for (i = 0; i < n; ++i) {
		scanf("%d", &x);
		if (delete(x, &root) == true) {
			printf("\nDeleted!\n");
		} else {
			printf("\nElement not found!\n");
		}
		printf("\nInorder: ");
		inorder_t(root);
		printf("\nPreorder: ");
		preorder_t(root);
		printf("\n");

	}
*/
	return 0;
}
