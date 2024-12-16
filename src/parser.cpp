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

Expr Syntax ::parse(Assoc& env) {
    return ptr.get()->parse(env);  // Syntax 作为封装类的parse是直接取其实际包裹指针的函数
}

Expr Number ::parse(Assoc& env) {
    return new Fixnum(this->n);
}

Expr Identifier ::parse(Assoc& env) {
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

    // first_ 是指向 Identifier类型的对象 比如保留字
    if (auto id = dynamic_cast<Identifier*>(first_)) {
        string id_name = id->s;
        if (reserved_words.count(id_name)) {
            switch (reserved_words[id_name]) {
                case E_LET: {
                    checkArgCount(3, stxs.size());
                    std::vector<std::pair<std::string, Expr>> bind_in;
                    if (auto list_it = dynamic_cast<List*>(stxs[1].get())) {
                        // 读取绑定条件
                        for (int i = 0; i < list_it->stxs.size(); i++) {
                            // 判断是否为 Var expr 对
                            if (auto pair_it = dynamic_cast<List*>(list_it->stxs[i].get())) {
                                checkArgCount(2, pair_it->stxs.size());
                                if (auto Ident_it = dynamic_cast<Identifier*>(pair_it->stxs.front().get())) {
                                    pair<string, Expr> tmp_pair = mp(Ident_it->s, pair_it->stxs.back().get()->parse(env));
                                    bind_in.push_back(tmp_pair);
                                }
                            }
                        }
                    }
                    return new Let(bind_in, stxs[2].get()->parse(env));
                }
                case E_LAMBDA: {
                    checkArgCount(3, stxs.size());
                    std::vector<std::string> vars;
                    if (auto it = dynamic_cast<List*>(stxs[1].get())) {
                        // 读取列表中变量
                        for (int i = 0; i < it->stxs.size(); i++) {
                            if (auto tmp_var = dynamic_cast<Identifier*>(it->stxs[i].get())) {
                                vars.push_back(tmp_var->s);
                            } else {
                                throw RuntimeError("Wrong with your var");
                            }
                        }
                        return new Lambda(vars, stxs[2].get()->parse(env));
                    } else {
                        throw RuntimeError("Wrong with your var");
                    }
                }
                case E_LETREC: {
                    checkArgCount(3, stxs.size());
                    std::vector<std::pair<std::string, Expr>> bind_in;
                    Assoc env1 = env;
                    Assoc env2 = env1;
                    if (auto list_it = dynamic_cast<List*>(stxs[1].get())) {
                        // 读取绑定条件
                        // 第一次遍历：声明变量，绑定 nullptr 到env1
                        for (int i = 0; i < list_it->stxs.size(); i++) {
                            // 判断是否为 Var expr 对
                            if (auto pair_it = dynamic_cast<List*>(list_it->stxs[i].get())) {
                                checkArgCount(2, pair_it->stxs.size());
                                if (auto Ident_it = dynamic_cast<Identifier*>(pair_it->stxs.front().get())) {
                                    env1 = extend(Ident_it->s, Value(nullptr), env1);
                                } else {
                                    throw RuntimeError("Not Var Expr* parm");
                                }
                            } else {
                                throw RuntimeError("Not List parm");
                            }
                        }
                        // 第二次遍历：计算值并更新 env2
                        env2 = env1;
                        for (int i = 0; i < list_it->stxs.size(); i++) {
                            if (auto pair_it = dynamic_cast<List*>(list_it->stxs[i].get())) {
                                if (auto Ident_it = dynamic_cast<Identifier*>(pair_it->stxs.front().get())) {
                                    pair<string, Expr> tmp_pair = mp(Ident_it->s, pair_it->stxs.back().get()->parse(env1));
                                    env2 = extend(tmp_pair.first, tmp_pair.second.get()->eval(env1), env2);
                                    bind_in.push_back(tmp_pair);
                                } else {
                                    throw RuntimeError("Not Var Expr* parm");
                                }
                            } else {
                                throw RuntimeError("Not List parm");
                            }
                        }
                    }
                    return new Letrec(bind_in, stxs[2].get()->parse(env2));
                }
                case E_IF: {
                    checkArgCount(4, stxs.size());
                    return new If(stxs[1].get()->parse(env), stxs[2].get()->parse(env), stxs[3].get()->parse(env));
                }
                case E_BEGIN: {
                    vector<Expr> rand_;  // 参数列表
                    for (int i = 1; i < stxs.size(); i++) {
                        rand_.push_back(stxs[i].get()->parse(env));
                    }
                    return new Begin(rand_);
                }
                case E_QUOTE: {
                    checkArgCount(2, stxs.size());
                    return new Quote(stxs[1]);
                }
            }
        }
        // 检查是否是保留字
        if (primitives.count(id_name)) {
            vector<Expr> rand_;  // 参数列表
            for (int i = 1; i < stxs.size(); i++) {
                rand_.push_back(stxs[i].get()->parse(env));
            }
            return new Apply(first_->parse(env), rand_);
            switch (primitives[id_name]) {
                case E_MUL: {
                    // *
                    checkArgCount(3, stxs.size());
                    return new Mult(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
                    break;
                }
                case E_MINUS: {
                    // -
                    checkArgCount(3, stxs.size());
                    return new Minus(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
                    break;
                }
                case E_PLUS: {
                    // +
                    checkArgCount(3, stxs.size());
                    return new Plus(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
                    break;
                }
                case E_LT: {
                    // <
                    checkArgCount(3, stxs.size());
                    return new Less(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
                    break;
                }
                case E_LE: {
                    // <=
                    checkArgCount(3, stxs.size());
                    return new LessEq(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
                    break;
                }
                case E_EQ: {
                    // ==
                    checkArgCount(3, stxs.size());
                    return new Equal(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
                    break;
                }
                case E_GE: {
                    // >=
                    checkArgCount(3, stxs.size());
                    return new GreaterEq(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
                    break;
                }
                case E_GT: {
                    // >
                    checkArgCount(3, stxs.size());
                    return new Greater(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
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
                    return new IsEq(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
                    break;
                }
                case E_BOOLQ: {
                    // "boolen?"
                    checkArgCount(2, stxs.size());
                    return new IsBoolean(stxs[1].get()->parse(env));
                    break;
                }
                case E_INTQ: {
                    // "fixnum?"
                    checkArgCount(2, stxs.size());
                    return new IsFixnum(stxs[1].get()->parse(env));
                    break;
                }
                case E_NULLQ: {
                    // "null?"
                    checkArgCount(2, stxs.size());
                    return new IsNull(stxs[1].get()->parse(env));
                    break;
                }
                case E_PAIRQ: {
                    // "pair?"
                    checkArgCount(2, stxs.size());
                    return new IsPair(stxs[1].get()->parse(env));
                    break;
                }
                case E_PROCQ: {
                    // "procedure?"
                    checkArgCount(2, stxs.size());
                    return new IsProcedure(stxs[1].get()->parse(env));
                    break;
                }
                case E_SYMBOLQ: {
                    // "symbol?"
                    checkArgCount(2, stxs.size());
                    return new IsSymbol(stxs[1].get()->parse(env));
                    break;
                }
                case E_CONS: {
                    // cons
                    checkArgCount(3, stxs.size());
                    return new Cons(stxs[1].get()->parse(env), stxs[2].get()->parse(env));
                    break;
                }
                case E_NOT: {
                    // not
                    checkArgCount(2, stxs.size());
                    return new Not(stxs[1].get()->parse(env));
                    break;
                }
                case E_CAR: {
                    // cdr
                    checkArgCount(2, stxs.size());
                    return new Car(stxs[1].get()->parse(env));
                    break;
                }
                case E_CDR: {
                    checkArgCount(2, stxs.size());
                    return new Cdr(stxs[1].get()->parse(env));
                    break;
                }
                case E_EXIT: {
                    checkArgCount(1, stxs.size());
                    return new Exit();
                }
            }
        }
        if (auto now_it = find(id_name, env).get()) {
            vector<Expr> rand_;  // 参数列表
            for (int i = 1; i < stxs.size(); i++) {
                rand_.push_back(stxs[i].get()->parse(env));
            }
            return new Apply(id->parse(env), rand_);
        } else {
            throw RuntimeError("RE");
        }
    }
    // first_ 是指向 List 类型的对象
    if (auto first_it = dynamic_cast<List*>(first_)) {
        vector<Expr> rand_;  // 参数列表
        for (int i = 1; i < stxs.size(); i++) {
            rand_.push_back(stxs[i].get()->parse(env));
        }
        return new Apply(first_it->parse(env), rand_);  // 传入引用
    }
}
#endif

//((if #t (lambda () +) (lambda () -)) 2 3)