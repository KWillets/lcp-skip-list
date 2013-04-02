lcp-skip-list
=============

LCP-enhanced skip list 

This is the basic skiplist data structure modified to store lcp length 
with each forward pointer.  The Search and Insert operations described 
in Pugh's paper are modified to avoid redundant character comparisons by 
using these pre-calculated lcp's.

The basic idea is that the lcp of the search key and each comparand increases
as the search narrows, and the results of one comparison can be used to 
infer the results at forward links.  

