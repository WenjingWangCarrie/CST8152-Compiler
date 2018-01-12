/* 
* Filename: parser.h
* Version: 1.17.2
* Compiler: MS Visual Studio 2015
* Author: Wenjing Wang, ID# 040812907
* Course: CST8152 - Compiler, Lab Section: 013
* Assignment: 3
* Date: December 30, 2017
* Professor: Svillen Ranev
* Purpose: Parser declarations necessary for the parser implementation
* Function list: parser(Buffer*), match(int, int ), syn_eh(int), syn_printe(), gen_incode(char*), program(void), opt_statements(), statements(),
* statement(), statements_p(), assignment_statement(), assignment_expression(), conditional_expression(), logical_OR_expression(), 
* logical_OR_expression_p(), logical_AND_expression(), logical_AND_expression_p(), relational_expression(), primary_a_relational_expression(),
* primary_a_relational_expression_p(), primary_s_relational_expression(), primary_s_relational_expression_p(), arithmetic_expression(),
* unary_arithmetic_expression(), additive_arithmetic_expression(), primary_aithmetic_expression(), additive_arithmetic_expression_p(), 
* multiplicative_arithmetic_expression(), multiplicative_arithmetic_expression_p(), string_expression(), primary_string_expression(), string_expression_p(), 
* selection_statement(), variable_list(), variable_list_p(), variable_identifier(), iteration_statement(), input_statement(), output_statement(), output_list()
*/

#ifndef PARSER_H_
#define PARSER_H_

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef TOKEN_H_
#include "token.h"
#endif

/* keyword constant */
#define NO_ATTR -1
#define ELSE 0	
#define FALSE 1 
#define IF 2
#define PLATYPUS 3
#define READ 4
#define REPEAT 5
#define THEN 6
#define TRUE 7
#define WHILE 8
#define WRITE 9

/* operator constant */
#define AND 0
#define OR 1

#define EQ 0
#define NE 1
#define GT 2
#define LT 3

#define PLUS 0
#define MINUS 1 
#define MULT 2 
#define DIV 3

#define SCC_OP_T 7

/* Golbal variables in the parser */
static Token lookahead; /* to look for the next token */
static Buffer* sc_buf; /* scanner buffer */
int synerrno; /* run time error counter */ 
extern char * kw_table[]; /* Keyword lookup table */
extern Buffer * str_LTBL; /* String literal table */
extern int line; /* current line number of the source code */
extern Token malar_next_token(Buffer*);	/* scanner function to get next token */

/* main parser functions */ 
void parser(Buffer*);
void match(int, int);
void syn_eh(int);
void syn_printe();
void gen_incode(char*);

/* grammar functions */
void program(void);
void opt_statements();
void statements(void);
void statement(void);
void statements_p(void);
void assignment_statement(void);
void assignment_expression(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
void additive_arithmetic_expression_p(void);
void multiplicative_arithmetic_expression(void);
void multiplicative_arithmetic_expression_p(void);
void primary_arithmetic_expression(void);
void string_expression(void);
void string_expression_p(void);
void primary_string_expression(void);
void selection_statement(void);
void conditional_expression(void);
void logical_or_expression(void);
void logical_or_expression_p(void);
void logical_and_expression(void);
void logical_and_expression_p(void);
void relational_expression(void);
void primary_a_relational_expression(void);
void primary_a_relational_expression_p(void);
void primary_s_relational_expression(void);
void primary_s_relational_expression_p(void);
void iteration_statement(void);
void input_statement(void);
void variable_list(void);
void variable_list_p(void);
void variable_identifier(void);
void output_statement(void);
void output_list(void);

#endif



