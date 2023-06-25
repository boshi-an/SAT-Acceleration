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

int parse(struct solver *S, char *filename);
void initCDCL(struct solver *S, int n, int m);
void copyCDCL(struct solver *S, struct solver *T);
int propagate(struct solver *S);
int *analyze(struct solver *S, int *clause);
int implied(struct solver *S, int lit);
void bump(struct solver *S, int lit);
void reduceDB(struct solver *S, int k);
int *addClause(struct solver *S, int *in, int size, int irr);
int *getMemory(struct solver *S, int mem_size);
void addWatch(struct solver *S, int lit, int mem);
void assign(struct solver *S, int *reason, int forced);
void restart(struct solver *S);
void unassign(struct solver *S, int lit);

#endif
