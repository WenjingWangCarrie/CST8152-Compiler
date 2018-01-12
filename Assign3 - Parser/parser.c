/*
* File Name: parser.c
* Version: 1.17.2
* Compiler: MS Visual Studio 2015
* Author: Wenjing Wang, ID# 040812907
* Course: CST8152 - Compiler, Lab Section: 013
* Assignment: 3
* Date: December 30, 2017
* Professor: Svillen Ranev
* Purpose: To parse the tokens in the file
* Function list: parser(Buffer*), match(int, int ), syn_eh(int), syn_printe(), gen_incode(char*), program(void), opt_statements(), statements(),
* statement(), statements_p(), assignment_statement(), assignment_expression(), conditional_expression(), logical_OR_expression(), 
* logical_OR_expression_p(), logical_AND_expression(), logical_AND_expression_p(), relational_expression(), primary_a_relational_expression(),
* primary_a_relational_expression_p(), primary_s_relational_expression(), primary_s_relational_expression_p(), arithmetic_expression(),
* unary_arithmetic_expression(), additive_arithmetic_expression(), primary_aithmetic_expression(), additive_arithmetic_expression_p(), 
* multiplicative_arithmetic_expression(), multiplicative_arithmetic_expression_p(), string_expression(), primary_string_expression(), string_expression_p(), 
* selection_statement(), variable_list(), variable_list_p(), variable_identifier(), iteration_statement(), input_statement(), output_statement(), output_list()
*/

#include <stdlib.h>
#include "parser.h"

/*
* Purpose: This function begin the parsing
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: malar_next_token(), program(), match(), gen_incode()
* Parameters: Buffer * sc_buf
* Return value: N/A
* Algorithm: start the program
*/
void parser(Buffer* in_buff) {
	sc_buf = in_buff;
	lookahead = malar_next_token(sc_buf);
	program();
	match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}

/*
* Purpose: To match two tokens: the current input token (lookahead)
*          and the token required by the parser.
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: syn_eh(), syn_printe(), malar_next_token()
* Parameters: int pr_token_code, int pr_token_attribute
* Return value: returns to the program
* Algorithm: 1. to check if the match is successful or not
*            2. to parse the token code and check if token is SEOF_T or not
*			 3. to check the lookahead token is ERR_T or not
*/
void match(int pr_token_code, int pr_token_attribute) {
	/* If the match is unsuccessful, the function calls the error handler */
	if (lookahead.code != pr_token_code) {
		syn_eh(pr_token_code);
		return;
	}
	
	/* If the match is successful and the lookahead is SEOF_T */
	if (lookahead.code == SEOF_T)
		return;

	/* to parse the token code */
	switch (pr_token_code) {
	case KW_T:
		if (pr_token_attribute == 1 || pr_token_attribute == 7)
			break;

		if (pr_token_attribute != lookahead.attribute.kwt_idx) {
			syn_eh(pr_token_code);
			return;
		}
		break;

	case LOG_OP_T:
	case ART_OP_T:
	case REL_OP_T:
		/* if the required token doesn't match the token attribute */
		if (pr_token_attribute != lookahead.attribute.get_int) {
			syn_eh(pr_token_code);
			return;
		}
		break;
	}

	/* the match is successful and not SEOF_T, advance to next token */
	lookahead = malar_next_token(sc_buf);
	/* If the new lookahead token is ERR_T*/
	if (lookahead.code == ERR_T) {
		syn_printe();
		lookahead = malar_next_token(sc_buf);
		++synerrno;
		return;
	}
}

