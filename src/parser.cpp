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
        throw RuntimeError("RE for empty list");
    }
    SyntaxBase* first_ = stxs[0].get();  // 先处理第一个

    // first_ 是指向 Identifier类型的对象 比如保留字
    if (dynamic_cast<Identifier*>(first_)) {
        auto id = dynamic_cast<Identifier*>(first_);
        string id_name = id->s;
        Value find_name_in_Env = find(id_name, env);
        if (find_name_in_Env.get() != nullptr) {
            // 说明被重定义了 两种情况 其一是绑定为 reserved_words 其二是绑定为普通函数
            vector<Expr> rand_;  // 参数列表
            for (int i = 1; i < stxs.size(); i++) {
                rand_.push_back(stxs[i].get()->parse(env));
            }
            return new Apply(first_->parse(env), rand_);
        }
        // 没有被绑定
        if (reserved_words.count(id_name)) {
            switch (reserved_words[id_name]) {
                case E_LET: {
                    checkArgCount(3, stxs.size());
                    std::vector<std::pair<std::string, Expr>> bind_in;
                    Assoc New_env = env;
                    if (auto list_it = dynamic_cast<List*>(stxs[1].get())) {
                        // 读取绑定条件
                        for (int i = 0; i < list_it->stxs.size(); i++) {
                            // 判断是否为 Var expr 对
                            if (auto pair_it = dynamic_cast<List*>(list_it->stxs[i].get())) {
                                checkArgCount(2, pair_it->stxs.size());
                                if (auto Ident_it = dynamic_cast<Identifier*>(pair_it->stxs.front().get())) {
                                    Expr temp_expr = pair_it->stxs.back().get()->parse(env);
                                    New_env = extend(Ident_it->s, NullV(), New_env);
                                    pair<string, Expr> tmp_pair = mp(Ident_it->s, temp_expr);
                                    bind_in.push_back(tmp_pair);
                                }
                            }
                        }
                    }
                    return new Let(bind_in, stxs[2].parse(New_env));
                    break;
                }
                case E_LAMBDA: {
                    checkArgCount(3, stxs.size());
                    Assoc New_env = env;
                    std::vector<std::string> vars;
                    if (auto it = dynamic_cast<List*>(stxs[1].get())) {
                        // 读取列表中变量
                        for (int i = 0; i < it->stxs.size(); i++) {
                            // Expr tmp_ = it->stxs[i].get()->parse(env);
                            if (auto tmp_var = dynamic_cast<Identifier*>(it->stxs[i].get())) {
                                vars.push_back(tmp_var->s);
                                New_env = extend(tmp_var->s, NullV(), New_env);
                            } else {
                                throw RuntimeError("Wrong with your var");
                            }
                        }
                        return Expr(new Lambda(vars, stxs[2].get()->parse(New_env)));
                    } else {
                        throw RuntimeError("Wrong with your var");
                    }
                    break;
                }
                case E_LETREC: {
                    checkArgCount(3, stxs.size());
                    std::vector<std::pair<std::string, Expr>> bind_in;
                    Assoc env1 = env;
                    Assoc env2 = env;

                    // 处理绑定条件（绑定变量并初始化为空）
                    if (auto list_it = dynamic_cast<List*>(stxs[1].get())) {
                        // 第一次遍历：声明变量，绑定 nullptr 到 env1
                        for (int i = 0; i < list_it->stxs.size(); i++) {
                            if (auto pair_it = dynamic_cast<List*>(list_it->stxs[i].get())) {
                                checkArgCount(2, pair_it->stxs.size());
                                if (auto Ident_it = dynamic_cast<Identifier*>(pair_it->stxs.front().get())) {
                                    env1 = extend(Ident_it->s, NullV(), env1);  // 将变量绑定到 NullV()
                                } else {
                                    throw RuntimeError("Not Var Expr* parm");
                                }
                            } else {
                                throw RuntimeError("Not List parm");
                            }
                        }

                        // 第二次遍历：计算值并绑定到 env2
                        env2 = env1;
                        for (int i = 0; i < list_it->stxs.size(); i++) {
                            if (auto pair_it = dynamic_cast<List*>(list_it->stxs[i].get())) {
                                if (auto Ident_it = dynamic_cast<Identifier*>(pair_it->stxs.front().get())) {
                                    // 计算绑定表达式的值
                                    Expr tem_expr = pair_it->stxs.back().get()->parse(env1);
                                    // 将绑定的值更新到 env2
                                    env2 = extend(Ident_it->s, tem_expr->eval(env2), env2);
                                    bind_in.push_back(mp(Ident_it->s, tem_expr));
                                }
                            }
                        }
                    } else {
                        throw RuntimeError("Not List parm");
                    }
                    return new Letrec(bind_in, stxs[2].get()->parse(env2));  // 返回 Letrec
                    break;
                }
                case E_IF: {
                    checkArgCount(4, stxs.size());
                    return new If(stxs[1].get()->parse(env), stxs[2].get()->parse(env), stxs[3].get()->parse(env));
                    break;
                }
                case E_BEGIN: {
                    vector<Expr> rand_;  // 参数列表
                    for (int i = 1; i < stxs.size(); i++) {
                        rand_.push_back(stxs[i].get()->parse(env));
                    }
                    return new Begin(rand_);
                    break;
                }
                case E_QUOTE: {
                    checkArgCount(2, stxs.size());
                    return new Quote(stxs[1]);
                    break;
                }
            }
        }
        // 检查是否是保留字
        vector<Expr> rand_;  // 参数列表
        for (int i = 1; i < stxs.size(); i++) {
            rand_.push_back(stxs[i].get()->parse(env));
        }
        return new Apply(first_->parse(env), rand_);
    }
    // first_ 是指向 List 类型的对象
    if (auto first_it = dynamic_cast<List*>(first_)) {
        vector<Expr> rand_;  // 参数列表
        for (int i = 1; i < stxs.size(); i++) {
            rand_.push_back(stxs[i].get()->parse(env));
        }
        return new Apply(first_it->parse(env), rand_);  // 传入引用
    }
    throw RuntimeError("RE : nothing to do");
}
#endif