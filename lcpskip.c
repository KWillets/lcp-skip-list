#include <stdlib.h>
#include <stdio.h>

#define MAXHEIGHT 16
#define PROB .5

int randomLevel() {
  int i = 0;
  while( rand() < PROB * RAND_MAX )
    i++;
  return i;
}


typedef struct _fwd {
  struct _node * ptr;
  unsigned short int lcp;  // lcp w/target
} fwd;

typedef struct _node {
  unsigned char *key;
  void * value; 

  // variable length
  fwd forward[1];
} Node;

typedef struct _list {
  Node * header;
  int level; 
} SkipList;

Node *mkNode( int height ) { 

  Node * n = (Node *) malloc(  sizeof( Node ) + (height)*((void *)(((fwd *)0)+1) - (void *)((fwd *)0)));
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
  l->level=0; 
  return l;
}

// strcmp with lcp length input/return
// using lcp as index of 1st mismatch (?)
// works 0-based
  int strlcpcmp( char * s, char * t, int *lcp ) {
    int i;
    for( i = *lcp; s[i] && t[i] && !( t[i] - s[i] ); i++ )
      ;
    *lcp = i;  // output new lcp
    return s[i] - t[i];  // works w/ \0 etc.
  }

char *Search( SkipList *list, char * key ) {
  
// loop variables
// x is current/most recent comparand
// lcp is lcp(x,key), to check against lcp(x,x->forward[i])
// lcptmp is result of forward cmp
  Node * x = list->header;  
  int lcp = 0;  // lcp( key, x->key )
  int lcptmp = 0;

  int i;
  for( i = list->level; i >= 0; i-- ) {

// same level traversal
// given key > x->key
//  => key > any forward key w/ lcp(x,forward) > lcp(key,x)
//      & do not strcmp since lcp longer than mismatch w/x
//  => possible match requires lcp(key,x) == lcp( x, forward )
//      & do strcmp starting from that lcp
//  any shorter lcp means > key
// does not break on exact match (traverses to predecessor instead)

    lcptmp = lcp;  
    while(x->forward[i].ptr != NULL  
	  && (lcp <  x->forward[i].lcp     // key definitely > forward key because same prefix
	      || lcp == x->forward[i].lcp && strlcpcmp(x->forward[i].ptr->key, key, &lcptmp ) < 0 ))
      {
	x = x->forward[i].ptr;
	// set lcp from cmp if == ( < ok; does not change lcptmp )
	lcp = lcptmp;
	printf( "search:  level %d lcp %d > %s\n", i, lcptmp, x->key );
      }
  }

  // x must be predecessor or NULL/end of list
  x = x->forward[0].ptr;
  if( x != NULL && !strlcpcmp( key, x->key, &lcp ))
    return x->value;
  else 
    return NULL;
}

int Insert( SkipList *list, char * key, char * value ) {
  
  Node * x = list->header;
  Node * update[MAXHEIGHT];
  int lcpupdate[MAXHEIGHT];
  int lcpfwd[MAXHEIGHT];
  
  int lcp = 0;  // lcp( key, x->key )
  
  int lcptmp = 0;
  int i;
  for(i = list->level; i >= 0; i-- ) {

    lcptmp = lcp;
    while( x->forward[i].ptr != NULL
	  && (lcp <  x->forward[i].lcp     // key definitely > forward key because same prefix
	      || lcp == x->forward[i].lcp && strlcpcmp( x->forward[i].ptr->key, key, &lcptmp ) < 0 ))
      {
	printf( "level=%d lcp = %d skipping %s\n", i, lcp, x->key );
	x = x->forward[i].ptr;
	// set lcp from cmp if == ( < ok; does not change lcptmp )
	lcp = lcptmp;
      }
    printf( "level=%d lcp = %d at  %s\n", i, lcp, x->key );
    update[i] = x;
    lcpupdate[i] = lcp;
    //??  need > case    lcpfwd[i] = lcptmp; // either (<) min(lcp,x->forward[i].lcp) or (==) result of strlcpcmp 

    if( x->forward[i].ptr == NULL )
      lcpfwd[i] = 0;
    else if( lcp > x->forward[i].lcp )
      lcpfwd[i] = x->forward[i].lcp;
    else
      lcpfwd[i] = lcptmp;  
  }

  // create new node and update predecessors
  int lvl = randomLevel(); // todo

  if( lvl > list->level ) {
    for( i = list->level +1; i <= lvl; i++ ) {
      //      list->header->forward[i].ptr = NULL; // ?? redundant
      //list->header->forward[i].lcp = 0;    // ??
      update[i]=list->header;
      lcpfwd[i]=0;    // to end/null
      lcpupdate[i]=0; // from header
    }
    list->level = lvl;
  }

  x = mkNode( lvl );
  x->key = key;
  x->value = value;

  for( i = 0; i <= lvl; i++ ) {
    x->forward[i].ptr = update[i]->forward[i].ptr;
    x->forward[i].lcp = lcpfwd[i];
    update[i]->forward[i].ptr = x;
    update[i]->forward[i].lcp = lcpupdate[i];
  }

  return 1;
}


int main( int argc, char **argv ) {
  char * s;
  SkipList * l = mkList();
  Insert( l, "aaa", "aaa" );
  Insert( l, "aab", "aab" );
  s = Search( l, "aaa" );
  printf("found: %s\n", s );
  s = Search( l, "aab" );
  printf("found: %s\n", s );
  Insert( l, "aardvark", "aardvark value" );
  Insert( l, "aardvark2", "aardvark2 value" );
  s = Search( l, "aardvark" );
  printf("found: %s\n", s );
  Insert( l, "aardwolf", "aardwolf value" );
  s = Search( l, "aardwolf" );
  printf("found: %s\n", s );
  s = Search( l, "aardvark2" );
  printf("found: %s\n", s );
}

