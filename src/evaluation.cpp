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
    switch (rator.get()->e_type) {
        case E_MUL: {
            return dynamic_cast<Mult*>(rator.get())->evalRator(rand[0].get()->eval(e), rand[1].get()->eval(e));
        }
        case E_PLUS: {
            return dynamic_cast<Plus*>(rator.get())->evalRator(rand[0].get()->eval(e), rand[1].get()->eval(e));
        }
        case E_MINUS: {
            return dynamic_cast<Minus*>(rator.get())->evalRator(rand[0].get()->eval(e), rand[1].get()->eval(e));
        }
    }
}  // for function calling

Value Letrec::eval(Assoc& env) {}  // letrec expression

Value Var::eval(Assoc& e) {}  // evaluation of variable

Value Fixnum::eval(Assoc& e) {
    return IntegerV(n);
}  // evaluation of a fixnum

Value If::eval(Assoc& e) {}  // if expression

Value True::eval(Assoc& e) {
    return BooleanV(1);
}  // evaluation of #t

Value False::eval(Assoc& e) {
    return BooleanV(0);
}  // evaluation of #f

Value Begin::eval(Assoc& e) {}  // begin expression

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
    } else if (auto pairExpr = dynamic_cast<List*>(s.get())) {
        if (pairExpr->stxs.empty()) {
            // 空列表判断为NUll
            return NullV();
        } else {
            // return PairV();
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

Value Binary::eval(Assoc& e) {}  // evaluation of two-operators primitive

Value Unary::eval(Assoc& e) {}  // evaluation of single-operator primitive

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

Value Less::evalRator(const Value& rand1, const Value& rand2) {}  // <

Value LessEq::evalRator(const Value& rand1, const Value& rand2) {}  // <=

Value Equal::evalRator(const Value& rand1, const Value& rand2) {}  // =

Value GreaterEq::evalRator(const Value& rand1, const Value& rand2) {}  // >=

Value Greater::evalRator(const Value& rand1, const Value& rand2) {}  // >

Value IsEq::evalRator(const Value& rand1, const Value& rand2) {}  // eq?

Value Cons::evalRator(const Value& rand1, const Value& rand2) {}  // cons

Value IsBoolean::evalRator(const Value& rand) {}  // boolean?

Value IsFixnum::evalRator(const Value& rand) {}  // fixnum?

Value IsSymbol::evalRator(const Value& rand) {}  // symbol?

Value IsNull::evalRator(const Value& rand) {}  // null?

Value IsPair::evalRator(const Value& rand) {}  // pair?

Value IsProcedure::evalRator(const Value& rand) {}  // procedure?

Value Not::evalRator(const Value& rand) {}  // not

Value Car::evalRator(const Value& rand) {}  // car

Value Cdr::evalRator(const Value& rand) {}  // cdr