/*
* Purpose: To implements a simple panic mode error recovery.
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: syn_printe(), malar_next_token(), exit()
* Parameters: int sync_token_code
* Return value: return to the program
* Algorithm: 1. count the error and printing
*            2. advance the input token
*		     3. check if the function reach the end of file
*    		 4. find the matching token
*/
void syn_eh(int sync_token_code) {
	/* count error and printing */
	syn_printe();
	++synerrno;

	/* implements a panic mode error recovery */
	do {
		/* to advance the input token */
		lookahead = malar_next_token(sc_buf);

		/* it finds the token code matching sync_token_code */
		if (lookahead.code == sync_token_code) {
			lookahead = malar_next_token(sc_buf);
			return;
		}

		/* If sync_token_code different from SEOF_T and reaches end of the source file */
		if (lookahead.code == SEOF_T) {
			exit(synerrno);
			return;
		}
	} while (lookahead.code != sync_token_code);
}

/*
* Purpose: To print the error messages
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: printf()
* Parameters: N/A
* Return value: N/A
* Algorithm: look for different situations, print error messages
*/
void syn_printe() {
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case  ERR_T: /* ERR_T     0   Error token */
		printf("%s\n", t.attribute.err_lex);
		break;
	case  SEOF_T: /*SEOF_T    1   Source end-of-file token */
		printf("NA\n");
		break;
	case  AVID_T: /* AVID_T    2   Arithmetic Variable identifier token */
	case  SVID_T:/* SVID_T    3  String Variable identifier token */
		printf("%s\n", t.attribute.vid_lex);
		break;
	case  FPL_T: /* FPL_T     4  Floating point literal token */
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: /* INL_T      5   Integer literal token */
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:/* STR_T     6   String literal token */
		printf("%s\n", b_location(str_LTBL, t.attribute.str_offset));
		break;

	case SCC_OP_T: /* 7   String concatenation operator token */
		printf("NA\n");
		break;

	case  ASS_OP_T:/* ASS_OP_T  8   Assignment operator token */
		printf("NA\n");
		break;
	case  ART_OP_T:/* ART_OP_T  9   Arithmetic operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T: /*REL_OP_T  10   Relational operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:/*LOG_OP_T 11  Logical operator token */
		printf("%d\n", t.attribute.get_int);
		break;

	case  LPR_T: /*LPR_T    12  Left parenthesis token */
		printf("NA\n");
		break;
	case  RPR_T: /*RPR_T    13  Right parenthesis token */
		printf("NA\n");
		break;
	case LBR_T: /*    14   Left brace token */
		printf("NA\n");
		break;
	case RBR_T: /*    15  Right brace token */
		printf("NA\n");
		break;

	case KW_T: /*     16   Keyword token */
		printf("%s\n", kw_table[t.attribute.get_int]);
		break;

	case COM_T: /* 17   Comma token */
		printf("NA\n");
		break;
	case EOS_T: /*    18  End of statement *(semi - colon) */
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}/*end switch*/
}/* end syn_printe()*/

 /*
 * Purpose: print the string argument
 * Author: Wenjing Wang
 * Versions: 1.17.2
 * Called Functions: printf()
 * Parameters: char *string
 * Return value: N/A
 * Algorithm: takes a string as an argument and prints it
 */
void gen_incode(char* string) {
	/* takes a string as an argument and prints it */
	printf("%s\n", string);
}

/*
* Purpose: <program> -> PLATYPUS {<opt_statements>} SEOF
*          FIRST(<program>) = { KW_T(PLATYPUS) }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: match(), opt_statements(), gen_incode()
* Parameters: void
* Return value: N/A
* Algorithm: 1. match the tokens
*            2. print message
*/
void program(void) {
	match(KW_T, PLATYPUS); 
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	gen_incode("PLATY: Program parsed");
}

/*
* Purpose: <opt_statements> -> <statements> | e
* FIRST(<opt_statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE), e }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: statements(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: 1. looking for the tokens
*			 2. chack for some keyword
*/
void opt_statements() {
	/* FIRST set: {AVID_T,SVID_T,KW_T(but not … see above),e} */
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T: 
		statements();
		break;

	case KW_T:
		/* check for PLATYPUS, ELSE, THEN, REPEAT here and in statements_p() */
		if (lookahead.attribute.get_int != PLATYPUS
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != REPEAT) {
			statements();
			break;
		}

	default: /* empty string – optional statements */;
		gen_incode("PLATY: Opt_statements parsed");
	}
}

