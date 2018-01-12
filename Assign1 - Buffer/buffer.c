/*
* File Name: buffer.c
* Version: 1.17.2
* Compiler: MS Visual Studio 2015
* Author: Wenjing Wang, ID# 040812907
* Course: CST8152 - Compiler, Lab Section: 013
* Assignment : 1
* Date: 29 September 2017
* Professor: Svillen Ranev
* Purpose: A character buffer utility with three modes of self-incrementation
*          through dynamic memory allocation, and ability to set a mark flag.
*
* Function list: b_allocate, b_addc, b_clear, b_free, b_isfull, b_limit, b_capacity, b_mark,
*                b_mode, b_incfactor, b_load, b_isempty, b_eob, b_getc, b_print, b_compact, 
*				 b_rflag, b_retract, b_reset, b_getcoffset, b_rewind, b_location
*/

#include <stdlib.h>
#include <limits.h>
#include "buffer.h"

/*
* Purpose: This function creates a new buffer in memory (on the program heap).
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: calloc(), malloc(), free()
* Parameters: short init_capacity, char inc_factor, char o_mode
* Return value: pBD, NULL
* Algorithm: 1. allocate memory for one Buffer; 
             2. allocates memory for one dynamic character buffer; 
			 3. sets the buffer operational mode indicator mode
*/
Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode) {
	/* pointer to buffer descriptor */
	pBuffer pBD; 
	/* allocate memory for one Buffer structure */
	pBD = (Buffer *)calloc(1, sizeof(Buffer));
	/* validation of the function arguments */
	if (!pBD) {
		return NULL;
	}

	/* check the range of the init_capacity */
	if (init_capacity < ZERO_CAPACITY || init_capacity < MAX_ALLOWED_VALUE) {
		return NULL;
	}

	/* allocates memory for one dynamic character buffer */
	pBD->cb_head = (char *)malloc(sizeof(char) * init_capacity);
	/* if allocation fails, abort and release memory */
	if (!pBD->cb_head) {
		free(pBD);
		return NULL;
	}

	/* sets the buffer operational mode indicator mode */
	if (o_mode == 'f' || inc_factor == FIX_INC_FACTOR) {
		pBD->mode = FIX_OP_MODE;
		pBD->inc_factor = FIX_INC_FACTOR;
	}
	else if (o_mode == 'a') {
		pBD->mode = ADD_OP_MODE;
		pBD->inc_factor = inc_factor;
	}
	else if (o_mode == 'm' && (inc_factor >= MIN_MUL_INC_FACTOR) && (inc_factor <= MAX_MUL_INC_FACTOR)) {
		pBD->mode = MUL_OP_MODE;
		pBD->inc_factor = inc_factor;
	}
	else {	
		free(pBD);
		return NULL;  
	}

	/* copies the given init_capacity value into the Buffer structure capacity variable. */
	pBD->capacity = init_capacity;
	return pBD; 
}

