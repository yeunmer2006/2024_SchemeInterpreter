#ifndef DEF_HPP
#define DEF_HPP

// By luke36

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

struct Syntax;
struct Expr;
struct Value;
struct AssocList;
struct Assoc;

enum ExprType  // Expr代表的类型
{
    E_LET,     // let
    E_LAMBDA,  // lambda
    E_APPLY,   // apply
    E_LETREC,  // letrec
    E_VAR,     // var
    E_FIXNUM,  //
    E_IF,      //"if"
    E_TRUE,
    E_FALSE,
    E_BEGIN,    //"begin"
    E_QUOTE,    //"quote"
    E_VOID,     //"void"
    E_MUL,      //"*"
    E_PLUS,     //"+"
    E_MINUS,    //"-"
    E_LT,       //"<"
    E_LE,       //"<="
    E_EQ,       //"="
    E_GE,       //">="
    E_GT,       //[">"
    E_CONS,     //"cons"
    E_NOT,      //"not"
    E_CAR,      //"car"
    E_CDR,      //"cdr"
    E_EQQ,      //"eq?"
    E_BOOLQ,    //"boolean?"
    E_INTQ,     //"fixnum?"
    E_NULLQ,    //"null?"
    E_PAIRQ,    //"pair?"
    E_PROCQ,    //"procedure?"
    E_SYMBOLQ,  //"symbol?"
    E_EXIT
};
enum ValueType {
    V_INT,
    V_BOOL,
    V_SYM,
    V_NULL,
    V_STRING,
    V_PAIR,
    V_PROC,
    V_VOID,
    V_PRIMITIVE,
    V_TERMINATE
};

void initPrimitives();
void initReservedWords();

#endif