/*
* Purpose: <statements> -> <statement><statements'>
* FIRST(<statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE) }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: statement(), statements_p()
* Parameters: N/A
* Return value: N/A
* Algorithm: loop the statement() and statements_p() function
*/
void statements(void) {
	statement();
	statements_p();
}

/*
* Purpose: <statement> -> <assignment statement>
*						| <selection statement>
*						| <iteration statement>
*						| <input statement>
*						| <output statement>
* FIRST(<statement>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE) }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: assignment_statement(), selection_statement(), iteration_statement(),
*                   input_statement(), output_statement(), syn_printe()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the statement FIRST Set to find tokens
*/
void statement(void) {
	/* look for the tokens */
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T: 
		assignment_statement();
		break;

	case KW_T:
		/* look for the keyword token */
		if (lookahead.attribute.get_int == IF)
			selection_statement();
		else if (lookahead.attribute.get_int == WHILE)
			iteration_statement();
		else if (lookahead.attribute.get_int == READ)
			input_statement();
		else if (lookahead.attribute.get_int == WRITE)
			output_statement();
		break;

	default: 
		syn_printe();
	}
}

/*
* Purpose: <statements'> -> <statement><statements'> | e
* FIRST(<statements’>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE) , e }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: statement(), statements_p(), statements()
* Parameters: N/A
* Return value: N/A
* Algorithm: loop the statement() and statements_p() function
*/
void statements_p(void) {
	/* look for the tokens */
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		statement();
		statements_p();
		break;

	case KW_T:
		/* check for PLATYPUS, ELSE, THEN, REPEAT here */
		if (lookahead.attribute.get_int != PLATYPUS
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != REPEAT) {
			statements();
			break;
		}

	default: /*empty string – optional statements*/
		break;
	}
}

/*
* Purpose: <assignment statement> -> <assignment expression>;
* FIRST(<assignment statement>) = { AVID_T, SVID_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: assignment_expression(), match(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the assignment_statement FIRST Set
*/
void assignment_statement(void) {
	assignment_expression();
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Assignment statement parsed");
}

/*
* Purpose: <assignment expression> -> AVID = <arithmetic expression>
*   								| SVID = <string expression>
* FIRST(<assignment expression>) = { AVID_T, SVID_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: assignment_expression(), match(), string_expression(), gen_incode(), syn_printe()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the assignment_statement FIRST Set
*/
void assignment_expression(void) {
	/* look for the token */
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR);
		match(ASS_OP_T, EQ);
		arithmetic_expression();
		gen_incode("PLATY: Assignment expression (arithmetic) parsed");
		break;

	case SVID_T:
		match(SVID_T, NO_ATTR);
		match(ASS_OP_T, EQ);
		string_expression();
		gen_incode("PLATY: Assignment expression (string) parsed");
		break;

	default:
		syn_printe();
	}
}

/*
* Purpose: <arithmetic expression> -> <unary arithmetic expression>
*									| <additive arithmetic expression>
* FIRST(<arithmetic expression>) = { -, +, AVID_T, FPL_T, INL_T, ( }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: gen_incode(), syn_printe(), unary_arithmetic_expression(), additive_arithmetic_expression()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the arithmetic_expression FIRST Set
*/
void arithmetic_expression(void) {
	/* look for the token */
	switch (lookahead.code) {
	case ART_OP_T:
		/* the attribute should be MULT adn DIV */
		switch (lookahead.attribute.arr_op) {
		case PLUS:
		case MINUS:
			unary_arithmetic_expression();
			break;

		default:
			syn_printe();
			break;
		}

		gen_incode("PLATY: Arithmetic expression parsed");
		break;

	case AVID_T:
	case FPL_T:
	case INL_T:
	case LPR_T:
		additive_arithmetic_expression();
		gen_incode("PLATY: Arithmetic expression parsed");
		break;

	default:
		syn_printe();
		break;
	}
}

