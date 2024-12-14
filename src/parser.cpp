#ifndef PARSER
#define PARSER

// parser of myscheme

#include <cstring>
#include <iostream>
#include <map>
#include "Def.hpp"
#include "RE.hpp"
#include "expr.hpp"
#include "syntax.hpp"
#include "value.hpp"
#define mp make_pair
using std ::pair;
using std ::string;
using std ::vector;

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

void checkArgCount(int expected, int actual) {
    if (expected != actual) {
        throw RuntimeError("Expected " + std::to_string(expected - 1) + " arguments, but got " + std::to_string(actual - 1));
    }
}  // 一个定义的 检查参数数量是否达标

Expr Syntax ::parse(Assoc& env) {
    return ptr.get()->parse(env);  // Syntax 作为封装类的parse是直接取其实际包裹指针的函数
}

Expr Number ::parse(Assoc& env) {
    return new Fixnum(this->n);
}

Expr Identifier ::parse(Assoc& env) {
    // 检查标识符是否为原始操作符
    if (primitives.count(this->s)) {
        // throw RuntimeError("Unknown operation: " + this->s);
        return new Var(s);
    }
    return new Var(s);
}

Expr TrueSyntax ::parse(Assoc& env) {
    return new True();
}

Expr FalseSyntax ::parse(Assoc& env) {
    return new False();
}

Expr List ::parse(Assoc& env) {
    if (stxs.empty()) {
        List* it = new List;
        return new Quote(it);
    }
    SyntaxBase* first_ = stxs[0].get();  // 先处理第一个

    // first_ 是指向 Identifier类型的对象
    if (auto id = dynamic_cast<Identifier*>(first_)) {
        string id_name = id->s;
        // 检查是否是保留字
        if (primitives.count(id_name)) {
            vector<Expr> rand_;  // 参数列表
            for (int i = 1; i < stxs.size(); i++) {
                rand_.push_back(stxs[i].get()->parse(env));
            }
            switch (primitives[id_name]) {
                case E_MUL: {
                    // *
                    checkArgCount(3, stxs.size());
                    return new Apply(new Mult(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_MINUS: {
                    // -
                    checkArgCount(3, stxs.size());
                    return new Apply(new Minus(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_PLUS: {
                    // +
                    checkArgCount(3, stxs.size());
                    return new Apply(new Plus(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_LT: {
                    // <
                    checkArgCount(3, stxs.size());
                    return new Apply(new Less(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_LE: {
                    // <=
                    checkArgCount(3, stxs.size());
                    return new Apply(new LessEq(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_EQ: {
                    // ==
                    checkArgCount(3, stxs.size());
                    return new Apply(new Equal(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_GE: {
                    // >=
                    checkArgCount(3, stxs.size());
                    return new Apply(new GreaterEq(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_GT: {
                    // >
                    checkArgCount(3, stxs.size());
                    return new Apply(new Greater(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_VOID: {
                    // void
                    checkArgCount(1, stxs.size());
                    return new MakeVoid();
                }
                case E_EQQ: {
                    // "eq?"
                    checkArgCount(3, stxs.size());
                    return new Apply(new IsEq(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_BOOLQ: {
                    // "boolen?"
                    checkArgCount(2, stxs.size());
                    return new Apply(new IsBoolean(stxs[1].get()->parse(env)), rand_);
                    break;
                }
                case E_INTQ: {
                    // "fixnum?"
                    checkArgCount(2, stxs.size());
                    return new Apply(new IsFixnum(stxs[1].get()->parse(env)), rand_);
                    break;
                }
                case E_NULLQ: {
                    // "null?"
                    checkArgCount(2, stxs.size());
                    return new Apply(new IsNull(stxs[1].get()->parse(env)), rand_);
                    break;
                }
                case E_PAIRQ: {
                    // "pair?"
                    checkArgCount(2, stxs.size());
                    return new Apply(new IsPair(stxs[1].get()->parse(env)), rand_);
                    break;
                }
                case E_PROCQ: {
                    // "procedure?"
                    checkArgCount(2, stxs.size());
                    return new Apply(new IsProcedure(stxs[1].get()->parse(env)), rand_);
                    break;
                }
                case E_SYMBOLQ: {
                    // "symbol?"
                    checkArgCount(2, stxs.size());
                    return new Apply(new IsSymbol(stxs[1].get()->parse(env)), rand_);
                    break;
                }
                case E_CONS: {
                    // cons
                    checkArgCount(3, stxs.size());
                    return new Apply(new Cons(stxs[1].get()->parse(env), stxs[2].get()->parse(env)), rand_);
                    break;
                }
                case E_NOT: {
                    // not
                    checkArgCount(2, stxs.size());
                    return new Apply(new Not(stxs[1].get()->parse(env)), rand_);
                    break;
                }
                case E_CAR: {
                    // cdr
                    checkArgCount(2, stxs.size());
                    return new Apply(new Car(stxs[1].get()->parse(env)), rand_);
                    break;
                }
                case E_CDR: {
                    checkArgCount(2, stxs.size());
                    return new Apply(new Cdr(stxs[1].get()->parse(env)), rand_);
                    break;
                }
                case E_EXIT: {
                    checkArgCount(1, stxs.size());
                    return new Exit();
                }
            }
        } else if (reserved_words.count(id_name)) {
            switch (reserved_words[id_name]) {
                case E_LET: {
                }
                case E_LAMBDA: {
                }
                case E_LETREC: {
                }
                case E_IF: {
                }
                case E_BEGIN: {
                }
                case E_QUOTE: {
                    checkArgCount(2, stxs.size());
                    return new Quote(stxs[1]);
                }
            }
        }
    }
    // first_ 是指向 List 类型的对象
    if (dynamic_cast<List*>(first_) != nullptr) {
    }
}

#endif