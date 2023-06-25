#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "microsat.h"

int solve(struct solver *S)
{ // Determine satisfiability
	int decision = S->head;
	S->res = 0; // Initialize the solver
	for (;;)
	{								  // Main solve loop
		int old_nLemmas = S->nLemmas; // Store nLemmas to see whether propagate adds lemmas
		if (propagate(S) == UNSAT)
			return UNSAT; // Propagation returns UNSAT for a root level conflict

		if (S->nLemmas > old_nLemmas)
		{						// If the last decision caused a conflict
			decision = S->head; // Reset the decision heuristic to head
			if (S->fast > (S->slow / 100) * 125)
			{ // If fast average is substantially larger than slow average
				//        printf("c restarting after %i conflicts (%i %i) %i\n", S->res, S->fast, S->slow, S->nLemmas > S->maxLemmas);
				S->res = 0;
				S->fast = (S->slow / 100) * 125;
				restart(S); // Restart and update the averages
				if (S->nLemmas > S->maxLemmas)
					reduceDB(S, 6);
			}
		} // Reduce the DB when it contains too many lemmas

		while (S->false[decision] || S->false[-decision])
		{ // As long as the temporay decision is assigned
			decision = S->prev[decision];
		} // Replace it with the next variable in the decision list
		if (decision == 0)
			return SAT;										  // If the end of the list is reached, then a solution is found
		decision = S->model[decision] ? decision : -decision; // Otherwise, assign the decision variable based on the model
		S->false[-decision] = 1;							  // Assign the decision literal to true (change to IMPLIED-1?)
		*(S->assigned++) = -decision;						  // And push it on the assigned stack
		decision = abs(decision);
		S->reason[decision] = 0;
	}
} // Decisions have no reason clauses

void swap_int_pointer(int **a, int **b)
{
	int *temp = *a;
	*a = *b;
	*b = temp;
}

void swap_int(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

int *perturb_clauses(struct problem *P)
{
	int *map_buff = (int *)malloc(sizeof(int) * P->nVars * 2 + 1);
	int *rmap_buff = (int *)malloc(sizeof(int) * P->nVars * 2 + 1);
	int *map = map_buff + P->nVars;
	int *rmap = rmap_buff + P->nVars;
	for (int i = 1; i <= P->nVars; i++)
	{
		map[i] = i;
		map[-i] = -i;
	}
	for (int i = 1; i <= 2 * P->nVars; i++)
	{
		int p = rand() % i + 1 - P->nVars;
		int q = i - P->nVars;
		if (p <= 0)
			p--;
		if (q <= 0)
			q--;
		swap_int(&map[q], &map[p]);
	}
	for (int i = 1; i <= P->nVars; i++)
	{
		rmap[map[i]] = i;
		rmap[map[-i]] = -i;
	}
	// printProblem(P);
	for (int i = 0; i < P->nClauses; i++)
		for (int j = 0; j < P->len[i]; j++)
			P->first[i][j] = map[P->first[i][j]];
	// printProblem(P);
	// for (int i = 0; i < P->nClauses; i++)
	// 	for (int j = 0; j < P->len[i]; j++)
	// 		P->first[i][j] = rmap[P->first[i][j]];
	// printProblem(P);
	// for (int i = 0; i < P->nClauses; i++)
	// {
	// 	for (int j = 1; j < P->len[i]; j++)
	// 	{
	// 		int p = rand() % (j + 1);
	// 		swap_int(&P->first[i][j], &P->first[i][p]);
	// 	}
	// }
	// for (int i = 1; i < P->nClauses; i++)
	// {
	// 	int p = rand() % (i + 1);
	// 	swap_int(&P->len[i], &P->len[p]);
	// 	swap_int_pointer(&P->first[i], &P->first[p]);
	// }
	free(map_buff);
	return rmap;
}

volatile int terminate_id = 0;
volatile int terminate_flag = 0;

void cleanup(void *arg)
{
	printf("Cleaning up thread %d\n", *((int *)arg));
}

void *thread_solve(void *arg)
{
	struct solver *T = (struct solver *)arg;
	int ans = solve(T);
	pthread_cleanup_push(cleanup, &T->solver_id);
	printf("Thread %d running\n", T->solver_id);
	T->result = ans;
	terminate_id = T->solver_id;
	terminate_flag = 1;
	printf("Thread %d terminated\n", T->solver_id);
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
}

int parallel_solve(struct problem *P)
{
	// for (int i = 0; i < S->mem_max && i < 1000; i++)
	// {
	// 	int p = S->DB[i];
	// 	printf("%i ", p);
	// }
	const int nThreads = NUM_THREADS;
	pthread_t threads[NUM_THREADS];
	struct solver T[NUM_THREADS];
	struct problem Q[NUM_THREADS];
	int **rmap, ans = UNSAT;
	rmap = (int **)malloc(sizeof(int *) * nThreads);
	// Copy problems and pertubate them
	for (int t = 0; t < nThreads; t++)
	{
		copyPorblem(P, &Q[t]);
		translate(P, &T[t]);
		int tmp = solve(&T[t]);
		printf("%d\n", tmp);
		exit(0);
		rmap[t] = perturb_clauses(&Q[t]);
	}
	for (int t = 0; t < nThreads; t++)
	{
		translate(&Q[t], &T[t]);
		T[t].solver_id = t;
		if (pthread_create(&threads[t], NULL, thread_solve, &T[t]) != 0)
		{
			perror("pthread_create");
			return UNSAT;
		}
		// ans = solve(&T);
	}
	while (!terminate_flag)
	{
		printf("Waiting to get results...\n");
		sleep(1);
	}
	for (int t = 0; t < NUM_THREADS; t++)
	{
		pthread_cancel(threads[t]);
	}
	for (int i = 0; i < T[terminate_id].nVars; i++)
	{
		int res = rmap[terminate_id][i + 1];
		P->ans[i] = T[terminate_id].model[abs(res)] ^ (res < 0);
	}
	// putchar('\n');
	// for (int i = 0; i < nThreads; i++)
	// 	free(rmap[i]);
	// free(rmap);
	return T[terminate_id].result;
}

int main(int argc, char **argv)
{ // The main procedure for a STANDALONE solver
	srand(time(0));
	struct problem P;
	if (argc <= 1)
	{
		printf("no input file provided\n");
		exit(0);
	} // Expect a single argument
	else if (input(&P, argv[1]) == UNSAT)
		printf("s UNSATISFIABLE\n"); // Parse the DIMACS file in argv[1]
	else if (parallel_solve(&P) == UNSAT)
		printf("s UNSATISFIABLE\n"); // Solve without limit (number of conflicts)
	else
	{
		printf("s SATISFIABLE\n"); // And print whether the formula has a solution
		for (int i = 0; i < P.nVars; i++)
			printf("%i ", P.ans[i]);
		putchar('\n');
	}
}
