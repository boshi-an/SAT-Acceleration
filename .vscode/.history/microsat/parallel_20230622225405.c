#include <stdio.h>
#include <stdlib.h>
#include "microsat.h"

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
	else if (solve(&S) == UNSAT)
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