/*
* Purpose: The function resets r_flag to 0 and tries to add the character symbol to
           the character array of the given buffer pointed by pBD.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: realloc()
* Parameters: pBuffer const pBD, char symbol
* Return value: pBuffer, NULL
* Algorithm: 1. check the buffer is operational and not full, add the character to buffer
             2. check the buffer is full, resize the buffer
			 3. check the operational mode, function returns different value
                operational mode indicator mode
*/
pBuffer b_addc(pBuffer const pBD, char symbol) {
	/* variables used to increase the current capacity */
	short ava_space, new_inc, new_capacity = 0;
	/* variables needed to check the memory location when reallocation */
	char *old_add;
	char *temp_add;

	/* validation of the function arguments */
	if (!pBD) {
		return NULL; 
	}

	/* reset reallocation flag to 0 */
	pBD->r_flag = UNSET_R_FLAG;

	/* the character buffer is already full, resize the buffer */
	if (pBD->addc_offset == pBD->capacity) {
		/* If the operational mode is 0, returns NULL. */
		if (pBD->mode == FIX_OP_MODE) { 
			return NULL;
		}
		/* If the operational mode is 1, increases current capacity */
		else if (pBD->mode == ADD_OP_MODE) { 
			/* If the result is positive, add inc_factor */
			if (new_capacity > ZERO_CAPACITY && new_capacity < MAX_ALLOWED_VALUE) {
				new_capacity = pBD->capacity + (unsigned char)pBD->inc_factor; 
			}
			/* If the result is not positive, assigns -1 to the new capacity */
			else if (new_capacity > ZERO_CAPACITY || new_capacity < MAX_ALLOWED_VALUE) {
				new_capacity = MAX_ALLOWED_VALUE;
			}
			/* The operation is unsuccessful */
		    else {
			    return NULL;
			}
		}
		/* If the operational mode is -1, increases current capacity */
		else if (pBD->mode == MUL_OP_MODE) {
			if (pBD->capacity == SHRT_MAX) {
				return NULL;
			}

			/* msvc warns about loss of data when converting from double to short,
			because the buffer doesn't deal with floating points, it is ok to get this warning */
			ava_space = SHRT_MAX - pBD->capacity;
			new_inc = (short)(ava_space * (((double)pBD->inc_factor) / 100));

			/* Check if the new capacity has incremented successfully */
			if (new_inc == ZERO_CAPACITY) {
				new_capacity = SHRT_MAX;
			}
			/* The current capacity is not incremented */
			else {
				new_capacity = pBD->capacity + new_inc;
			}
		} /* end of mode -1 */
		
		/* If the capacity increment in mode 1 or -1 is successful, reallocate memory */
		old_add = pBD->cb_head;
		/* expand the character buffer calling realloc() with the new capacity */
		temp_add = (char *)realloc(pBD->cb_head, sizeof(char) * (unsigned short)new_capacity);
		/* If reallocation fails, returns NULL */
		if (temp_add == NULL) {
			return NULL;
		}
			
		pBD->cb_head = temp_add;
		pBD->capacity = new_capacity;
		pBD->r_flag = (pBD->cb_head == old_add);
	} /* end of resize the buffer */

	/* symbol stored in the buffer */
	pBD->cb_head[pBD->addc_offset++] = symbol;
	return pBD;
}

/*
* Purpose: The function retains the memory space currently allocated to the buffer,
           but re-initializes all appropriate data members of the given Buffer.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: 1 (TRUE), -1 (R_FAIL1)
* Algorithm: reset everything in the buffer
*/
int b_clear(Buffer * const pBD) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}

	/* reinitialize the data members of buffer */
	pBD->addc_offset = OFFSET_RESET;
	pBD->getc_offset = OFFSET_RESET;
	pBD->markc_offset = OFFSET_RESET;
	pBD->eob = UNSET_EOB_FLAG;
	pBD->r_flag = UNSET_R_FLAG;

	return TRUE;
}

/*
* Purpose: The function de-allocates (frees) the memory
           occupied by the character buffer and the Buffer.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: free()
* Parameters: Buffer * const pBD
* Return value: nothing
* Algorithm: de-allocates the memory occupied by the character buffer
*/
void b_free(Buffer * const pBD) {
	/* de-allocates the memory */
	if (pBD) {
		free(pBD->cb_head);
		free(pBD);
	}
}

/*
* Purpose: The function checks whether the Buffer is full or not
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: -1 (R_FAIL1), 0 (FALSE), 1 (TRUE)
* Algorithm: 1. check the character buffer is full or not
             2. check the run-time error is possible or not
*/
int b_isfull(Buffer* const pBD) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}

	/* check the the character buffer is full or not */
	if (pBD->addc_offset == b_capacity(pBD)) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/*
* Purpose: The function returns the current limit of the character buffer.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: addc_offset
* Algorithm: 1. check the current limit of the buffer
             2. check the run-time error is possible or not
*/
short b_limit(Buffer* const pBD) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}

	return pBD->addc_offset;
}

/*
* Purpose: The function returns the current capacity of the character buffer.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: -1 (R_FAIL1), capacity
* Algorithm: 1. current capacity of the character buffer
             2. check the run-time error is possible or not
*/
short b_capacity(Buffer * const pBD) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}
	return pBD->capacity;
}

/*
* Purpose: The function sets markc_offset to mark.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: markc_offset, RT_FAIL1
* Algorithm: 1. check the parameter mark within the current limit of buffer
             2. check the run-time error is possible or not
*/
short b_mark(Buffer * const pBD, short mark) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}

	/* check the mark must be within the current limit of the buffer */
	if (mark < 0 || mark > pBD->addc_offset) {
		return RT_FAIL1;
	}

	pBD->markc_offset = mark;
	return pBD->markc_offset;
}

