#include <sys/time.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binary_tree.h"

static binary_tree_entry_t *
entry_init(const char *const key, const char *const val,
    binary_tree_node_t *next)
{
	binary_tree_entry_t *to_return = malloc(sizeof(binary_tree_entry_t));

	if (!to_return) {
		return NULL;
	}

	to_return->key = key;
	to_return->val = val;
	to_return->next = next;

	return to_return;
}

static binary_tree_node_t *
node_init(int m)
{
	binary_tree_node_t *to_return = malloc(sizeof(binary_tree_node_t));

	if (!to_return) {
		return NULL;
	}

	to_return->m = m;

	for (size_t i = 0; i < M; i++) {
		to_return->children[i] = entry_init("", "", NULL);
	}
	return to_return;
}

binary_tree_t *
binary_tree_init(void)
{
	binary_tree_t *to_return = malloc(sizeof(binary_tree_t));

	if (!to_return) {
		return NULL;
	}

	to_return->height = 0;
	to_return->n = 0;
	to_return->root = node_init(0);

	pthread_mutex_init(&to_return->lock, NULL);

	return to_return;
}

static binary_tree_node_t *
split(binary_tree_node_t *node)
{
	binary_tree_node_t *tmp = node_init(M / 2);

	node->m = M / 2;

	for (int j = 0; j < M / 2; j++) {
		tmp->children[j] = node->children[M / 2 + j];
	}

	return tmp;
}

static char *
search(binary_tree_node_t *x, const char *const key, int ht)
{
	binary_tree_entry_t *children[M];

	for (size_t i = 0; i < M; i++) {
		children[i] = x->children[i];
	}

	if (ht == 0) {

		for (int j = 0; j < x->m; j++) {
			if (key == children[j]->key) {
				return children[j]->val;
			}
		}
	}

	else {

		for (int j = 0; j < x->m; j++) {
			if (j + 1 == x->m ||
			    strcmp(key, children[j + 1]->key) < 0) {
				return search(children[j]->next, key, ht - 1);
			}
		}
	}

	return NULL;
}

char *
binary_tree_get(const binary_tree_t *const binary_tree, const char *const key)
{
	return key ? search(binary_tree->root, key, binary_tree->height) : NULL;
}

static binary_tree_node_t *
insert(binary_tree_node_t *node, const char *const key, const char *const val,
    int ht)
{
	int j;

	binary_tree_entry_t *entry = entry_init(key, val, NULL);

	if (ht == 0) {

		for (j = 0; j < node->m; j++) {
			if (strcmp(key, node->children[j]->key) < 0) {
				break;
			}
		}
	}

	else {

		for (j = 0; j < node->m; j++) {
			if ((j + 1 == node->m) ||
			    strcmp(key, node->children[j + 1]->key) < 0) {

				binary_tree_node_t *u =
				    insert(node->children[j++]->next, key, val,
					ht - 1);

				if (!u) {
					return NULL;
				}

				entry->key = u->children[0]->key;
				entry->next = u;

				break;
			}
		}
	}

	for (int i = node->m; i > j; i--) {
		node->children[i] = node->children[i - 1];
	}

	node->children[j] = entry;
	node->m++;

	if (node->m < M) {
		return NULL;
	} else {
		return split(node);
	}
}

void
binary_tree_put(binary_tree_t *const btree, const char *const key,
    const char *const val)
{
	binary_tree_node_t *u = NULL, *t = NULL;

	if (!key) {
		return;
	}

	u = insert(btree->root, key, val, btree->height);

	btree->n++;

	if (!u) {
		return;
	}

	t = node_init(2);
	t->children[0] = entry_init(btree->root->children[0]->key, NULL,
	    btree->root);
	t->children[1] = entry_init(u->children[0]->key, NULL, u);
	btree->root = t;
	btree->height++;
}