/*
* Purpose: <unary arithmetic expression> -> - <primary arithmetic expression>
*										  | + <primary arithmetic expression>
* FIRST(<unary arithmetic expression>) = { -, + }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: gen_incode(), syn_printe(), match(), primary_arithmetic_expression()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the unary_arithmetic_expression FIRST Set
*/
void unary_arithmetic_expression(void) {
	/* look for the token */
	switch (lookahead.code) {
	case ART_OP_T:
		/* the attribute should be PLUS and MINUS */
		switch (lookahead.attribute.arr_op) {
		case PLUS:
			match(ART_OP_T, PLUS);
			primary_arithmetic_expression();
			break;

		case MINUS:
			match(ART_OP_T, MINUS);
			primary_arithmetic_expression();
			break;

		default:
			syn_printe();
			break;
		}

	default:
		break;
	}
	gen_incode("PLATY: Unary arithmetic expression parsed");
}

/*
* Purpose: <additive arithmetic expression> -> <multiplicative arithmetic expression> <additive arithmetic expression’>
* FIRST(<additive arithmetic expression>) = { AVID_T, FPL_T, INL_T, ( }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: multiplicative_arithmetic_expression(), additive_arithmetic_expression_p()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the additive_arithmetic_expression FIRST Set
*/
void additive_arithmetic_expression(void) {
	multiplicative_arithmetic_expression();
	additive_arithmetic_expression_p();
}

/*
* Purpose: <additive arithmetic expression’> -> + <multiplicative arithmetic expression> <additive arithmetic expression’>
*										      | - <multiplicative arithmetic expression> <additive arithmetic expression’>
* FIRST(<additive arithmetic expression’>) = { +, -, e }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: match(), multiplicative_arithmetic_expression(), additive_arithmetic_expression_p(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the additive_arithmetic_expression_p FIRST Set
*/
void additive_arithmetic_expression_p(void) {
	/* look for the tokens */
	switch (lookahead.code) {
	case ART_OP_T:
		/* the attribute should be PLUS and MINUS */
		switch (lookahead.attribute.arr_op) {
		case PLUS:
			match(ART_OP_T, PLUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_p();
			gen_incode("PLATY: Additive arithmetic expression parsed");
			break;

		case MINUS:
			match(ART_OP_T, MINUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_p();
			gen_incode("PLATY: Additive arithmetic expression parsed");
			break;

		default:
			syn_printe();
			break;
		}
	}
}

/*
* Purpose: <multiplicative arithmetic expression> -> <primary arithmetic expression> < multiplicative arithmetic expression’>
* FIRST(<multiplicative arithmetic expression>) = { AVID_T, FPL_T, INL_T, ( }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: primary_arithmetic_expression(), multiplicative_arithmetic_expression_p()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the multiplicative_arithmetic_expression FIRST Set
*/
void multiplicative_arithmetic_expression(void) {
	primary_arithmetic_expression();
	multiplicative_arithmetic_expression_p();
}

/*
* Purpose: <multiplicative arithmetic expression’> -> * < primary arithmetic expression> < multiplicative arithmetic expression’>
| / < primary arithmetic expression> < multiplicative arithmetic expression’>
* FIRST(<multiplicative arithmetic expression’>) = { *, /, e }

* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: match(), primary_arithmetic_expression(), multiplicative_arithmetic_expression_p(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the multiplicative_arithmetic_expression_p FIRST Set
*/
void multiplicative_arithmetic_expression_p(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case ART_OP_T:
		/* the attribute should be MULT and DIV */
		switch (lookahead.attribute.arr_op) {
		case MULT:
			match(ART_OP_T, MULT);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expression_p();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
			break;

		case DIV:
			match(ART_OP_T, DIV);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expression_p();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
			break;
		}
	}
}

/*
* Purpose: <primary arithmetic expression> -> AVID_T
*											| FPL_T
*											| INL_T
*											| (<arithmetic expression>)
* FIRST(<primary arithmetic expression>) = { AVID_T, FPL_T, INL_T, ( }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: gen_incode(), syn_printe(), match(), arithmetic_expression
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the primary_arithmetic_expression FIRST Set
*/
void primary_arithmetic_expression(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case AVID_T:
	case FPL_T:
	case INL_T:
		match(lookahead.code, NO_ATTR);
		break;

	case LPR_T:
		match(LPR_T, NO_ATTR);
		arithmetic_expression();
		match(RPR_T, NO_ATTR);
		break;
	}
	gen_incode("PLATY: Primary arithmetic expression parsed");
}

