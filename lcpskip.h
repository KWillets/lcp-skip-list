
#define MAXHEIGHT 16
#define PROB .5

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

Node *mkNode( int height ) ;

SkipList * mkList() ;

char *Search( SkipList *list, char * key ) ;

int Insert( SkipList *list, char * key, char * value ) ;


