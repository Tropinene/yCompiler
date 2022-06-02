#ifndef __AST_H__
#define __AST_H__

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "minic_yacc.h"

//节点类型
enum node_kind  { 
    DEF,
    EXPR,
    F_DEF,
    F_DEC,
    F_NAME,
    FUNC_CALL,
    F_C_NAME,
    G_V_DEF,
    G_V_EXPR,
    G_ID,
    G_ASSIGNOP,
    IF_THEN,
    L_V_DEF,
    L_V_EXPR,
    L_ASSIGNOP,
    L_ID,
    PARAMS,
    PARAM,
    P_NAME,
    RELOP,
    STMTS,
    STMT,
    V_P_NAME,
    V_PARAMS,
    V_PARAM
};

//抽象语法树节点
struct node 
{
	enum node_kind kind;                //结点类型
	union 
    {
        char    type_id[33];            //由标识符生成的叶结点
	    int     type_int;               //由整常数生成的叶结点
	    float   type_float;             //由浮点常数生成的叶结点
        char    type_char;              //由字符常量生成的叶节点
        // int     offset_id;              //对于标识符记录存储的符号表位置
	};
    struct node *ptr[3];                //子树指针
    int  type;                          //结点对应值的类型,INT,CHAR,FLOAT
    int pos;                            //语法单位所在位置行号,用于报错
};

struct node *makeNode(int kind,struct node *first,struct node *second, struct node *third,int pos );//生成抽象语法树节点
void createAST(struct node *T, int level);//生成.gv文件，用于可视化抽象语法树
void startSemanticAnalysis(struct node *T);//开始语义分析
void prtTable(int level);

//全局变量
char OPTION;//minic操作
char *fl_name;
FILE *fp;//用于储存抽象语法树生成的文件


#endif