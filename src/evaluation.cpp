#include <cstring>
#include <map>
#include <vector>
#include "Def.hpp"
#include "RE.hpp"
#include "expr.hpp"
#include "syntax.hpp"
#include "value.hpp"

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

void CheckVar(std::string x) {
    if ((x.front() >= '0' && x.front() <= '9') || (x.front() == '.') || (x.front() == '@')) {
        throw RuntimeError("Wrong with your var");
    }
    return;
}

Value Let::eval(Assoc& env) {}  // let expression

Value Lambda::eval(Assoc& env) {
    Assoc capturedEnv = env;  // 拷贝当前环境 深拷贝
    return ClosureV(x, e, capturedEnv);
}  // lambda expression

Value Apply::eval(Assoc& e) {
    Value func = rator.get()->eval(e);
    auto closure_ = dynamic_cast<Closure*>(func.get());
    if (!closure_) {
        throw RuntimeError("RuntimeError: Not a procedure");
    }

    // 2.对 rand 中的每个实参数求值
    std::vector<Value> evaluatedArgs;
    for (const auto& operand : rand) {
        evaluatedArgs.push_back(operand->eval(e));  // 依次对每个参数求值
    }

    // 3.检查参数数量 实参形参是否匹配
    if (evaluatedArgs.size() != closure_->parameters.size()) {
        // throw RuntimeError("RuntimeError: Argument count mismatch");
    }

    // 4.创建新作用域，将形参与实参绑定
    Assoc newEnv = closure_->env;  // 使用闭包中的环境
    for (size_t i = 0; i < closure_->parameters.size(); ++i) {
        // 将形参绑定成实参 参数遮蔽
        newEnv = extend(closure_->parameters[i], evaluatedArgs[i], newEnv);
    }
    return closure_->e.get()->eval(newEnv);  // 在新环境中求值
}  // for function calling

Value Letrec::eval(Assoc& env) {}  // letrec expression