/*
* Purpose: <string expression> -> <primary string expression><string expression’>
* FIRST(<string expression>) = { SVID_T, STR_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: primary_string_expression(), string_expression_p(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the string_expression FIRST Set
*/
void string_expression(void) {
	primary_string_expression();
	string_expression_p();
	gen_incode("PLATY: String expression parsed");
}

/*
* Purpose: <string expression’> -> # <primary string expression> <string expression’> | e
* FIRST(<string expression’>) = { #, e }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: primary_string_expression(), string_expression_p(), match()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the string_expression_p FIRST Set
*/
void string_expression_p(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case SCC_OP_T:
		match(SCC_OP_T, NO_ATTR);
		primary_string_expression();
		string_expression_p();
		break;

	default:
		break;
	}
}

/*
* Purpose: <primary string expression> -> SVID_T
*										| STR_T
* FIRST(<primary string expression>) = { SVID_T, STR_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: match(), syn_printe(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the primary_string_expression FIRST Set
*/
void primary_string_expression(void) {
	/* look for token */
	switch (lookahead.code) {
	case SVID_T:
		match(SVID_T, NO_ATTR);
		break;

	case STR_T:
		match(STR_T, NO_ATTR);
		break;
	}
	gen_incode("PLATY: Primary string expression parsed");
}

/*
* Purpose: <selection statement> -> IF TRUE (<conditional expression>) THEN { <opt_statements> }
*                                   ELSE { <opt_statements> } ;
* FIRST(<selection statement>) = { KW_T(IF) }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: match(), conditional_expression(), opt_statements(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the selection_statement FIRST Set
*/
void selection_statement(void) {
	match(KW_T, IF);
	match(KW_T, TRUE);
	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);

	match(KW_T, THEN);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);

	match(KW_T, ELSE);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Selection statement parsed");
}

/*
* Purpose: <conditional expression> -> <logical OR expression>
* FIRST(<conditional expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: logical_or_expression(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the conditional_expression FIRST Set
*/
void conditional_expression(void) {
	logical_or_expression();
	gen_incode("PLATY: Conditional expression parsed");
}

/*
* Purpose: <logical OR expression> -> <logical AND expression> <logical OR expression’>
* FIRST(<logical OR expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: logical_and_expression(), logical_or_expression_p()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the logical_or_expression FIRST Set
*/
void logical_or_expression(void) {
	logical_and_expression();
	logical_or_expression_p();
}

/*
* Purpose: <logical OR expression’> -> .OR. <logical AND expression> <logical OR expression’>
* FIRST(<logical OR expression’>) = { .OR. , e }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: logical_and_expression(), logical_or_expression_p(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the logical_or_expression_p FIRST Set
*/
void logical_or_expression_p(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case LOG_OP_T:
		/* the attribute logical operator should be OR */
		switch (lookahead.attribute.log_op) {
		case OR:
			match(LOG_OP_T, OR);
			logical_and_expression();
			logical_or_expression_p();
			gen_incode("PLATY: Logical OR expression parsed");
			break;

		default:
			break;
		}

	default:
		break;
	}
}

