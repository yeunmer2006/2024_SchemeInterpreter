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

Value Let::eval(Assoc& env) {}  // let expression

Value Lambda::eval(Assoc& env) {}  // lambda expression

Value Apply::eval(Assoc& e) {
}  // for function calling

Value Letrec::eval(Assoc& env) {}  // letrec expression

Value Var::eval(Assoc& e) {
    return SymbolV(x);
}  // evaluation of variable

Value Fixnum::eval(Assoc& e) {
    return IntegerV(n);
}  // evaluation of a fixnum

Value If::eval(Assoc& e) {
    if (auto it = dynamic_cast<False*>(cond.get())) {
        return alter.get()->eval(e);
    } else {
        return conseq.get()->eval(e);
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
            // 非空 则生成 第一个value 和后面的
            Value firstValue = it->stxs.front().parse(e).get()->eval(e);  // List 中 存储第一个syntax进行parser之后再进行eval
            List* remain_list = new List;
            (*remain_list).stxs = std::vector<Syntax>(it->stxs.begin() + 1, it->stxs.end());
            Value restValue = Quote(Syntax(remain_list)).eval(e);
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