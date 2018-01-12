/*
* File Name: scanner.c
* Version: 1.17.2
* Compiler: MS Visual Studio 2015
* Author: Wenjing Wang, ID# 040812907
* Course: CST8152 - Compiler, Lab Section: 013
* Assignment: 2
* Date: November 30, 2017
* Professor: Svillen Ranev
* Purpose: Functions implementing a Lexical Analyzer (Scanner).
*
* Function list: scanner_init(), malar_next_token(), char_class(), get_next_state(), iskeyword(), atolh()
aa_func02(), aa_func03(), aa_func05(), aa_func08(), aa_func11(), aa_func12()
*/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
It is defined in platy_st.c */
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

						 /* Local(file) global objects - variables */
static Buffer *lex_buf;  /*pointer to temporary lexeme buffer*/

						 /* No other global variable declarations/definitiond are allowed */

						 /* scanner.c static(local) function  prototypes */
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */
static long atolh(char * lexeme); /* converts hexadecimal string to decimal value */

								  /*
								  * Purpose: This function Initializes scanner
								  * Author: Wenjing Wang
								  * Versions: 1.17.2
								  * Called Functions: b_isempty(), b_clear()
								  * Parameters: Buffer * sc_buf
								  * Return value: EXIT_FAILURE and EXIT_SUCCESS
								  * Algorithm: initialize scanner
								  */
