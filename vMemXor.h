#ifndef __V_MEM_XOR_H
#define __V_MEM_XOR_H

#include <stdint.h>
#include <stdio.h>

#define __PRINTF printf

#define __ALIGN_BITS      ( sizeof( size_t ) * 8 )
#define __ALIGN_BYTES     ( sizeof( size_t ) )
#define __ALIGN_ADDR_MASK ( sizeof( size_t ) - 1 )

#define __ALIGN_CEIL( n )  ( ( void* )( ( ( size_t )( n ) + __ALIGN_ADDR_MASK ) & ~__ALIGN_ADDR_MASK ) )
#define __ALIGN_FLOOR( n ) ( ( void* )( ( size_t )( n ) & ~__ALIGN_ADDR_MASK ) )

#define __BLOCK_SIZE ( sizeof( stBlock_T ) )

#define NEXT_BLOCK( b1, b2 )         ( ( stBlock_T* )( ( b2 )->xorPtr ^ ( size_t )( b1 ) ) )
#define BLOCK_XOR( b1, b2 )          ( ( size_t )b1 ^ ( size_t )b2 )
#define GET_ADDR_FROM_BLOCK( block ) ( ( void* )( ( size_t )block + __BLOCK_SIZE ) )
#define GET_BLOCK_FROM_ARRD( addr )  ( ( stBlock_T* )( ( size_t )addr - __BLOCK_SIZE ) )

/**
 * @brief  塊結構
 */
typedef struct stBlock {
    size_t xorPtr;
    size_t size;
} stBlock_T;

/**
 * @brief  鏈結串列結構
 */
typedef struct vMemXorLinkList {
    uint8_t*  array;
    size_t    arraySize;
    stBlock_T pxStart, pxEnd;
} vMemXorLinkList;

/**
 * @brief  指針結構
 */
typedef struct stPointer {
    vMemXorLinkList* list;
    stBlock_T*       lastPointer;
    stBlock_T*       pointer;
} stPointer_T;

void vMemInfoPrint( vMemXorLinkList* list );
void* vMemXorAlloc( vMemXorLinkList* list, size_t size );
size_t vMemXorInit( vMemXorLinkList* list );
size_t vMemXorFree( vMemXorLinkList* list, void* addr );

#endif /* __V_MEM_XOR_H */