/*
* Purpose: The function returns the value of mode to the calling function
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: -1 (multiplicative), 0 (fixed), 1 (additive), -2 (RT_FAIL2)
* Algorithm: 1. check the buffer mode
*/
int b_mode(Buffer * const pBD) {
	/* If run-time error, returns -2 */
	if (!pBD) {
		return RT_FAIL2;
	}
	return pBD->mode;
}

/*
* Purpose: The function returns the non-negative value of inc_factor to the calling function.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: ERR_INC_FACTOR, inc_factor
* Algorithm: 1. return the value of inc_fator
             2. check the run-time error is possible or not
*/
size_t b_incfactor(Buffer * const pBD) {
	/* If run-time error, returns 256 */
	if (!pBD) {
		return ERR_INC_FACTOR;
	}
	return (size_t)(unsigned char)pBD->inc_factor;
}

/*
* Purpose: The function loads (reads) an open input file specified by fi into a buffer specified by pBD.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: fgetc, feof, b_addc
* Parameters: FILE * const fi, Buffer * const pBD
* Return value: int, -1 (R_FAIL1), -2 (LOAD_FAIL)
* Algorithm: 1. loads (reads) an open input file
             2. check the run-time error is possible or not
*/
int b_load(FILE * const fi, Buffer * const pBD) {
	/* to catch the buffer character */
	char buffer; 
	/* catch the character counter */
	int count = 0; 

	/* If run-time error, returns -1 */
	if (!pBD || !fi) {
		return RT_FAIL1;
	}

	/* repeate operation until the standard macro feof(fi) detects end-of-file on the input file */
	while (!feof(fi)) {
		/* read one character at a time */
		buffer = (char)fgetc(fi);
		/* to check the end of the file */
		if (feof(fi)) {
			break;
		}

		/* if current character cannot be put in the buffer */
		if (!b_addc(pBD, buffer)) {
			return LOAD_FAIL;
		}
		count++;
	}
	return count;
}

/*
* Purpose: The function check the buffer is empty or not
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: -1 (R_FAIL1), 0 (FALSE), 1 (TRUE)
* Algorithm: 1. check the value of addc_offset 
             2. check the run-time error
*/
int b_isempty(Buffer * const pBD) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}
	/* check the value of addc_offset */
	if (pBD->addc_offset == OFFSET_RESET) {
		return TRUE;
	}
	else {
		return FALSE;
	}	
}

/*
* Purpose: The function returns eob to the calling function
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: 1, 0, -1 (R_FAIL1)
* Algorithm: 1. returns eob to the calling function
             2. check the run-time error
*/
int b_eob(Buffer * const pBD) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}
	return (unsigned int)pBD->eob;
}

/*
* Purpose: The function gets the character symbol from buffer
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: char, -2 (R_FAIL2), -1 (R_FAIL1)
* Algorithm: 1. checks the argument for validity (possible run-time error).
             2. check if getc_offset and addc_offset are equal
			 3. returns the character located at getc_offset
*/
char b_getc(Buffer * const pBD) {
	/* If run-time error, returns -2 */
	if (!pBD) {
		return RT_FAIL2;
	}

	/* make sure the buffer is not at the end of the read offset */
	if (pBD->getc_offset == pBD->addc_offset) {
		pBD->eob = SET_EOB_FLAG;
		return RT_FAIL1;
	}
	else {
		pBD->eob = UNSET_EOB_FLAG;
	}

	/* returns the character located at getc_offset. */
	return pBD->cb_head[pBD->getc_offset++];
}

/*
* Purpose: The function is intended to be used for used for diagnostic purposes only.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: b_isempty(), b_getc(), b_eob(), printf()
* Parameters: Buffer * const pBD
* Return value: int, -1 (R_FAIL1)
* Algorithm: 1. checks if the buffer is empty 
             2. prints character by character the contents of the buffer
			 3. detect the end of the buffer content
			 4. prints a new line character
*/
int b_print(Buffer * const pBD) {
	/* buffer character */
	char buffer; 
	/* catch the character counter */
	int count = 0; 

	/* If run-time error, returns -1 */
	if (!pBD || !pBD->cb_head) {
		return RT_FAIL1;
	}

	/* check the buffer is empty or not */
	if (b_isempty(pBD) == TRUE) {
		printf("Empty buffer\n");
	}

	/* prints the content calling b_getc() in a loop */
	do {
		buffer = b_getc(pBD);

		/* detect the end of the buffer content */
		if (b_eob(pBD)) {
			break;
		}

		/* prints a new line character */
		printf("%c", (char)buffer);
		count++;
	} while (!b_eob(pBD));

	printf("\n");
	
	return count;
}

