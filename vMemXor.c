/*
       __  ___          _  __         __   _      __    __   _     __
 _  __/  |/  /__ __ _  | |/_/__  ____/ /  (_)__  / /__ / /  (_)__ / /_
| |/ / /|_/ / -_)  ' \_>  </ _ \/ __/ /__/ / _ \/  '_// /__/ (_-</ __/
|___/_/  /_/\__/_/_/_/_/|_|\___/_/ /____/_/_//_/_/\_\/____/_/___/\__/

*/
// verion : 1.0.0

#include "vMemXor.h"

/**
 * @brief  Block 互斥或
 * @retval None
 */
static void BlockXor( stBlock_T* a, stBlock_T* b ) {
    a->xorPtr = BLOCK_XOR( a->xorPtr, b );
    b->xorPtr = BLOCK_XOR( b->xorPtr, a );
}

/**
 * @brief 正向遍歷
 * @note   指針正向移動
 * @param  px: 串列指針
 * @retval None
 */
static void TraverseBlock( stPointer_T* px ) {
    stBlock_T* temp = NEXT_BLOCK( px->lastPointer, px->pointer );
    px->lastPointer = px->pointer;
    px->pointer     = temp;
}

/**
 * @brief 反向遍歷
 * @note   指針正向移動
 * @param  px: 串列指針
 * @retval None
 */
static void TraverseBlock_r( stPointer_T* px ) {
    stBlock_T* temp = NEXT_BLOCK( px->pointer, px->lastPointer );
    px->pointer     = px->lastPointer;
    px->lastPointer = temp;
}

/**
 * @brief  插入 Block
 * @note   向指針的下一個位置插入 Block
 * @param  px: 串列指針
 * @param  newBlock:
 * @retval None
 */
static void BlockInsert( stPointer_T* px, stBlock_T* newBlock ) {
    stBlock_T* nextBlock = NEXT_BLOCK( px->lastPointer, px->pointer );
    newBlock->xorPtr     = ( size_t )NULL;
    BlockXor( px->pointer, nextBlock );
    BlockXor( px->pointer, newBlock );
    BlockXor( newBlock, nextBlock );
    TraverseBlock( px );
}

/**
 * @brief  移除 Block
 * @note   移除指針指向的 Block
 * @param  px: 串列指針
 * @retval None
 */
static void BlockDelete( stPointer_T* px ) {
    stBlock_T* nextBlock = NEXT_BLOCK( px->lastPointer, px->pointer );
    BlockXor( px->lastPointer, px->pointer );
    BlockXor( px->pointer, nextBlock );
    BlockXor( px->lastPointer, nextBlock );
    px->pointer = nextBlock;
    TraverseBlock_r( px );
}

/**
 * @brief  Block 交換
 * @note   交換指針指向的 Block
 * @param  px: 串列指針
 * @param  swapBlock: 待交換 Block
 * @retval None
 */
static void BlockSwap( stPointer_T* px, stBlock_T* swapBlock ) {
    stBlock_T* nextBlock = NEXT_BLOCK( px->lastPointer, px->pointer );
    BlockDelete( px );
    BlockInsert( px, swapBlock );
}

/**
 * @brief  分配Block
 * @note   在指針上的Block分配新空間
 * @param  px: 串列指針
 * @param  allocSize: 分配大小
 * @retval 分配後的Block地址，未分配失敗回傳NULL
 */
static stBlock_T* BlockAlloc( stPointer_T* px, size_t allocSize ) {
    if ( px->pointer->size < allocSize ) return NULL;
    stBlock_T* retval = px->pointer;
    if ( ( px->pointer->size - __BLOCK_SIZE ) > allocSize ) {
        stBlock_T* newBlock = ( stBlock_T* )( ( size_t )px->pointer + allocSize );
        newBlock->size      = px->pointer->size - allocSize;
        px->pointer->size   = allocSize;
        BlockSwap( px, newBlock );
    }
    else {
        BlockDelete( px );
    }
    return retval;
}

/**
 * @brief  尋找最小空間 Block
 * @note   尋找最小空間 Block 並指向
 * @param  px: 串列指針
 * @retval
 */
static stPointer_T* FindMinSizeBlock( stPointer_T* px ) {
    stPointer_T retval = ( stPointer_T ){ px->list, &px->list->pxStart, NEXT_BLOCK( NULL, &px->list->pxStart ) };
    *px                = retval;
    TraverseBlock( px );
    while ( px->pointer != &px->list->pxEnd ) {
        if ( px->pointer->size < retval.pointer->size ) retval = *px;
        TraverseBlock( px );
    }
    *px = retval;
    return px;
}

/**
 * @brief  尋找 Block 所在區間
 * @note   尋找 Block 所在區間並指向前一個Block
 * @param  px: 串列指針
 * @param  pxb:
 * @retval
 */