/*
* Purpose: <logical AND expression> -> <relational expression> <logical AND expression’>
* FIRST(<logical AND expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: relational_expression(), logical_and_expression_p()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the logical_and_expression FIRST Set
*/
void logical_and_expression(void) {
	relational_expression();
	logical_and_expression_p();
}

/*
* Purpose: <logical AND expression’> ->	.AND. <relational expression> <logical AND expression’>
* FIRST(<logical AND expression’>) = { .AND., e }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: relational_expression(), logical_and_expression_p(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the logical_and_expression_p FIRST Set
*/
void logical_and_expression_p(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case LOG_OP_T:
		/* the attribute logical operator should be AND */
		switch (lookahead.attribute.log_op)	{
		case AND:
			match(LOG_OP_T, AND);
			relational_expression();
			logical_and_expression_p();
			gen_incode("PLATY: Logical AND expression parsed");
			break;

		default:
			break;
		}

	default:
		break;
	}
}

/*
* Purpose: <relational expression> -> <primary a_relational expression> <primary a_relational expression’>
*									| <primary s_relational expression> <primary s_relational expression’>
* FIRST(<relational expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: primary_a_relational_expression(), primary_a_relational_expression_p(), gen_incode()
  primary_s_relational_expression(), primary_s_relational_expression_p(), syn_printe()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the relational_expression FIRST Set
*/
void relational_expression(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case AVID_T:
	case FPL_T:
	case INL_T:
		primary_a_relational_expression();
		primary_a_relational_expression_p();
		break;

	case SVID_T:
	case STR_T:
		primary_s_relational_expression();
		primary_s_relational_expression_p();
		break;

	default:
		syn_printe();
		break;
	}
	gen_incode("PLATY: Relational expression parsed");
}

/*
* Purpose: <primary a_relational expression> -> AVID_T
*											  | FPL_T
*											  | INL_T
* FIRST(<primary a_relational expression>) = { AVID_T, FPL_T, INL_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: match(), syn_printe(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the primary_a_relational_expression FIRST Set
*/
void primary_a_relational_expression(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case AVID_T:
	case FPL_T:
	case INL_T:
		match(lookahead.code, NO_ATTR);
		gen_incode("PLATY: Primary a_relational expression parsed");
		break;

	case SVID_T:
	case STR_T:
		syn_printe();
		gen_incode("PLATY: Primary a_relational expression parsed");
		break;

	default:
		syn_printe();
		break;

	}
}

/*
* Purpose: <primary a_relational expression’> -> == <primary a_relational expression>
*										       | <> <primary a_relational expression>
*										       | > <primary a_relational expression>
*										       | < <primary a_relational expression>
* FIRST(<primary a_relational expression’>) = { ==, <>, >, < }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: match(), primary_a_relational_expression(), syn_printe()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the primary_a_relational_expression_p FIRST Set
*/
void primary_a_relational_expression_p(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case REL_OP_T:
		switch (lookahead.attribute.rel_op) {
		case EQ:
		case NE:
		case GT:
		case LT:
			match(REL_OP_T, lookahead.attribute.rel_op);
			primary_a_relational_expression();
			break;

		default:
			syn_printe();
			break;
		}
	default:
		break;
	} 
}

/*
* Purpose: <primary s_relational expression> -> <primary string expression>
* FIRST(<primary s_relational_expression>) = { SVID_T, STR_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: primary_string_expression(), syn_printe(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the primary_s_relational_expression FIRST Set
*/
void primary_s_relational_expression(void) {
	/* look for the tokens */
	switch (lookahead.code) {
	case SVID_T:
	case STR_T:
		primary_string_expression();
		gen_incode("PLATY: Primary s_relational expression parsed");
		break;

	case AVID_T:
	case FPL_T:
	case INL_T:
		syn_printe();
		gen_incode("PLATY: Primary s_relational expression parsed");
		break;

	default:
		syn_printe();
		break;
	}
}

