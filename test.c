#include "lcpskip.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv ) {
  char *s,*t ;
  int i=0, cnt=0;

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
  //  assert( s != NULL );
  printf("found: %s\n", s );

  stat_cmps=0;
  stat_chr_cmps=0;

  for( i = 0; i < 100000; i++ ) {
    s = malloc( 256 );
    sprintf( s, "%255d", i );
    Insert( l, s, s );
  }
  printf( "%d string cmp calls %d chr cmps\n", stat_cmps, stat_chr_cmps );
  printf( "level = %d\n", l->level );

  stat_cmps=0;
  stat_chr_cmps=0;
  stat_lcp_cmps=0;

  s = malloc( 256 );

  for( i = 0; i < 100000; i++ ) {
    sprintf( s, "%255d", i );
    t = Search( l, s );
#ifndef NDEBUG
  printf( "%d string cmp calls %d chr cmps\n", stat_cmps, stat_chr_cmps );
#endif
    if( t != NULL && strcmp(s,t) == 0 )
      cnt++;
  }

  //  printf( "sample key:  %s\n", s );
  printf( "%d matches\n", cnt );
  printf( "%d lcp tests %d string cmp calls %d chr cmps\n", stat_lcp_cmps, stat_cmps, stat_chr_cmps );
}