static stPointer_T* FindBlockZone( stPointer_T* px, stBlock_T* pxb ) {
    *px                  = ( stPointer_T ){ px->list, NULL, &px->list->pxStart };
    stBlock_T* nextBlock = NEXT_BLOCK( px->lastPointer, px->pointer );
    while ( nextBlock != &px->list->pxEnd ) {
        if ( ( size_t )nextBlock > ( size_t )pxb ) return px;
        TraverseBlock( px );
        nextBlock = NEXT_BLOCK( px->lastPointer, px->pointer );
    }
    return px;
}

/**
 * @brief  聯合 Block
 * @note   檢查指向Block的頭尾區塊，若為連續Block，則連接
 * @param  px: 串列指針
 * @retval None
 */
static void BlockUnite( stPointer_T* px ) {
    stBlock_T* nextBlock = NEXT_BLOCK( px->lastPointer, px->pointer );
    if ( nextBlock != &px->list->pxEnd ) {
        if ( ( stBlock_T* )( ( size_t )px->pointer + px->pointer->size ) == nextBlock ) {
            px->pointer->size += nextBlock->size;
            TraverseBlock( px );
            BlockDelete( px );
        }
    }
    if ( px->lastPointer != &px->list->pxStart ) {
        if ( ( stBlock_T* )( ( size_t )px->lastPointer + px->lastPointer->size ) == px->pointer ) {
            px->lastPointer->size += px->pointer->size;
            BlockDelete( px );
        }
    }
}

/**
 * @brief  釋放空間
 * @note
 * @param  list: 鏈結串列結構
 * @param  addr: 釋放空間位置
 * @retval 成功回傳0
 */
size_t vMemXorFree( vMemXorLinkList* list, void* addr ) {
    if ( list == NULL ) return -1;
    if ( addr == NULL ) return -2;

    stPointer_T px      = { list };
    stBlock_T*  pxblock = GET_BLOCK_FROM_ARRD( addr );

    FindBlockZone( &px, pxblock );
    BlockInsert( &px, pxblock );
    BlockUnite( &px );
    return 0;
}

/**
 * @brief  初始化鏈結串列
 * @note
 * @param  list: 鏈結串列結構
 * @retval 成功回傳0
 */
size_t vMemXorInit( vMemXorLinkList* list ) {
    if ( list == NULL ) return -1;
    if ( list->array == NULL ) return -2;

    stBlock_T* firstBlock;
    firstBlock       = ( stBlock_T* )__ALIGN_CEIL( list->array );
    list->arraySize  = ( size_t )__ALIGN_FLOOR( list->arraySize - ( ( size_t )firstBlock - ( size_t )list->array ) );
    firstBlock->size = list->arraySize;
    if ( firstBlock->size <= __BLOCK_SIZE ) return -3;

    stPointer_T px = { list, ( stBlock_T* )NULL, &list->pxStart };
    BlockXor( &list->pxStart, &list->pxEnd );

    BlockInsert( &px, firstBlock );
    return 0;
}

/**
 * @brief  分配空間
 * @note
 * @param  list: 鏈結串列結構
 * @param  size: 分配大小
 * @retval 分配空間地址，分配失敗回傳NULL
 */
void* vMemXorAlloc( vMemXorLinkList* list, size_t size ) {
    if ( list == NULL ) return NULL;
    if ( size == 0 ) return NULL;

    stPointer_T px = { list };
    size           = ( size_t )__ALIGN_CEIL( ( void* )size ) + __BLOCK_SIZE;

    FindMinSizeBlock( &px );
    stBlock_T* blockAddr = BlockAlloc( &px, size );

    if ( blockAddr == NULL ) return NULL;
    return GET_ADDR_FROM_BLOCK( blockAddr );
}

/**
 * @brief  列印鏈接串列資訊
 * @note
 * @param  list: 鏈結串列結構
 * @retval None
 */
void vMemInfoPrint( vMemXorLinkList* list ) {
    stPointer_T px = { list, NULL, &list->pxStart };
    __PRINTF( "\n----------vMemXorInfoPrint-------------\n" );
    while ( px.pointer != NULL ) {
        __PRINTF( "Block Addr: 0x%08zX\n", ( size_t )px.pointer );
        __PRINTF( "Block size: %zd\n", ( size_t )px.pointer->size );
        __PRINTF( "Block xor: 0x%08zX\n", ( size_t )px.pointer->xorPtr );
        __PRINTF( "Next block: 0x%08zX\n", ( size_t )NEXT_BLOCK( px.lastPointer, px.pointer ) );
        __PRINTF( "----------------------------------------\n" );
        TraverseBlock( &px );
    }
    __PRINTF( "[End]\n" );
}