Value Var::eval(Assoc& e) {
    // 先检查是否合法
    CheckVar(x);
    auto it = find(x, e);
    if (it.get() == nullptr) {
        if (primitives.count(x)) {
            std::vector<std::string> parameters_;  // 形参
            Expr exp;
            int type_name = -1;
            if (primitives.count(x)) {
                type_name = primitives[x];
            } else {
                type_name = reserved_words[x];
            }
            // 转为函数
            switch (type_name) {
                case E_MUL: {
                    // *
                    exp = (new Mult(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_MINUS: {
                    // -
                    exp = (new Minus(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_PLUS: {
                    // +
                    exp = (new Plus(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_LT: {
                    // <
                    exp = (new Less(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_LE: {
                    // <=
                    exp = (new LessEq(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_EQ: {
                    // ==
                    exp = (new Equal(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_GE: {
                    // >=
                    exp = (new GreaterEq(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_GT: {
                    // >
                    exp = (new Greater(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_EQQ: {
                    // "eq?"
                    exp = (new IsEq(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_BOOLQ: {
                    // "boolen?"
                    exp = (new IsBoolean(new Var("parm")));
                    break;
                }
                case E_INTQ: {
                    // "fixnum?"
                    exp = (new IsFixnum(new Var("parm")));
                    break;
                }
                case E_NULLQ: {
                    // "null?"
                    exp = (new IsNull(new Var("parm")));
                    break;
                }
                case E_PAIRQ: {
                    // "pair?"
                    exp = (new IsPair(new Var("parm")));
                    break;
                }
                case E_PROCQ: {
                    // "procedure?"
                    exp = (new IsProcedure(new Var("parm")));
                    break;
                }
                case E_SYMBOLQ: {
                    // "symbol?"
                    exp = (new IsSymbol(new Var("parm")));
                    break;
                }
                case E_CONS: {
                    // cons
                    exp = (new Cons(new Var("parm1"), new Var("parm2")));
                    break;
                }
                case E_NOT: {
                    // not
                    exp = (new Not(new Var("parm")));
                    break;
                }
                case E_CAR: {
                    // cdr
                    exp = (new Car(new Var("parm")));
                    break;
                }
                case E_CDR: {
                    exp = (new Cdr(new Var("parm")));
                    break;
                }
                case E_VOID: {
                    // void
                    exp = (new MakeVoid());
                }
                case E_EXIT: {
                    exp = (new Exit());
                }
                    // case E_LET: {
                    // }
                    // case E_LAMBDA: {
                    //     std::vector<std::string> vars(3);
                    //     vars[0] = "parm1";
                    //     vars[1] = "parm2";
                    //     for (int i = 0; i < vars.size(); i++) {
                    //         parameters_.push_back(vars[i]);
                    //     }
                    //     exp = (new Lambda(vars,))
                    // }
                    // case E_LETREC: {
                    // }
                    // case E_IF: {
                    //     checkArgCount(4, stxs.size());
                    //     return new If(stxs[1].get()->parse(env), stxs[2].get()->parse(env), stxs[3].get()->parse(env));
                    // }
                    // case E_BEGIN: {
                    //     vector<Expr> rand_;  // 参数列表
                    //     for (int i = 1; i < stxs.size(); i++) {
                    //         rand_.push_back(stxs[i].get()->parse(env));
                    //     }
                    //     return new Begin(rand_);
                    // }
                    // case E_QUOTE: {
                    //     checkArgCount(2, stxs.size());
                    //     return new Quote(stxs[1]);
                    // }
            }
            if (dynamic_cast<Binary*>(exp.get())) {
                parameters_.push_back("parm1");
                parameters_.push_back("parm2");
            } else {
                parameters_.push_back("parm");
            }
            return ClosureV(parameters_, exp, e);
        } else {
            throw RuntimeError("RE");
        }
    } else {
        return it;
    }
}  // evaluation of variable

Value Fixnum::eval(Assoc& e) {
    return IntegerV(n);
}  // evaluation of a fixnum

Value If::eval(Assoc& e) {
    if (auto it = dynamic_cast<False*>(cond.get())) {
        auto it_value = alter.get()->eval(e);
        return it_value;
    } else {
        auto it_value = conseq.get()->eval(e);
        return it_value;
    }
}  // if expression

Value True::eval(Assoc& e) {
    return BooleanV(1);
}  // evaluation of #t

Value False::eval(Assoc& e) {
    return BooleanV(0);
}  // evaluation of #f

Value Begin::eval(Assoc& e) {
    for (int it = 0; it < es.size(); it++) {
        auto task_value = es[it].get()->eval(e);
        if (it == es.size() - 1) {
            return task_value;
        }
    }
}  // begin expression

Value Quote::eval(Assoc& e) {
    if (auto it = dynamic_cast<Number*>(s.get())) {
        return IntegerV(it->n);
    } else if (auto it = dynamic_cast<TrueSyntax*>(s.get())) {
        return BooleanV(1);
    } else if (auto it = dynamic_cast<FalseSyntax*>(s.get())) {
        return BooleanV(0);
    } else if (auto it = dynamic_cast<Identifier*>(s.get())) {
        // symbol 为单个独立的变量
        return SymbolV(it->s);
    } else if (auto it = dynamic_cast<List*>(s.get())) {
        if (it->stxs.empty()) {
            // 空列表判断为NUll
            return NullV();
        } else if (it->stxs.size() == 1) {
            if (auto tmp_it = dynamic_cast<Identifier*>(it->stxs.front().get())) {
                // 是表示符 则为了防止是保留字强制转化为 symbol
                return PairV(SymbolV(tmp_it->s), NullV());
            } else
                return PairV(it->stxs.front().parse(e).get()->eval(e), NullV());
        } else {
            // 先检查是不是 (a . b) 格式
            if (it->stxs.size() == 3) {
                if (auto tmp_it = dynamic_cast<Identifier*>(it->stxs[1].get())) {
                    if (tmp_it->s == ".") {
                        Quote car_(it->stxs[0]);
                        Quote cdr_(it->stxs[2]);
                        return PairV(car_.eval(e), cdr_.eval(e));
                    }
                }
            }
            // 先去除之后非0的
            List* remain_list = new List;
            (*remain_list).stxs = std::vector<Syntax>(it->stxs.begin() + 1, it->stxs.end());
            Value restValue = Quote(Syntax(remain_list)).eval(e);
            if (auto tmp_it = dynamic_cast<Identifier*>(it->stxs.front().get())) {
                Value firstValue = SymbolV(tmp_it->s);
                return PairV(firstValue, restValue);
            } else {
                Value firstValue = it->stxs.front().parse(e).get()->eval(e);
                return PairV(firstValue, restValue);
            }  // List 中 存储第一个syntax进行parser之后再进行eval
        }
    } else {
        throw RuntimeError("Unsupported Expr type for quote.");
    }

}  // quote expression

Value MakeVoid::eval(Assoc& e) {
    return VoidV();
}  // (void)

Value Exit::eval(Assoc& e) {
    return TerminateV();
}  // (exit)

Value Binary::eval(Assoc& e) {
    return evalRator(rand1.get()->eval(e), rand2.get()->eval(e));
}  // evaluation of two-operators primitive

Value Unary::eval(Assoc& e) {
    return evalRator(rand.get()->eval(e));
}  // evaluation of single-operator primitive

Value Mult::evalRator(const Value& rand1, const Value& rand2) {
    if (auto it1 = dynamic_cast<Integer*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Integer*>(rand2.get())) {
            return new Integer(it1->n * it2->n);
        }
    }
    throw RuntimeError("Wrong with your parametric");
}  // *

Value Plus::evalRator(const Value& rand1, const Value& rand2) {
    if (auto it1 = dynamic_cast<Integer*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Integer*>(rand2.get())) {
            return new Integer(it1->n + it2->n);
        }
    }
    throw RuntimeError("Wrong with your parametric");
}  // +

Value Minus::evalRator(const Value& rand1, const Value& rand2) {
    if (auto it1 = dynamic_cast<Integer*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Integer*>(rand2.get())) {
            return new Integer(it1->n - it2->n);
        }
    }
    throw RuntimeError("Wrong with your parametric");
}  // -

Value Less::evalRator(const Value& rand1, const Value& rand2) {
    if (auto it1 = dynamic_cast<Integer*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Integer*>(rand2.get())) {
            return ((it1->n < it2->n) ? BooleanV(1) : BooleanV(0));
        }
    }
    throw RuntimeError("Wrong with your parametric");
}  // <

Value LessEq::evalRator(const Value& rand1, const Value& rand2) {
    if (auto it1 = dynamic_cast<Integer*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Integer*>(rand2.get())) {
            return ((it1->n <= it2->n) ? BooleanV(1) : BooleanV(0));
        }
    }
    throw RuntimeError("Wrong with your parametric");
}  // <=

Value Equal::evalRator(const Value& rand1, const Value& rand2) {
    if (auto it1 = dynamic_cast<Integer*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Integer*>(rand2.get())) {
            return ((it1->n == it2->n) ? BooleanV(1) : BooleanV(0));
        }
    }
    throw RuntimeError("Wrong with your parametric");
}  // =

Value GreaterEq::evalRator(const Value& rand1, const Value& rand2) {
    if (auto it1 = dynamic_cast<Integer*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Integer*>(rand2.get())) {
            return ((it1->n >= it2->n) ? BooleanV(1) : BooleanV(0));
        }
    }
    throw RuntimeError("Wrong with your parametric");
}  // >=

Value Greater::evalRator(const Value& rand1, const Value& rand2) {
    if (auto it1 = dynamic_cast<Integer*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Integer*>(rand2.get())) {
            return ((it1->n > it2->n) ? BooleanV(1) : BooleanV(0));
        }
    }
    throw RuntimeError("Wrong with your parametric");
}  // >

Value IsEq::evalRator(const Value& rand1, const Value& rand2) {
    if (auto it1 = dynamic_cast<Integer*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Integer*>(rand2.get())) {
            return ((it1->n == it2->n) ? BooleanV(1) : BooleanV(0));
        }
    } else if (auto it1 = dynamic_cast<Boolean*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Boolean*>(rand2.get())) {
            return ((it1->b == it2->b) ? BooleanV(1) : BooleanV(0));
        }
    } else if (auto it1 = dynamic_cast<Symbol*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Symbol*>(rand2.get())) {
            return ((it1->s == it2->s) ? BooleanV(1) : BooleanV(0));
        }
    } else if (dynamic_cast<Null*>(rand1.get()) || dynamic_cast<Null*>(rand2.get())) {
        if (dynamic_cast<Null*>(rand1.get()) && dynamic_cast<Null*>(rand2.get())) {
            return BooleanV(1);
        } else {
            return BooleanV(0);
        }
    } else if (dynamic_cast<Void*>(rand1.get()) || dynamic_cast<Void*>(rand2.get())) {
        if (dynamic_cast<Void*>(rand1.get()) && dynamic_cast<Void*>(rand2.get())) {
            return BooleanV(1);
        } else {
            return BooleanV(0);
        }
    } else {
        if (rand1.get() == rand2.get()) {
            return BooleanV(1);
        }
        return BooleanV(0);
    }
}  // eq?

Value Cons::evalRator(const Value& rand1, const Value& rand2) {
    return PairV(rand1, rand2);
}  // cons

Value IsBoolean::evalRator(const Value& rand) {
    if (dynamic_cast<Boolean*>(rand.get())) {
        return BooleanV(1);
    } else
        return BooleanV(0);
}  // boolean?

Value IsFixnum::evalRator(const Value& rand) {
    if (dynamic_cast<Integer*>(rand.get())) {
        return BooleanV(1);
    } else
        return BooleanV(0);
}  // fixnum?

Value IsSymbol::evalRator(const Value& rand) {
    if (dynamic_cast<Symbol*>(rand.get())) {
        return BooleanV(1);
    } else
        return BooleanV(0);
}  // symbol?

Value IsNull::evalRator(const Value& rand) {
    if (dynamic_cast<Null*>(rand.get())) {
        return BooleanV(1);
    } else
        return BooleanV(0);
}  // null?

Value IsPair::evalRator(const Value& rand) {
    if (dynamic_cast<Pair*>(rand.get())) {
        return BooleanV(1);
    } else
        return BooleanV(0);
}  // pair?

Value IsProcedure::evalRator(const Value& rand) {
    if (dynamic_cast<Closure*>(rand.get())) {
        return BooleanV(1);
    } else
        return BooleanV(0);
}  // procedure?

Value Not::evalRator(const Value& rand) {
    if (auto it = dynamic_cast<Boolean*>(rand.get())) {
        if (it->b == 0) {
            return BooleanV(1);
        }
    }
    return BooleanV(0);
}  // not

Value Car::evalRator(const Value& rand) {
    if (auto it = dynamic_cast<Pair*>(rand.get())) {
        return it->car;
    } else {
        throw RuntimeError("RE");
    }

}  // car

Value Cdr::evalRator(const Value& rand) {
    if (auto it = dynamic_cast<Pair*>(rand.get())) {
        return it->cdr;
    } else {
        throw RuntimeError("RE");
    }
}  // cdr
