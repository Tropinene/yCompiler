#include "ast.h"
#include "funcstack.h"
#include "genCode.h"
#include "string.h"

/**
 * @brief 生成抽象语法树节点
 * 
 * @param kind 标注节点类型
 * @param first 第一个子节点
 * @param second 第二个子节点
 * @param third 第三个子节点
 * @param pos 所在行数
 * @return struct node* 返回节点 
 */
struct node *makeNode(int kind, struct node *first, struct node *second, struct node *third, int pos)
{
    struct node *T = (struct node *)malloc(sizeof(struct node));
    T->kind = kind;
    T->ptr[0] = first;
    T->ptr[1] = second;
    T->ptr[2] = third;
    T->pos = pos;
    return T;
}

/**
 * @brief 用于绘制节点名（或者说行号更确切一点）
 * 
 * @return char* 返回行号
 */
char * newNode()
{
    static int cnt = 1;
    static char res[10]; 
    memset(res, ' ', 10);
    int i = cnt;
    int idx = 0;
    while (i) {
        idx++;
        i /= 10;
    }
    res[0] = '$';
    res[4] = '\0';

    i = cnt++;
    while(i) {
        res[idx--] = i % 10 + '0';
        i /= 10;
    }
    return res;
}

void prtTable(int level) {
    while(level--) {
        fprintf(fp, "|   ");
    }
}

/**
 * @brief 生成语法树文件
 * 
 * @param T 抽象语法树节点
 * @param level 所在层级
 */
void createAST(struct node *T, int level) {
    struct node *T0;
    if (T) {
        switch (T->kind) {
            case DEF:
                createAST(T->ptr[0], level);
                createAST(T->ptr[1], level);
                break;
            case G_V_DEF:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp, "< label=\"GLOBAL_Definition\" >\n");
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                break;
            case G_V_EXPR:
                createAST(T->ptr[0], level);
                break;
            case G_ID:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp, "< label=\"GLOBAL_Identifier\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                break;
            case G_ASSIGNOP:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Opration\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                break;
            case F_DEF:
                funcName[funcIndex_++] = T;
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp, "< label=\"Function_Definition\" >\n");
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                if(T->ptr[2])
                    createAST(T->ptr[2], level+1);
                break;
            case F_DEC:
                createAST(T->ptr[0], level);
                createAST(T->ptr[1], level);
                break;
            case F_NAME:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp, "< label=\"Function_Name\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                break;
            case PARAMS:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp, "< label=\"Function_Params\" >\n");
                createAST(T->ptr[0], level);
                createAST(T->ptr[1], level);
                break;
            case PARAM:
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                break;
            case P_NAME:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp, "< label=\"Param_Name\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                break;
            case EXPR:
                createAST(T->ptr[0], level);
                break;
            case STMTS:
                createAST(T->ptr[0], level);
                createAST(T->ptr[1], level);
                break;
            case STMT:
                createAST(T->ptr[0], level);
                createAST(T->ptr[1], level);
                break;
            case L_V_DEF:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp, "< label=\"LOCAL_Definition\" >\n");
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                break;
            case L_V_EXPR:
                createAST(T->ptr[0], level);
                break;
            case L_ID:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp, "< label=\"LOCAL_Identifier\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                break;
            case L_ASSIGNOP:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Opration\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                break;
            case IF_THEN:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=ONLY_IF>\n");
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                break;
            case IF_THEN_ELSE:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=IF>\n");
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=ELSE>\n");
                createAST(T->ptr[2], level+1);
                break;
            case WHILE:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=WHILE >\n");
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                break;
            case RETURN:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=RETURN >\n");
                createAST(T->ptr[0], level+1);
                break;
            case FUNC_CALL:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Function_Call\" >\n");
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                break;
            case F_C_NAME:
                for(int i=0; i<funcIndex_; i++) {
                    if(strcmp(T->type_id, funcName[i]->ptr[1]->ptr[0]->type_id)==0){
                        T0 = funcName[i];
                        break;
                    }
                }
                createAST(T0->ptr[0], level+1);
                createAST(T0->ptr[1], level+1);
                createAST(T0->ptr[2], level+1);
                break;
            case BREAK:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=BREAK >\n");
                break;
            case CONTINUE:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=CONTINUE >\n");
                break;
            case KEYINT:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                break;
            case KEYVOID:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                break;    
            case KEYFLOAT:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                break;
            case KEYCHAR:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Keyword\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                break;
            case INT:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"INT\", val=\"%d\", line=%d >\n",T->type_int, T->pos);
                break;
            case FLOAT:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"FLOAT\", val=\"%f\", line=%d >\n",T->type_float, T->pos);
                break;
            case CHAR:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"CHAR\", val=\"%s\", line=%d >\n",T->type_id, T->pos);
                break;
            case ID:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Identifier\", val=\"%s\", line=%d >\n",T->type_id, T->pos);
                break;
            case ASSIGNOP:
            case GT:
            case LT:
            case GE:
            case LE:
            case EQUAL:
            case NE:
            case PLUS:
            case MINUS:
            case DIV:
            case MOD:
            case AND:
            case OR:
            case MUL:
                fprintf(fp, "%s", newNode());
                prtTable(level);
                fprintf(fp,"< label=\"Opration\", val=\"%s\", line=%d >\n", T->type_id, T->pos);
                createAST(T->ptr[0], level+1);
                createAST(T->ptr[1], level+1);
                break;

        }
    }
}

/**
 * @brief 类似于菜单，-a生成语法树，-i生成中间代码
 * 
 * @param T 
 */
void startSemanticAnalysis(struct node *T) {
    //生成抽象语法树.gv文件ast
    if(OPTION == 'a')
    {
        fp = fopen("./tree.gv", "w+");
        if(fp == NULL)
        {
            printf("文件打开失败\n");
            return;
        }
        fprintf(fp,"#### Program start ####\n\n");
        createAST(T, 0);
        fprintf(fp,"\n######### End #########\n");
        fclose(fp);
        printf("The Abstract Syntax Tree is generated successfully!\n");
    }
    else if(OPTION == 'i') {
        fp_ir = fopen("./a.ir", "w+");
        if(fp_ir == NULL)
        {
            printf("文件打开失败\n");
            exit(1);
        }
        generate_code(T);
        printf("The IR is generated successfully!\n");
        fclose(fp_ir);
    }
    else {
        printf("Error: Invalid arguments\n");
        exit(-1);
    }
}