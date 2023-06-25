#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

void perturb_clauses(struct solver *S)
{
}

int parallel_solve(struct solver *S)
{
	for (int i = 0; i < S->mem_max && i < 1000; i++)
	{
		int p = S->DB[i];
		printf("%i ", p);
	}
	struct solver T;
	copyCDCL(S, &T);
	perturb_clauses(&T);
	int ans = solve(&T);
	copyCDCL(&T, S);
	return ans;
}

int main(int argc, char **argv)
{					 // The main procedure for a STANDALONE solver
	struct solver S; // Create the solver datastructure
	if (argc <= 1)
	{
		printf("no input file provided\n");
		exit(0);
	} // Expect a single argument
	else if (parse(&S, argv[1]) == UNSAT)
		printf("s UNSATISFIABLE\n"); // Parse the DIMACS file in argv[1]
	else if (parallel_solve(&S) == UNSAT)
		printf("s UNSATISFIABLE\n"); // Solve without limit (number of conflicts)
	else
	{
		printf("s SATISFIABLE\n"); // And print whether the formula has a solution
		printf("Printing Results:\n");
		for (int i = 0; i < S.nVars; i++)
			printf("%i ", S.model[i + 1]);
		putchar('\n');
	}
	printf("c statistics of %s: mem: %i conflicts: %i max_lemmas: %i\n", argv[1], S.mem_used, S.nConflicts, S.maxLemmas);
}
