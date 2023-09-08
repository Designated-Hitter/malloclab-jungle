/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "team 4",
    /* First member's full name */
    "Jeong Dong-hwan",
    /* First member's email address */
    "dosadola@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7) //가장 가까운 8의 배수로 사이즈를 올려주는 매크로 함수

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4 //word와 header, footer size = 4bytes
#define DSIZE 8 //double word size = 8bytes
#define CHUNKSIZE (1<<12) //sbrk 함수로 늘어나는 힙의 양

#define MAX(x,y) ((x) > (y)? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc)) //size와 alloc을 OR연산자로 합치기

#define GET(p) (*(unsigned int *)(p)) //주소 p에서 word 읽기 
#define PUT(p, val) (*(unsigned int *)(p) = (val)) //주소 p에서 word 쓰기

#define GET_SIZE(p) (GET(p) & ~0x7) //주소 p에서 사이즈를 가져오는 매크로 함수
                                    //0x7은 2진수로 111이고 ~111은 000이므로 GET(p)로 가져온 값에 마지막 3자리를 000으로 만드는 역할
#define GET_ALLOC(p) (GET(p) & 0x1) //주소 p의 마지막 자리와 1을 AND연산하여 1이면 할당됨, 아니면 가용상태라고 판단하기 위한 매크로 함수

#define HDRP(bp) ((char *)(bp) - WSIZE) //블록 포인터 bp를 이용해 header의 주소를 계산
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //블록 포인터 bp를 이용해 footer의 주소를 계산

#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) //bp를 이용해 다음 블록의 주소를 계산
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) //bp를 이용해 이전 블록의 주소를 계산

/* global variable & functions */
static char* heap_listp; // 항상 prologue block을 가리키는 정적 전역 변수 설정
                        // static 변수는 함수 내부(지역)에서도 사용이 가능하고 함수 외부(전역)에서도 사용이 가능하다.

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *) -1) {
        return -1;
    }
    PUT(heap_listp, 0); //alignment padding
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); //prologue header
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); //prologue footer
    PUT(heap_listp + (3*WSIZE), PACK(0, 1)); //Epilogue header
    heap_listp += (2*WSIZE);

    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) {
        return -1;
    }

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














