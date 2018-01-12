/*
* File Name: buffer.h
* Version: 1.17.2
* Compiler: MS Visual Studio 2015
* Author: Wenjing Wang, ID# 040812907
* Course: CST8152 - Compiler, Lab Section: 013
* Assignment: 1
* Date: 29 September 2017
* Professor: Svillen Ranev
* Purpose: To include all the constants, data type and function declarations that are needed in buffer.c file
* Function list: b_allocate, b_addc, b_clear, b_free, b_isfull, b_limit, b_capacity, b_mark,
*                b_mode, b_incfactor, b_load, b_isempty, b_eob, b_getc, b_print, b_compact,
*				 b_rflag, b_retract, b_reset, b_getcoffset, b_rewind, b_location
*/
#ifndef BUFFER_H_
#define BUFFER_H_

#define MACOS_DEP
#undef MACOS_DEP
/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

							/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

														   /* standard header files */
#include <stdio.h>  /* standard input/output */
#include <limits.h> /* implementation-defined data type ranges and limits */

#ifndef WIN32
#include <mm_malloc.h> /* for dynamic memory allocation.*/
#else
#include <malloc.h>
#endif

														   /* constant definitions */
#ifndef TRUE 
#define TRUE 1
#endif

#ifndef FALSE 
#define FALSE 0
#endif

#define RT_FAIL1 -1             /* fail return value */
#define RT_FAIL2 -2             /* fail return value */
#define LOAD_FAIL -2            /* load fail error */
#define SET_R_FLAG 1            /* realloc flag set value */
#define UNSET_R_FLAG 0          /* unset reallocation flag */
#define SET_EOB_FLAG 1          /* set end of buffer flag */
#define UNSET_EOB_FLAG 0        /* unset end of buffer flag */

#define MUL_OP_MODE -1          /* multiplicative op mode */
#define FIX_OP_MODE 0           /* fixed op mode */
#define ADD_OP_MODE 1           /* additive op mode */

#define OFFSET_RESET 0          /* reset character offset */
#define ZERO_CAPACITY 0         /* indicate the capaciy is zero*/
#define MAX_ALLOWED_VALUE -1    /* maximum allowed positive value*/
#define MIN_CAPACITY 1          /* minimum character buffer capacity */

#define FIX_INC_FACTOR 0        /* fixed increment factor constant */
#define MIN_INC_FACTOR 1        /* minimum additive increment factor constant */
#define MIN_MUL_INC_FACTOR 1    /* minumum multiplicative increment factor constant */
#define MAX_ADD_INC_FACTOR 255  /* maximum additive increment factor constant */
#define MAX_MUL_INC_FACTOR 100  /* maximum multiplicative increment factor constant */
#define ERR_INC_FACTOR 256      /* return value in case of error on reading increment factor */

														   /* user data type declarations */
typedef struct BufferDescriptor {
	char *cb_head;      /* pointer to the beginning of character array (character buffer) */
	short capacity;     /* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;  /* the offset (in chars) to the add-character location */
	short getc_offset;  /* the offset (in chars) to the get-character location */
	short markc_offset; /* the offset (in chars) to the mark location */
	char  inc_factor;   /* character array increment factor */
	char  r_flag;       /* reallocation flag */
	char  mode;         /* operational mode indicator*/
	int   eob;          /* end-of-buffer flag */
} Buffer, *pBuffer;     /*typedef Buffer *pBuffer;*/

						/* function declarations */
Buffer* b_allocate(short, char, char);
pBuffer b_addc(pBuffer const, char);
int b_clear(Buffer* const);
void b_free(Buffer* const);
int b_isfull(Buffer* const);
short b_limit(Buffer* const);
short b_capacity(Buffer* const);
short b_mark(Buffer* const, short);
int b_mode(Buffer * const);
size_t b_incfactor(Buffer * const);
int b_load(FILE * const, Buffer * const);
int b_isempty(Buffer * const);
int b_eob(Buffer * const);
char b_getc(Buffer * const);
int b_print(Buffer* const);
Buffer* b_compact(Buffer* const, char);
char b_rflag(Buffer* const);
short b_retract(Buffer* const);
short b_reset(Buffer* const);
short b_getcoffset(Buffer * const);
int b_rewind(Buffer* const);
char* b_location(Buffer* const, short);

#endif
