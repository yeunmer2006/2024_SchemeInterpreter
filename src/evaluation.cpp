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
#define mp make_pair
using std ::pair;
using std ::string;
using std ::vector;

void CheckVar(std::string name) {
    if (name.empty())
        throw RuntimeError("RE in CheckVar");
    char firstChar = name[0];
    if (std::isdigit(firstChar) || firstChar == '.' || firstChar == '@')
        throw RuntimeError("RE in CheckVar");
    for (int i = 0; i < name.size(); i++) {
        if (name[i] == '#') {
            throw RuntimeError("RE in CheckVar");
        }
    }
    return;
}

Value Let::eval(Assoc& env) {
    Assoc New_env = env;
    for (auto bind_it : bind) {
        // 不允许相互引用
        CheckVar(bind_it.first);
        Value value = bind_it.second->eval(env);
        New_env = extend(bind_it.first, value, New_env);
    }
    return body.get()->eval(New_env);
}  // let expression

Value Lambda::eval(Assoc& env) {
    Assoc newEnv = env;  // 拷贝当前环境 深拷贝
    return ClosureV(x, e, newEnv);
}  // lambda expression

Value Apply::eval(Assoc& e) {
    Value func = rator.get()->eval(e);
    if (func.get()->v_type != V_PROC) {
        throw RuntimeError("RuntimeError: Not a procedure");
    }
    auto closure_ = dynamic_cast<Closure*>(func.get());

    // 2.对 rand 中的每个实参数求值
    std::vector<Value> evaluatedArgs;
    for (const auto& rand : rand) {
        evaluatedArgs.push_back(rand->eval(e));  // 依次对每个参数求值
    }

    // 3.检查参数数量 实参形参是否匹配
    if (evaluatedArgs.size() != closure_->parameters.size()) {
        throw RuntimeError("RuntimeError: Argument count mismatch");
    }

    // 4.创建新作用域，将形参与实参绑定
    Assoc newEnv = closure_->env;  // 使用闭包中的环境
    for (size_t i = 0; i < closure_->parameters.size(); ++i) {
        // 将形参绑定成实参 参数遮蔽
        newEnv = extend(closure_->parameters[i], evaluatedArgs[i], newEnv);
    }
    return closure_->e.get()->eval(newEnv);  // 在新环境中求值
}  // for function calling

Value Letrec::eval(Assoc& env) {
    Assoc New_env = env;

    // 1. 先将所有变量绑定到 nullptr
    for (const auto& bind_it : bind) {
        New_env = extend(bind_it.first, NullV(), New_env);
    }
    // 2 . 再次求值
    for (const auto& bind_it : bind) {
        Value val = bind_it.second->eval(New_env);
        modify(bind_it.first, val, New_env);
    }
    return body.get()->eval(New_env);
}  // letrec expression

Value Var::eval(Assoc& e) {
    // 先检查是否合法
    CheckVar(x);
    Value it = find(x, e);
    if (it.get() == nullptr) {
        if (primitives.count(x)) {
            // primitive 关键字
            std::vector<std::string> parameters_;  // 形参
            const auto& it = this;
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
                    parameters_ = {"Mult1", "Mult2"};
                    exp = (new Mult(new Var("Mult1"), new Var("Mult2")));
                    break;
                }
                case E_MINUS: {
                    // -
                    parameters_ = {"Minus1", "Minus2"};
                    exp = (new Minus(new Var("Minus1"), new Var("Minus2")));
                    break;
                }
                case E_PLUS: {
                    // +
                    parameters_ = {"Plus1", "Plus2"};
                    exp = (new Plus(new Var("Plus1"), new Var("Plus2")));
                    break;
                }
                case E_LT: {
                    // <
                    parameters_ = {"Lt1", "Lt2"};
                    exp = (new Less(new Var("Lt1"), new Var("Lt2")));
                    break;
                }
                case E_LE: {
                    // <=
                    parameters_ = {"Le1", "Le2"};
                    exp = (new LessEq(new Var("Le1"), new Var("Le2")));
                    break;
                }
                case E_EQ: {
                    // ==
                    parameters_ = {"Eq1", "Eq2"};
                    exp = (new Equal(new Var("Eq1"), new Var("Eq2")));
                    break;
                }
                case E_GE: {
                    // >=
                    parameters_ = {"Ge1", "Ge2"};
                    exp = (new GreaterEq(new Var("Ge1"), new Var("Ge2")));
                    break;
                }
                case E_GT: {
                    // >
                    parameters_ = {"Gt1", "Gt2"};
                    exp = (new Greater(new Var("Gt1"), new Var("Gt2")));
                    break;
                }
                case E_EQQ: {
                    // "eq?"
                    parameters_ = {"EqQ1", "EqQ2"};
                    exp = (new IsEq(new Var("EqQ1"), new Var("EqQ2")));
                    break;
                }
                case E_BOOLQ: {
                    // "boolean?"
                    parameters_ = {"BoolQ"};
                    exp = (new IsBoolean(new Var("BoolQ")));
                    break;
                }
                case E_INTQ: {
                    // "fixnum?"
                    parameters_ = {"IntQ"};
                    exp = (new IsFixnum(new Var("IntQ")));
                    break;
                }
                case E_NULLQ: {
                    // "null?"
                    parameters_ = {"NullQ"};
                    exp = (new IsNull(new Var("NullQ")));
                    break;
                }
                case E_PAIRQ: {
                    // "pair?"
                    parameters_ = {"PairQ"};
                    exp = (new IsPair(new Var("PairQ")));
                    break;
                }
                case E_PROCQ: {
                    // "procedure?"
                    parameters_ = {"ProcQ"};
                    exp = (new IsProcedure(new Var("ProcQ")));
                    break;
                }
                case E_SYMBOLQ: {
                    // "symbol?"
                    parameters_ = {"SymbolQ"};
                    exp = (new IsSymbol(new Var("SymbolQ")));
                    break;
                }
                case E_CONS: {
                    // cons
                    parameters_ = {"Cons1", "Cons2"};
                    exp = (new Cons(new Var("Cons1"), new Var("Cons2")));
                    break;
                }
                case E_NOT: {
                    // not
                    parameters_ = {"Not"};
                    exp = (new Not(new Var("Not")));
                    break;
                }
                case E_CAR: {
                    // car
                    parameters_ = {"Car"};
                    exp = (new Car(new Var("Car")));
                    break;
                }
                case E_CDR: {
                    // cdr
                    parameters_ = {"Cdr"};
                    exp = (new Cdr(new Var("Cdr")));
                    break;
                }
                case E_VOID: {
                    // void
                    exp = (new MakeVoid());
                    break;
                }
                case E_EXIT: {
                    exp = (new Exit());
                    break;
                }
            }
            Assoc new_env = e;
            return ClosureV(parameters_, exp, new_env);
        } else {
            throw RuntimeError("Error in Var->eval");
        }
    } else {
        return it;
    }
}  // evaluation of variable

