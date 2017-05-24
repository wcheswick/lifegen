# lifegen
Compute a new generation of Conway's life in parallel in wide integers in constant time.

**Gen life in parallel**

\

Computing whether a cell in a new generation of life is set
is a simple bit of counting neighbors and some easy logic. We only
need to count the neighbors, then do some shifting, masking, and simple
logical operations to decide whether the cell is set in the next
generation.

\

We have to count the neighbors, which may reach 8, which needs 4 bits. 
We don't
want to deal with a carry bit: these are 4-bit unsigned integers.
Therefore,
we can do every 4 bits in a long integer at once, with single operations
on the integer.
Shift and repeat four times, and one has the answer.  

\

There are no conditionals here: the simple logic does that for us,
meaning that computing
any arena of life cells up to the word size minus 2 (I don't care about
stuff beyond the edge)
will take the exact same number of CPU cycles.

\

Originally I wrote this for the CDC 6400 in assembler around 1972,
shortly after Conway's
life had emerged as a rage.  He recently told me that the US government
told him that 
about 2 million man-hours of government workers’ time was spent on
this. 

\

As I recall, the COMPASS subroutine computed a 58x58 generation in about
3.3ms, not
bad for a \~1 MIP machine.

\

Now we have at least 64-bit unsigned longs, and many compilers offer
128-bit unsigned longs.
The process requires shifting, masking, adding, and logical operations,
all possibly in different
in different functional CPU units.  A good optimizer might make a big
difference on the maximum
speed of the routine.

\

Of course, it still limits its computation to one word size, which, if
at 128, ought to be big enough
for almost anyone.  I leave it to others to glue these together into a
bigger arena.

\

And, of course, all of this cleverness might be dwarfed by simple
routines in a GPU.

\

\

**The logic**

\

A live cell continues to exist if it has exact two or three neighbors,
or else it dies. An empty
cell comes to life if it has exactly three neighbors. We want to compute
the new value
for *c, nc*.

\

So we start with the current cell, *c*, and count its neighbors by
extracting them with a mask
and shifting and adding to get a 4-bit unsigned integer between 0 and 8:

*b8 b4 b2 b1*

\

Rule 1: If *b8* or *b4* is set, the new cell is overpopulated and
therefore empty; else

\

Rule 2: if *b2* is zero, the number of neighbors is 0 or 1, and the new
cell is empty.

\

At this point we have *c* and either two or three neighbors:

\

*c   b2  b1   nc*

0   1    0     0

0   1    1�     1

1   1    0     1

1   1    1     1

This is *c* ∨ (*b1* ∧ *b2*)

\

So, the *nc* is ¬*b8* ∧ ¬*b4* ∧ *b2* ∧ (*c* ∨ (*b1* ∧ *b2*))

\

In C, this translates to:

\

nc = \~b8 & \~b4 ! \~b2 & (c | (b1 & b2))

\

**The code**

\

Note: we could use CAND (the && operator), but that is a conditional. 
It is probably faster just to get the
logic right.

\

**Performance**

\

See the source code for the results.  The speedup is about 4,700 times
that of the CDC implementation: Moore's Law.


