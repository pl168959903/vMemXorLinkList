

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vMemXor.h"

uint8_t dd[ 2000 ] = { 0 };

int main( void ) {

    uint8_t*        testAddr = dd;
    uint32_t        addrSize = 1000;
    vMemXorLinkList t        = { testAddr, addrSize };

    if ( vMemXorInit( &t ) ) {
        printf( "分配失敗" );
        while ( 1 ) {};
    }
    else
        printf( "分配成功" );

    printf( "\n" );
    printf( "base addr : 0x%08zX ~ 0x%08zX\n", ( size_t )testAddr, ( size_t )testAddr + addrSize - 1 );
    printf( "addr size : %d\n", addrSize );
    printf( "Start addr : 0x%08zX \n", ( size_t )&t.pxStart );
    printf( "End addr : 0x%08zX \n", ( size_t )&t.pxEnd );
    vMemInfoPrint( &t );
    void* a[ 10 ];

    printf( "step 1\n" );
    a[ 0 ] = vMemXorAlloc( &t, 100 );
    vMemInfoPrint( &t );

    printf( "step 2\n" );
    a[ 1 ] = vMemXorAlloc( &t, 100 );
    vMemInfoPrint( &t );

    printf( "step 3\n" );
    a[ 2 ] = vMemXorAlloc( &t, 100 );
    vMemInfoPrint( &t );

    printf( "step 4\n" );
    vMemXorFree( &t, a[ 0 ] );
    vMemInfoPrint( &t );

    printf( "step 5\n" );
    vMemXorFree( &t, a[ 2 ] );
    vMemInfoPrint( &t );

    printf( "step 6\n" );
    vMemXorFree( &t, a[ 1 ] );
    vMemInfoPrint( &t );
    return 0;
}
