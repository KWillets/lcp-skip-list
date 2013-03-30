#include "lcpskip.h"
#include <stdio.h>

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
