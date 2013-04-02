#include <stdlib.h>
#include <stdio.h>
#include "lcpskip.h"

// debug output
#ifdef NDEBUG
#define eprintf(...)
#define INC(var)
#else
#define eprintf(...) fprintf (stderr, __VA_ARGS__);
#define INC(var) var++
#endif

// stats
stat_cmps=0;
stat_chr_cmps=0;
stat_lcp_cmps=0;


int randomLevel() {
  int l = 0;
  while( rand() < PROB * RAND_MAX )
    l++;
  return l;
}

Node *mkNode( int height ) { 

  Node * n = (Node *) malloc( 100+  sizeof( Node ) + (height)*sizeof(fwd));
  int i;
  for( i = 0; i < height; i++ ) {
    n->forward[i].ptr = NULL;
    n->forward[i].lcp = 0;
  }
  n->key = NULL;
  n->value = NULL;
  return n;
}

SkipList * mkList() {
  SkipList *l = (SkipList *) malloc( sizeof( SkipList ));
  l->header = mkNode(MAXHEIGHT);
  l->level=0; // level is 0-based, ie 0 means 1 link
  return l;
}

// strcmp with lcp length input/return
// input lcp is start, output set to last cmp
int strlcpcmp( char * s, char * t, int *lcp ) {
  int i,r;

  // to test no lcp:  
  //return strcmp(s,t);
  INC(stat_cmps);

  for( i = *lcp;  !(r= t[i] - s[i] ) && s[i]; i++ )
    INC(stat_chr_cmps);

  INC( stat_chr_cmps );

  *lcp = i;  // output new lcp

  return r;  // works w/ \0 etc.

}

// Search similar to pseudo-code in Pugh's skip list paper
// same control flow, but key comparisons use lcp tricks 
// similar to lcp merge sort algo
char *Search( SkipList *list, char * key ) {
  
// loop variables
// x is current/most recent comparand
// lcp is lcp(x,key), to check against lcp(x,x->forward[i])
// lcpfwd is result of forward cmp
  Node * x = list->header;  
  int lcp = 0;  // lcp( key, x->key ) from most recent > comparison
  int lcpfwd = 0; // lcp from most recent comp even if >=

  int i;

  for( i = list->level; i >= 0; i-- ) {

    lcpfwd = lcp;  
// same level traversal
// lcp-based optimization:
// using x as current key and forward as next in (sorted) linked list
//
// basic trick is to predict strcmp(key, forward)
// given key>x and we know lcp(key,x):
//  => if lcp(key,x) < lcp(x,forward), then key > forward
//      since x and forward have same prefix up to where key cmp stops
//  => key==forward only if lcp(key,x)==lcp(x,forward)
//     ie forward must agree w/x where key does, but no more
//  any shorter lcp(x,forward) means forward>key (stop)
//
//  this rule is described in Ng and Kakehi
//  https://www.jstage.jst.go.jp/article/ipsjdc/4/0/4_0_69/_pdf
//  
    while(x->forward[i].ptr != NULL  
	  && (   lcp <  x->forward[i].lcp
	      || lcp == x->forward[i].lcp && strlcpcmp(x->forward[i].ptr->key, key, &lcpfwd ) < 0 ))
      {
	INC( stat_lcp_cmps );
	x = x->forward[i].ptr;
	lcp = lcpfwd;
	eprintf( "search:  level %d lcp %d > %s\n", i, lcpfwd, x->key );
      }
  }

  INC( stat_lcp_cmps ); // count last cmp 
  // x must be predecessor or NULL/end of list
  x = x->forward[0].ptr;
  //  if( x != NULL && !strlcpcmp( key, x->key, &lcp ))
    return x->value;
    //else 
    //return NULL;
}

int Insert( SkipList *list, char * key, char * value ) {
  
  Node * x = list->header;
  Node * update[MAXHEIGHT];
  int lcpupdate[MAXHEIGHT];
  int lcp_fwd[MAXHEIGHT];
  
  int lcp = 0;  // lcp( key, x->key )
  
  int lcpfwd = 0;
  int i;
  for(i = list->level; i >= 0; i-- ) {

    lcpfwd = lcp;
    while( x->forward[i].ptr != NULL
	  && (   lcp <  x->forward[i].lcp
	      || lcp == x->forward[i].lcp && strlcpcmp( x->forward[i].ptr->key, key, &lcpfwd ) < 0 ))
      {
	eprintf( "key: %s level=%d lcp = %d key > %s\n", key, i, lcp, x->key );
	x = x->forward[i].ptr;
	lcp = lcpfwd;
      }
    eprintf( "key: %s level=%d lcp = %d stop >  %s\n", key, i, lcp, x->key );
    update[i] = x;
    lcpupdate[i] = lcp;

    // figure out forward lcp based on boolean exprs above
    if( x->forward[i].ptr == NULL )
      lcp_fwd[i] = 0;
    else if( lcp > x->forward[i].lcp )
      lcp_fwd[i] = x->forward[i].lcp;
    else // == 
      lcp_fwd[i] = lcpfwd;  
  }

  // create new node and update predecessors
  int lvl = randomLevel(); 

  if( lvl > list->level ) {
    for( i = list->level +1; i <= lvl; i++ ) {
      update[i]=list->header;
      lcp_fwd[i]=0;    // to end/null
      lcpupdate[i]=0; // from header
    }
    list->level = lvl;
  }

  x = mkNode( lvl );
  x->key = key;
  x->value = value;

  for( i = 0; i <= lvl; i++ ) {
    x->forward[i].ptr = update[i]->forward[i].ptr;
    x->forward[i].lcp = lcp_fwd[i];
    update[i]->forward[i].ptr = x;
    update[i]->forward[i].lcp = lcpupdate[i];
  }

  return 1;
}

