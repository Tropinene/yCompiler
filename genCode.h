#ifndef __GEN_H__
#define __GEN_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"

FILE *fp_ir;

void asm_generator(struct node *T);
void generate_code(struct node *T);
void code_generator(struct node *T);

#endif