/*
* Purpose: For all operational modes, the function shrinks buffer to new_capacity.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: realloc()
* Parameters: Buffer * const pBD, char symbol
* Return value: NULL, pBD
* Algorithm: 1. define and adjust the memory of the new capacity
             2. updates all the necessary members of the buffer
			 3. adds the symbol to the end of the buffer
			 4. set the buffer r_flag appropriately
*/
Buffer * b_compact(Buffer * const pBD, char symbol) {
    /* variables used to shrinks buffer to new capacity */
	short new_capacity;
	/* variables needed to check the memory location */
	char *old_add;
	char *temp_add;

	/* if cannot to perform the required operation */
	if (!pBD || !pBD->cb_head) {
		return NULL;
	}

	/* set the new capacity */
	new_capacity = pBD->addc_offset + 1;

	/* if underflow, new_capacity returns NULL */
	if (new_capacity <= 0) {
		return NULL;
	}

	/* Update all the necessary members of the buffer */
	old_add = pBD->cb_head; 
	/* use realloc() to adjust the new capacity */
	temp_add = realloc(pBD->cb_head, sizeof(char) * new_capacity);
	/* If realloc failed */
	if (!temp_add) {
		return NULL;
	}
	pBD->cb_head = temp_add;
	pBD->capacity = new_capacity;

	/* add symbol to the end of the buffer */
	pBD->cb_head[pBD->addc_offset++] = symbol;
	/* compare the address in the memory and set the r_flag */
	pBD->r_flag = (pBD->cb_head == old_add);

	return pBD;
}

/*
* Purpose: The function returns r_flag to the calling function
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: r_flag, RT_FAIL1
* Algorithm: 1. returns r_flag to the calling function
             2. check the run-time error
*/
char b_rflag(Buffer * const pBD) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}
    return pBD->r_flag;
}

/*
* Purpose: The function decrements getc_offset by 1.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: getc_offset, RT_FAIL1
* Algorithm: 1. decrements getc_offset by 1.
             2. check the run-time error
*/
short b_retract(Buffer * const pBD) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}
	else if (pBD->getc_offset > OFFSET_RESET) {
		pBD->getc_offset--;
	}

	return pBD->getc_offset;
}

/*
* Purpose: The function sets getc_offset to the value of the current markc_offset
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: getc_offset, RT_FAIL1
* Algorithm: 1. set the value of getc_offset to current markc_offset
             2. check the run-time error
*/
short b_reset(Buffer * const pBD) {
	
	/* If run-time error, returns -1 */
	if (!pBD || pBD->markc_offset < OFFSET_RESET || pBD->markc_offset > pBD->capacity) {
		return RT_FAIL1;
	}

	/* sets value of getc_offset to current markc_offset */
	pBD->getc_offset = pBD->markc_offset;
	
	return pBD->getc_offset;
}

/*
* Purpose: The function returns getc_offset to the calling function.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: getc_offset, RT_FAIL1
* Algorithm: 1. returns getc_offset to the calling function
             2. check the run-time error
*/
short b_getcoffset(Buffer * const pBD) {
	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}

	return pBD->getc_offset;
}

/*
* Purpose: The function makes the buffer can be reread again.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD
* Return value: 0, RT_FAIL1
* Algorithm: 1. set the getc_offset and markc_offset to 0
             2. check the run-time error
*/
int b_rewind(Buffer * const pBD) {
	/* set the getc_offset and markc_offset to 0 to reread buffer */
	pBD->getc_offset = OFFSET_RESET;
	pBD->markc_offset = OFFSET_RESET;

	/* If run-time error, returns -1 */
	if (!pBD) {
		return RT_FAIL1;
	}
	else {
		return 0;
	}
}

/*
* Purpose: The function returns a pointer to a location of the character buffer indicated by loc_offset.
* Author: Wenjing Wang
* Versions: 1.17.2 2017-09-29
* Called Functions: nothing
* Parameters: Buffer * const pBD, short loc_offset
* Return value: NULL, loc_offset
* Algorithm: 1. get a pointer to a location of buffer
             2. check the run-time error
*/
char * b_location(Buffer * const pBD, short loc_offset) {
	/* If run-time error, returns NULL */
	if (!pBD) {
		return NULL;
	}
	if (loc_offset < 0 || loc_offset >= pBD->addc_offset) {
		return NULL;
	}

	return pBD->cb_head + loc_offset;;
}