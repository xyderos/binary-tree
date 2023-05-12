#pragma once

#include <pthread.h>

#define M 4

typedef struct binary_tree_entry {
	char *key, *val;
	struct binary_tree_node *next;
} binary_tree_entry_t;

typedef struct binary_tree_node {
	int m;
	char pad[sizeof(binary_tree_entry_t *) - sizeof(int)];
	binary_tree_entry_t *children[M];
} binary_tree_node_t;

typedef struct binary_tree {
	binary_tree_node_t *root;
	int height, n;
	pthread_mutex_t lock;
} binary_tree_t;

binary_tree_t *binary_tree_init(void);

void binary_tree_put(binary_tree_t *const, const char *const,
    const char *const);

char *binary_tree_get(const binary_tree_t *const, const char *const);