/*
* Purpose: <primary s_relational expression’> -> == <primary s_relational expression>
*										       | <> <primary s_relational expression>
*											   | > <primary s_relational expression>
*										       | < <primary s_relational expression>
* FIRST(<primary s_relational expression’>) = { ==, <>, >, < }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: primary_s_relational_expression(), match(), syn_printe()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the primary_s_relational_expression_p FIRST Set
*/
void primary_s_relational_expression_p(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case REL_OP_T:
		switch (lookahead.attribute.rel_op) {
		case EQ:
		case NE:
		case GT:
		case LT:
			match(REL_OP_T, lookahead.attribute.rel_op);
			primary_s_relational_expression();
			break;

		default:
			syn_printe();
			break;
		}

	default:
		break;
	} 
}

/*
* Purpose: <iteration statement> -> WHILE <pre-condition> (<conditional expression>)
*          REPEAT {<statements>};
* FIRST(<iteration statement>) = { KW_T(WHILE) }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: pre_condition(), conditional_expression(), match(), statements(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the iteration_statement FIRST Set
*/
void iteration_statement(void) {
	match(KW_T, WHILE);
	match(KW_T, TRUE);
	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);

	match(KW_T, REPEAT);
	match(LBR_T, NO_ATTR);
	statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Iteration statement parsed");
}

/*
* Purpose: <input statement> -> READ (<variable list>);
* FIRST(<input statement>) = { KW_T(READ) }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: match(), variable_list(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the input_statement FIRST Set
*/
void input_statement(void) {
	match(KW_T, READ);
	match(LPR_T, NO_ATTR);
	variable_list();
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Input statement parsed");
}

/*
* Purpose: <variable list> -> <variable identifier> <variable list’>
* FIRST(<variable list>) = { AVID_T, SVID_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: variable_identifier(), variable_list_p(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the variable_list FIRST Set
*/
void variable_list(void) {
	variable_identifier();
	variable_list_p();
	gen_incode("PLATY: Variable list parsed");
}

/*
* Purpose: <variable identifier> -> AVID_T | SVID_T
* FIRST(<variable identifier>) = { AVID_T, SVID_T }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: match(), syn_printe()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the variable_identifier FIRST Set
*/
void variable_identifier(void) {
	/* look for the tokens */
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		match(lookahead.code, NO_ATTR);
		break;

	default:
		syn_printe();
		break;
	}
}

/*
* Purpose: <variable list’> -> , <variable identifier><variable list’> | e
* FIRST(<variable list’>) = { COM_T, AVID_T, SVID_T, e }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: variable_identifier(), variable_list_p(), match()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the variable_list_p FIRST Set
*/
void variable_list_p(void) {
	/* look for the comma token */
	if (lookahead.code == COM_T) {
		match(COM_T, NO_ATTR);
		variable_identifier();
		variable_list_p();
	}
}

/*
* Purpose: <output statement> -> WRITE (<output list>);
* FIRST(<output statement>) = { KW_T(WRITE) }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: variable_identifier(), match()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the output_statement FIRST Set
*/
void output_statement(void) {
	match(KW_T, WRITE);
	match(LPR_T, NO_ATTR);
	output_list();
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Output statement parsed");
}

/*
* Purpose: <output list> -> <variable list> | STR_T | e
* FIRST(<output list>) = { AVID_T, SVID_T, STR_T, e }
*
* Author: Wenjing Wang
* Versions: 1.17.2
* Called Functions: variable_list(), match(), gen_incode()
* Parameters: N/A
* Return value: N/A
* Algorithm: go through the output_list FIRST Set
*/
void output_list(void) {
	/* look for tokens */
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		variable_list();
		break;

	case STR_T:
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Output list (string literal) parsed");
		break;

	default:
		gen_incode("PLATY: Output list (empty) parsed");
		break;
	}
}
