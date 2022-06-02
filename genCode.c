#include "genCode.h"
#include "minic_yacc.h"

int ii = 0, itop = -1, istack[100];     // if语句使用的栈
#define _BEG_IF     {istack[++itop] = ++ii;}
#define _END_IF     {itop--;}
#define _i          (istack[itop])

int ww = 0, wtop = -1, wstack[100];     // while语句使用的栈
#define _BEG_WHILE  {wstack[++wtop] = ++ww;}
#define _END_WHILE  {wtop--;}
#define _w          (wstack[wtop])

/**
 * @brief 给定AST，生成中间代码，返回值为结果所在寄存器号
 * 
 * @param T 输入的AST
 */
void code_generator(struct node *T)
{
    if(T){
        switch(T->kind) {
            case DEF:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                break;
            case L_V_DEF:
            case G_V_DEF:
                code_generator(T->ptr[1]);
                break;
            case L_V_EXPR:
            case G_V_EXPR:
                fprintf(fp_ir, "\tvar %s\n", T->ptr[0]->type_id);
                break;
            case L_ASSIGNOP:
            case G_ASSIGNOP:
                fprintf(fp_ir, "\tvar %s\n", T->ptr[0]->type_id);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tpop %s\n", T->ptr[0]->type_id);
                break;

            case F_DEF:
                code_generator(T->ptr[1]);
                code_generator(T->ptr[2]);
                fprintf(fp_ir, "\nENDFUNC\n");
                break;
            case F_DEC:
                fprintf(fp_ir, "FUNC @%s\n", T->ptr[0]->type_id);
                code_generator(T->ptr[1]);
                break;
            
            case STMTS:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                break;
            case STMT:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                break;
            case EXPR:
                code_generator(T->ptr[0]);
                break;
            case RETURN:
                code_generator(T->ptr[0]);
                fprintf(fp_ir, "\tret\n");
                break;
            case IF_THEN:
                
            case IF_THEN_ELSE:
                _BEG_IF; 
                fprintf(fp_ir, "_begIf_%d:\n", _i);
                code_generator(T->ptr[0]);
                fprintf(fp_ir, "\tjz _elIf_%d\n", _i);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tjmp _endIf_%d\n_elIf_%d:\n", _i, _i);
                code_generator(T->ptr[2]);
                fprintf(fp_ir, "_endIf_%d:\n\n", _i);
                _END_IF;
                break; 
            case WHILE:
                _BEG_WHILE; 
                fprintf(fp_ir, "_begWhile_%d:\n", _w);
                code_generator(T->ptr[0]);
                fprintf(fp_ir, "\tjz _endWhile_%d\n", _w);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tjmp _begWhile_%d\n_endWhile_%d:\n\n", _w, _w); 
                _END_WHILE;
                break;
            case BREAK:
                fprintf(fp_ir, "\tjmp _endWhile_%d\n", _w);
                break;
            case CONTINUE:
                fprintf(fp_ir, "\tjmp _begWhile_%d\n", _w);
                break;
            case ASSIGNOP:
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tpop %s\n\n", T->ptr[0]->type_id);
                break;
            case FUNC_CALL:
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tCALL ");
                code_generator(T->ptr[0]);
                break;
            case F_C_NAME:
                fprintf(fp_ir, "%s\n", T->type_id);
                break;
            case PARAMS:
            case PARAM:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                break;
            case AND:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tand\n");
                break;
            case OR:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tor\n");
                break;
            case GT:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tcmpa\n");
                break;
            case LT:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tcmpb\n");
                break;
            case GE:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tcmpae\n");
                break;
            case LE:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tcmpbe\n");
                break;
            case EQUAL:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tcmpe\n");
                break;
            case NE:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tcmpne\n");
                break;
            case MOD:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tmod\n");
                break;
            case PLUS:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tadd\n");
                break;
            case MINUS:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tsub\n");
                break;
            case MUL:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tmul\n");
                break;
            case DIV:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                fprintf(fp_ir, "\tdiv\n");
                break;
            
            case INT:
                fprintf(fp_ir, "\tpush %d\n", T->type_int);
                break;
            case ID:
                fprintf(fp_ir, "\tpush %s\n", T->type_id);
                break;

            case KEYINT:
                fprintf(fp_ir, "\targs ");  // 大雷
                break;
            case P_NAME:
                fprintf(fp_ir, "%s\n\n", T->type_id);
                break;
            
            case V_PARAMS:
            case V_PARAM:
                code_generator(T->ptr[0]);
                code_generator(T->ptr[1]);
                break;
            case V_P_NAME:
                fprintf(fp_ir, "\tpush %s\n", T->type_id);
                break;
        }
    }
}

/**
 * @brief 生成中间代码
 * 
 */
void generate_code(struct node *T) 
{
    int reg;
    code_generator(T);	//生成汇编代码
}