int scanner_init(Buffer * sc_buf) {
	if (b_isempty(sc_buf)) return EXIT_FAILURE;/*1*/
											   /* in case the buffer has been read previously  */
	b_rewind(sc_buf);
	b_clear(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;/*0*/
						/*   scerrnum = 0;  *//*no need - global ANSI C */
}

/*
* Purpose: Perform the token recognition
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions:  strcpy(),  b_getc(), b_retract(), b_mark(), b_reset(),
b_getcoffset(), b_limit(), b_addc(), isalpha(),  isdigit(),
get_next_state(), b_allocate(), b_location(), b_free()
* Parameters: Buffer * sc_buf
* Return value: token t
* Algorithm: 1. scan characters in different token
2. perform the transitation table
*/
Token malar_next_token(Buffer * sc_buf)
{
	Token t = { 0 };  /* token to return after recognition */
	unsigned char c;  /* input symbol */
	int state = 0;    /* initial state of the FSM */
	short lexstart;   /*start offset of a lexeme in the input char buffer (array) */
	short lexend;     /*end offset of a lexeme in the input char buffer (array)*/
	int accept = NOAS; /* type of state - initially not accepting */
	int i = 0;        /* the index for loop */
	char temp_c;      /* temporary character holder */
					  /* check is the buffer is NULL, error situation */

	while (1) { /* endless loop broken by token returns it will generate a warning */
				/* get the next symbol from the input buffer */
		c = b_getc(sc_buf);

		/* check for SEOF */
		if (c == SEOF || c == '\0') {
			t.code = SEOF_T;
			return t;
		}

		/* space character */
		if (isspace(c)) {  /* isspace() detects ' ', '\t', '\v', '\n', '\f', '\r' */
			if (c == '\n') ++line; 
			continue;
		}

		/* if it is a letter or a number */
		// if (isalpha(c) || isalnum((int)c)) {
		if (isalnum((int)c) != 0) {
			lexstart = b_mark(sc_buf, b_getcoffset(sc_buf) - 1);
			state = get_next_state(state, c, &accept);

			/* go through the accepting state until not accepting state */
			while (accept == NOAS) {
				c = b_getc(sc_buf);
				state = get_next_state(state, c, &accept);
			}
#ifdef DEBUG
			printf("state= %d, accept= %d\n", state, accept);
#endif 
			/* to check if need to retract */
			if (accept == ASWR) {
				b_retract(sc_buf);
			}

			lexend = b_getcoffset(sc_buf);
			// lex_buf = b_allocate(200, 10, 'a');
			lex_buf = b_allocate(lexend - lexstart, 0, 'f');

			/* to check if the buffer is empty */
			if (!lex_buf) {
				t.code = ERR_T;
				scerrnum = 1;
				strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");
				return t;
			}
			b_reset(sc_buf);

			/* to go through the character and add it into Buffer */
			while (b_getcoffset(sc_buf) < lexend) {
				c = b_getc(sc_buf);
				b_addc(lex_buf, c);
			}
			b_compact(lex_buf, '\0');

			t = aa_table[state](b_location(lex_buf, 0));
			b_free(lex_buf);
			return t;
		}

		/* special cases or token driven processing */
		switch (c) {
		case ';': t.code = EOS_T; return t; /* End of statement */
		case ',': t.code = COM_T; return t; /* Comma */
		case '{': t.code = LBR_T; return t; /* Left brace */
		case '}': t.code = RBR_T; return t; /* Right brace */
		case '(': t.code = LPR_T; return t; /* Left parenthesis */
		case ')': t.code = RPR_T; return t; /* Right parenthesis */
		case '#': t.code = SCC_OP_T; return t; /* strong concatination token */
		case '+': t.code = ART_OP_T; t.attribute.arr_op = PLUS; return t;   /* Addition operator */
		case '-': t.code = ART_OP_T; t.attribute.arr_op = MINUS; return t; /* Substraction operator */
		case '*': t.code = ART_OP_T; t.attribute.arr_op = MULT; return t; /* Multiplication operator */
		case '/': t.code = ART_OP_T; t.attribute.arr_op = DIV; return t; /* Devision operator */
		case '>': t.code = REL_OP_T; t.attribute.rel_op = GT; return t; /* Greater-than relational operator */
		case '<':
			c = b_getc(sc_buf);
			/* if not equal or if less than token */
			if (c == '>') {
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
				return t;
			}
			else {
				b_retract(sc_buf);
				t.code = REL_OP_T;
				t.attribute.rel_op = LT;
				return t;
			}

		case '=':
			c = b_getc(sc_buf);
			/* if '=' or '==' sign, set tokens */
			if (c == '=') {
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
				return t;
			}
			b_retract(sc_buf);
			t.code = ASS_OP_T;
			return t;

		case '.':
			/* set the current mark */
			b_mark(sc_buf, b_getcoffset(sc_buf));
			c = b_getc(sc_buf);

			/* if 'AND' or 'OR' or the wrong tokens */
			if (c == 'A' && b_getc(sc_buf) == 'N' && b_getc(sc_buf) == 'D' && b_getc(sc_buf) == '.') {
				t.code = LOG_OP_T;
				t.attribute.log_op = AND;
				return t;
			}
			else if (c == 'O' && b_getc(sc_buf) == 'R' && b_getc(sc_buf) == '.') {
				t.code = LOG_OP_T;
				t.attribute.log_op = OR;
				return t;
			}
			else {
				b_reset(sc_buf);
				t.code = ERR_T;
				t.attribute.err_lex[0] = '.';
				t.attribute.err_lex[1] = '\0';
				return t;
			}

		case '!':
			temp_c = c = b_getc(sc_buf);

			/* if is comment or error */
			if (temp_c != '!') {
				/* error state */
				b_retract(sc_buf);
				t.code = ERR_T;
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[1] = temp_c;
				t.attribute.err_lex[2] = '\0';
			}

			/* go through the loop until find a new line */
			while (c != '\n') {
				c = b_getc(sc_buf);

				/* if go to the end of the file */
				if (c == '\0' || c == SEOF) {
					t.code = SEOF_T;
					return t;
				}
			}
			line++;
			if (temp_c != '!')
				return t;
			continue;

		case '"':
			lexstart = b_mark(sc_buf, b_getcoffset(sc_buf));

			/* go through the string */
			for (;;) {
				c = b_getc(sc_buf);

				/* if character is a new line*/
				if (c == '\n') ++line;

				/* if character is the end of the '"' sign */
				if (c == '"') {
					t.code = STR_T;
					t.attribute.str_offset = b_limit(str_LTBL);

					lexend = b_getcoffset(sc_buf) - 1;

					/* if the string is not empty */
					if (lexend != lexstart) {
#ifdef DEBUG
						printf("lexstart= %d\t lexend= %d\n", lexstart, lexend);
#endif 
						b_reset(sc_buf);

						/* go through the string and store it into string literal table */
						for (i = lexstart; i < lexend; ++i) {
							b_addc(str_LTBL, b_getc(sc_buf));
						}
						b_getc(sc_buf);
					}
					b_addc(str_LTBL, '\0');
					return t;
				}

				/* if character is the end of the file */
				if (c == SEOF || c == '\0') {
					lexend = b_getcoffset(sc_buf);
					// lex_buf = b_allocate(lexstart - lexend, 0, 'a');
					lex_buf = b_allocate(lexend - lexstart, 0, 'f');

					/* if the buffer is empty, error */
					if (!lex_buf) {
						scerrnum = 1;
						t.code = ERR_T;
						strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");
						return t;
					}

					b_mark(sc_buf, lexstart - 1);
					b_reset(sc_buf);

					/* go through the string and store it into buffer */
					for (i = lexstart; i < lexend; ++i) {
						b_addc(lex_buf, b_getc(sc_buf));
					}
					b_compact(lex_buf, '\0');
					b_getc(sc_buf);

					t = aa_table[12](b_location(lex_buf, 0));
					free(lex_buf);
					return t;
				}
			}

		default:
			/* check illegal characters */
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = '\0';
			return t;
		}
	}
}

/*
* Purpose: to get the character's next state from the scanner
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: char_class()
* Parameters: int state, char c, int *accept
* Return value: next - next state
* Algorithm: 1.  to check the state of the next and return its state
*/
int get_next_state(int state, char c, int *accept)
{
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif				 
	assert(next != IS);

#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	*accept = as_table[next];
	return next;
}

/*
* Purpose: This function return the column index for the column in the transition table
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions:
* Parameters: char c - the input character be matched in the transition table
* Return value: int val - the value represente the column in the transition table
* Algorithm: 1. scan the input character
2. match character in the transition table
*/
int char_class(char c)
{
	int val;

	if (isalpha(c)) {
		if (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F') {
			val = 0;
		}
		else if (c == 'x') {
			val = 1;
		}
		else {
			val = 2;
		}
	}
	else if (c == '0')
		val = 3;
	else if (isalnum(c))
		val = 4;
	else if (c == '.')
		val = 5;
	else if (c == '$')
		val = 6;
	else
		val = 7;

	return val;
}

/*
* Purpose: to accept function for the arithmentic variable identifier and keywords
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: iskeyword(), strlen(), strcpy()
* Parameters: char lexeme
* Return value: Token t
* Algorithm: 1. check if lexeme is keyword
2. if yes, return correspond token attribute
3. set a AVID token
4. check if lexeme is longer than VID_LEN
*/
Token aa_func02(char *lexeme) {
	Token t;
	int num = iskeyword(lexeme);
	int i;

#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif

	/* To check if the lexeme is a keyword */
	if (num != -1) {
		t.code = KW_T;
		t.attribute.kwt_idx = num;
		return t;
	}
	else {
		t.code = AVID_T;
	}

	/* if the length longer than VID_LEN */
	if (strlen(lexeme) > VID_LEN) {
		/* to store the character into variable strong array */
		for (i = 0; i < VID_LEN; ++i) {
			t.attribute.vid_lex[i] = lexeme[i];
		}
		t.attribute.vid_lex[VID_LEN] = '\0';

	}
	else {
		strcpy(t.attribute.vid_lex, lexeme);
	}

	return t;
}

/*
* Purpose: to accept function for the string variable identifier (VID - SVID)
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: strlen(), strcpy()
* Parameters: char lexeme
* Return value: Token t
* Algorithm: 1. check if the length of lexeme is longer than VID_LEN characters
2. set token token and attribute in different situation
*/
Token aa_func03(char *lexeme) {
	Token t;
	int length = strlen(lexeme);
	int i;

#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif

	t.code = SVID_T;
	/* the lexeme length longer than VID_LEN */
	if (length > VID_LEN) {
		strncpy(t.attribute.vid_lex, lexeme, (VID_LEN - 1));

		/* to store the character into variable strong array */
		for (i = 0; i < VID_LEN; i++) {
			t.attribute.vid_lex[i] = lexeme[i];
		}
		/* Add the $ character appended to the name */
		t.attribute.vid_lex[VID_LEN - 1] = '$';
		t.attribute.vid_lex[VID_LEN] = '\0';

	}
	else {
		strncpy(t.attribute.vid_lex, lexeme, length);
		t.attribute.err_lex[length] = '\0';
	}
	return t;
}

/*
* Purpose: to accept function for the integer literal(IL) - decimal constant (DIL)
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: strlen(), strcpy(), atolh()
* Parameters: char lexeme
* Return value: Token t
* Algorithm: 1. check if the range of lexeme is 2-byte integer
2. check if the length of character is valid or not
3. set token token and attribute in different situation
*/
Token aa_func05(char *lexeme) {
	Token t;
	long num = atol(lexeme);

#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif

	/* convert a decimal constant to a decimal integer value */
	if (num > SHRT_MAX || num < SHRT_MIN) {
		t = aa_table[12](lexeme);
	}
	else {
		t.code = INL_T;
		t.attribute.int_value = num;
	}
	return t;
}
/*
* Purpose: accept function for the floating-point literal (FPL)
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: strlen(), strcpy(), atolh()
* Parameters: char lexeme
* Return value: Token t
* Algorithm: 1. check the range of float data type
2. check if the length of character is valid or not
3. set token token and attribute in different situation
*/
Token aa_func08(char *lexeme) {
	Token t = { 0 };
	float num = strtof(lexeme, NULL);

#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif

	/* the value must in the same range as the value of 4-byte float */
	if (((num >= 0 && strlen(lexeme) > 7) && (num < FLT_MIN || num > FLT_MAX)) || (num < 0)) {
		t = aa_table[12](lexeme);
	}
	else {
		t.code = FPL_T;
		t.attribute.flt_value = num;
	}
	return t;
}
/*
* Purpose: accepting function for the integer literal(IL) - hexadecimal constant (HIL)
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: strlen(), strcpy(), atolh()
* Parameters: char lexeme
* Return value: Token t
* Algorithm: 1. check the range of short data type
2. check if the length of character is valid or not
3. set token token and attribute in different situation
*/
Token aa_func11(char *lexeme) {
	Token t;
	long hex = atolh(lexeme);

#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif

	/* the value must in the smae range as the value of 2-byte integer */
	if (hex < SHRT_MIN || hex > SHRT_MAX) {
		t = aa_table[12](lexeme);
	}
	else {
		t.code = INL_T;
		t.attribute.int_value = hex;
	}
	return t;
}
/*
* Purpose: accepting function for the ERROR TOKEN
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: strlen(), strcpy(), atolh()
* Parameters: char lexeme
* Return value: Token t
* Algorithm: 1. set token code
2. check if the length of character is valid or not
3. set token attribute in different situation
*/
Token aa_func12(char *lexeme) {
	Token t;
	int length = strlen(lexeme);

#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif

	t.code = ERR_T;
	/* ERROR lexeme is longer than ERR_LEN characters */
	if (length > ERR_LEN) {
		strncpy(t.attribute.err_lex, lexeme, ERR_LEN - 3);
		t.attribute.err_lex[ERR_LEN - 3] = '.';
		t.attribute.err_lex[ERR_LEN - 2] = '.';
		t.attribute.err_lex[ERR_LEN - 1] = '.';
		t.attribute.err_lex[ERR_LEN] = '\0';
	}
	else {
		/* ERROR lexeme is not longer than ERR_LEN characters */
		strncpy(t.attribute.err_lex, lexeme, length);
		t.attribute.err_lex[length] = '\0';
	}

	return t;
}

/*
* Purpose: Convert an ASCII string representing an Hexadecimal integer
constant to integer value.
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: strtol()
* Parameters: char* lexeme: the string to convert to hex
* Return value: hex - integer representation of the hex string
* Algorithm: call the strtol function to convert character into hex
*/
long atolh(char *lexeme) {
	long hex;

	/* to check if lexeme is NULL */
	if (lexeme == NULL) {
		return 0;
	}

	/* convert the initial part of the string to a long int according to base */
	hex = strtol(lexeme, NULL, 16);
	return hex;
}

/*
* Purpose: Looks up the string pattern on the keyword table
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: strcmp()
* Parameters: char* lexeme: the string pattern to look up in kw_table
* Return value: int -1 - can't find a match,
int i - index location of the matching keyword
* Algorithm: compare character with kw_table to find the keyword
*/
int iskeyword(char * kw_lexeme) {
	int i;

	/* to check if lexeme is NULL */
	if (kw_lexeme == NULL)
		return -1;

	/* to go through the keyword array and match the keyword */
	for (i = 0; i < KWT_SIZE; i++) {
		if (strcmp(kw_table[i], kw_lexeme) == 0) {
			return i;
		}
	}

	return -1;
}