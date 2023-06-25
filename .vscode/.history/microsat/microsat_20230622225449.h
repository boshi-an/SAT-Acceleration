#ifndef __MICROSAT_H__
#define __MICROSAT_H__

enum
{
	END = -9,
	UNSAT = 0,
	SAT = 1,
	MARK = 2,
	IMPLIED = 6
};

struct solver
{ // The variables in the struct are described in the initCDCL procedure
	int *DB, nVars, nClauses, mem_used, mem_fixed, mem_max, maxLemmas, nLemmas, *buffer, nConflicts, *model,
		*reason, *falseStack, *false, *first, *forced, *processed, *assigned, *next, *prev, head, res, fast, slow;
};

#endif
