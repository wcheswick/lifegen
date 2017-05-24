
/* gen: sample routine and test showing how to compute generations of Conway's
 *	game of life on machines with wide integers, in constant time.
 *
 * I wrote similar code for the CDC6400 in COMPASS around 1972, when I probably
 * should have been working on math homework.
 *
 * Bill Cheswick, Flemington, NJ. May 2017
 */

// Timing on my Mac, unrolled loop
//
// O0: 2.19
// O1: 1.017
// O2: 0.77
// O3: 0.77
//
// With the loop unrolled
//
// O0: 2.41
// O1: 1.12
// O2: 0.697	** best deal.  I wonder why
// O3: 0.713
//
// With the loop unrolled, calling the inline function:
//
// O0: (inline not available)
// O1: (inline not available)
// O2: 0.711
// O3: 0.711
//
// Speedup on my Mac 3.5 GHz Intel Core i5 compared to the CDC 6400 code
// from 1972: about 4700 times faster for a slightly larger arena (64 vs 60 bits)

// NB: This code is not thoroughly tested.  Edge conditions and neighbors >= 4 come to
// mind.  And all this should run much faster in a GPU.

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#define TYPE	u_int64_t
#define BITS	(8*sizeof(TYPE))

typedef TYPE board[BITS];

void
dump_type(TYPE t, char *m) {
	int j;
	if (!m)
		m = " ";
	printf("%4s", m);
	printf("|");
	for (j=0; j<BITS; j++) {
		if (j && (j % 4) == 0)
			printf("|");
		printf("%s", t>>(BITS - 1 - j) & 0x1 ? "X" : " ");
	}
	printf("|\n");
}

void
dump(board b) {
	int i, j;
	printf("---\n");
	for (i=0; i<5 /*BITS*/; i++) {
		dump_type(b[i], 0);
	}
	printf("---\n");
}

#define B0(i)	(b0mask & (i)>>0)
#define B1(i)	(b0mask & (i)>>1)
#define B2(i)	(b0mask & (i)>>2)
#define B3(i)	(b0mask & (i)>>3)

#define R1(i)	((i)>>1)	// right shift one

TYPE b0mask = 0;
TYPE b1mask = 0;
TYPE b2mask = 0;
TYPE b3mask = 0;

TYPE
donibble(TYPE rp, TYPE rc, TYPE rn, int shift) {
	TYPE ps = rp >> shift;	// shifted fields
	TYPE cs = rc >> shift;
	TYPE ns = rn >> shift;
	TYPE c = cs & b1mask;
	TYPE nn = B0(ps) + B1(ps) + B2(ps) + // compute neighbors
		B0(cs) + B2(cs) +
		B0(ns) + B1(ns) + B2(ns);

	// compute new value in bit 1
	// NB: old shift trick:  shift 3-1 means move bit 3 to bit 1
	TYPE x = 
		~(nn >> (3-1)) &	// b3 means eight neighbors, dead
		~(nn >> (2-1)) &	// b2 means >= 4 neighbors, dead
		(nn & b1mask) &		// to be alive, b2 set (2 or three neighbors)
		(c |			// either already alive with 2--3 neighbors or
		 (nn << (1-0)));		// b0 + b1 = 3 neighbors, birth
	TYPE y = (b1mask & x) << shift;
	return y;
}

void
gen(board in, board out) {\
	TYPE rp;
	TYPE rc = in[0];
	TYPE rn = in[1];
	int i, j;

	for (i=0; i<BITS; i+=4) {
		b0mask |= (TYPE)1<<i;
	}
	b1mask = b0mask << 1;
	b2mask = b1mask << 1;
	b3mask = b2mask << 1;

	out[0] = out[BITS-1] = 0;

	for (i=1; i<BITS-1; i++) {
		TYPE ncw = 0;
		rp = rc;
		rc = rn;
		rn = in[i+1];

//dump_type(rp, "rp");
//dump_type(rc, "rc");
//dump_type(rn, "rn");

		out[i] = donibble(rp, rc, rn, 0) |
			donibble(rp, rc, rn, 1) |
			donibble(rp, rc, rn, 2) |
			donibble(rp, rc, rn, 3);
	}
}

int
main(int argc, char *argv[]) {
	int i;
	board in, out;
	memset(in, 0, sizeof(board));
#ifdef RANDOM
	srandom(1);
	for (i=0; i<BITS; i++)
		in[i] = random() <<33 | random()<<16 | random();
#endif
	in[1] = (TYPE)0x7654321076543210;
	in[2] = (TYPE)0x0000000022222222;
	in[3] = (TYPE)0x0000000000000000;
//	in[3] = (TYPE)0x7654321076543210;
	gen(in, out);
dump(in);

// do a million times, for timing test

	for (i=0; i<1000000; i++)
		gen(in, out);
dump(out);
}
