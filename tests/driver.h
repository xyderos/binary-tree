#include <sys/time.h>

#include <stdio.h>

#define ONE_MILLION 1000000

#include <check.h>
#include <stdlib.h>

#include "../src/binary_tree.h"

typedef struct args {
	binary_tree_t *btree;
	int threads;
	char pad[sizeof(binary_tree_t *) - sizeof(int)];
} args_t;

static void *
thread_function(void *args)
{
	args_t *m = (args_t *)args;

	char *urls[] = { "www.cs.princeton.edu", "www.princeton.edu",
		"www.yale.edu", "www.simpsons.com", "www.apple.com",
		"www.amazon.com", "www.ebay.com", "www.cnn.com",
		"www.google.com", "www.nytimes.com", "www.microsoft.com" };

	pthread_mutex_lock(&m->btree->lock);
	for (int i = 0; i < 100 / m->threads; i++) {
		binary_tree_put(m->btree, urls[i % 11], "128.112.136.12");
	}

	pthread_mutex_unlock(&m->btree->lock);

	pthread_exit(EXIT_SUCCESS);
}

START_TEST(BENCH_BINARY_TREE)
{
	for (int i = 1; i < 11; i++) {
		binary_tree_t *tree = binary_tree_init();
		int s = 0;
		args_t args;
		args.btree = tree;
		args.threads = i;
		pthread_t *threads = malloc((size_t)i * sizeof(pthread_t));
		if (!threads) {
			return;
		}

		struct timeval start, end;
		s = gettimeofday(&start, NULL);
		if (s) {
			return;
		}
		for (int j = 0; j < i; j++) {
			pthread_create(&threads[j], NULL, &thread_function,
			    &args);
		}
		for (int k = 0; k < i; k++) {
			pthread_join(threads[k], NULL);
		}
		s = gettimeofday(&end, NULL);
		if (s) {
			return;
		}
		long long startusec, endusec;
		startusec = start.tv_sec * ONE_MILLION + start.tv_usec;
		endusec = end.tv_sec * ONE_MILLION + end.tv_usec;
		printf("%d threads\n", i);
		printf("Time (seconds): %f\n\n",
		    ((double)(endusec - startusec) / ONE_MILLION));
		printf("size: %d\n\n", tree->n);
		free(threads);
	}
}
END_TEST