Value Fixnum::eval(Assoc& e) {
    return IntegerV(n);
}  // evaluation of a fixnum

Value If::eval(Assoc& e) {
    auto tmp_ = cond.get()->eval(e);
    if (auto it = dynamic_cast<Boolean*>(tmp_.get())) {
        if (it->b == 0) {
            auto it_value = alter.get()->eval(e);
            return it_value;
        }
    }
    auto it_value = conseq.get()->eval(e);
    return it_value;
}  // if expression

Value True::eval(Assoc& e) {
    return BooleanV(1);
}  // evaluation of #t

Value False::eval(Assoc& e) {
    return BooleanV(0);
}  // evaluation of #f

Value Begin::eval(Assoc& e) {
    Value task_value(nullptr);
    for (int it = 0; it < es.size(); it++) {
        task_value = es[it].get()->eval(e);
    }
    return task_value;
}  // begin expression

Value Quote::eval(Assoc& e) {
    if (auto it = dynamic_cast<Number*>(s.get())) {
        return IntegerV(it->n);
    } else if (auto it = dynamic_cast<TrueSyntax*>(s.get())) {
        return BooleanV(1);
    } else if (auto it = dynamic_cast<FalseSyntax*>(s.get())) {
        return BooleanV(0);
    } else if (auto it = dynamic_cast<Identifier*>(s.get())) {
        // symbol 为单个独立的变量 防止变成函数引用
        return SymbolV(it->s);
    } else if (auto it = dynamic_cast<List*>(s.get())) {
        if (it->stxs.empty()) {
            // 空列表判断为NUll
            return NullV();
        } else if (it->stxs.size() == 1) {
            Value firstValue = Quote(it->stxs[0]).eval(e);
            return PairV(firstValue, NullV());
        } else {
            // 先检查是不是 (a . b) 格式
            int find_dot_pos = -1, find_dot_cnt = 0;
            for (int i = 0; i < it->stxs.size(); i++) {
                if (auto tmp_it = dynamic_cast<Identifier*>(it->stxs[i].get())) {
                    if (tmp_it->s == ".") {
                        find_dot_pos = i;
                        find_dot_cnt++;
                    }
                }
            }
            if (find_dot_cnt > 1 || ((find_dot_pos != it->stxs.size() - 2) && (find_dot_cnt))) {
                throw RuntimeError("Parm isn't fit");
            }
            if (find_dot_cnt == 1 && (it->stxs.size() < 3)) {
                throw RuntimeError("Parm isn't fit");
            }
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
            Value firstValue = Quote(it->stxs.front()).eval(e);
            return PairV(firstValue, restValue);
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
        throw RuntimeError("Wrong with your parametric");
    } else if (auto it1 = dynamic_cast<Boolean*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Boolean*>(rand2.get())) {
            return ((it1->b == it2->b) ? BooleanV(1) : BooleanV(0));
        }
        throw RuntimeError("Wrong with your parametric");
    } else if (auto it1 = dynamic_cast<Symbol*>(rand1.get())) {
        if (auto it2 = dynamic_cast<Symbol*>(rand2.get())) {
            return ((it1->s == it2->s) ? BooleanV(1) : BooleanV(0));
        }
        throw RuntimeError("Wrong with your parametric");
    } else if (dynamic_cast<Null*>(rand1.get()) || dynamic_cast<Null*>(rand2.get())) {
        if (dynamic_cast<Null*>(rand1.get()) && dynamic_cast<Null*>(rand2.get())) {
            return BooleanV(1);
        } else {
            return BooleanV(0);
        }
        throw RuntimeError("Wrong with your parametric");
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
        throw RuntimeError("RE in Car eval");
    }

}  // car

Value Cdr::evalRator(const Value& rand) {
    if (auto it = dynamic_cast<Pair*>(rand.get())) {
        return it->cdr;
    } else {
        throw RuntimeError("RE in Cdr eval");
    }
}  // cdr