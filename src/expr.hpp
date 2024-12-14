#ifndef EXPRESSION
#define EXPRESSION

#include <cstring>
#include <memory>
#include <vector>
#include "Def.hpp"
#include "shared.hpp"
#include "syntax.hpp"

struct ExprBase {
    ExprType e_type;
    ExprBase(ExprType);
    virtual Value eval(Assoc&) = 0;  // eval用来计算函数的值
    virtual ~ExprBase() = default;
};

struct Expr {
    SharedPtr<ExprBase> ptr;
    Expr(ExprBase*);
    ExprBase* operator->() const;
    ExprBase& operator*();
    ExprBase* get() const;
};

struct Let : ExprBase {
    std::vector<std::pair<std::string, Expr>> bind;
    Expr body;
    Let(const std ::vector<std ::pair<std ::string, Expr>>&, const Expr&);
    virtual Value eval(Assoc&) override;
};  // let 表达式，表示绑定变量和值，并有一个表达式体

struct Lambda : ExprBase {
    std::vector<std::string> x;
    Expr e;
    Lambda(const std ::vector<std ::string>&, const Expr&);
    virtual Value eval(Assoc&) override;
};  // 表示一个函数定义

struct Apply : ExprBase {
    Expr rator;              // 表示被调用函数
    std::vector<Expr> rand;  // 储存参数的容器
    Apply(const Expr&, const std ::vector<Expr>&);
    virtual Value eval(Assoc&) override;
};  // this is used to handle function calling, where rator is the operator and rands are operands 表示函数调用

struct Letrec : ExprBase {
    std::vector<std::pair<std::string, Expr>> bind;
    Expr body;
    Letrec(const std ::vector<std ::pair<std ::string, Expr>>&, const Expr&);
    virtual Value eval(Assoc&) override;
    int x;
};

struct Var : ExprBase {
    std::string x;
    Var(const std ::string&);
    virtual Value eval(Assoc&) override;
};  // 表示变量

struct Fixnum : ExprBase {
    int n;
    Fixnum(int);
    virtual Value eval(Assoc&) override;
};  // 表示整数常量

struct If : ExprBase {
    Expr cond;
    Expr conseq;
    Expr alter;
    If(const Expr&, const Expr&, const Expr&);
    virtual Value eval(Assoc&) override;
};

struct True : ExprBase {
    True();
    virtual Value eval(Assoc&) override;
};

struct False : ExprBase {
    False();
    virtual Value eval(Assoc&) override;
};

struct Begin : ExprBase {
    std::vector<Expr> es;
    Begin(const std ::vector<Expr>&);
    virtual Value eval(Assoc&) override;
};  // 表示顺序表达式序列

struct Quote : ExprBase {
    Syntax s;
    Quote(const Syntax&);
    virtual Value eval(Assoc&) override;
};  // 引用表达式

struct MakeVoid : ExprBase {
    MakeVoid();
    virtual Value eval(Assoc&) override;
};

struct Exit : ExprBase {
    Exit();
    virtual Value eval(Assoc&) override;
};

struct Binary : ExprBase {
    Expr rand1;
    Expr rand2;
    Binary(ExprType, const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) = 0;
    virtual Value eval(Assoc&) override;
};  // 表示二元操作

struct Unary : ExprBase {
    Expr rand;
    Unary(ExprType, const Expr&);
    virtual Value evalRator(const Value&) = 0;
    virtual Value eval(Assoc&) override;
};  // 表示一元操作

struct Mult : Binary {
    Mult(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 乘法

struct Plus : Binary {
    Plus(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 加法

struct Minus : Binary {
    Minus(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 减法

struct Less : Binary {
    Less(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 小于

struct LessEq : Binary {
    LessEq(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 小于等于

struct Equal : Binary {
    Equal(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 等于

struct GreaterEq : Binary {
    GreaterEq(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 大于等于

struct Greater : Binary {
    Greater(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 大于

struct IsEq : Binary {
    IsEq(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 检查两个表达式是否完全相同

struct Cons : Binary {
    Cons(const Expr&, const Expr&);
    virtual Value evalRator(const Value&, const Value&) override;
};  // 创建一个新的有序对（或列表）

struct IsBoolean : Unary {
    IsBoolean(const Expr&);
    virtual Value evalRator(const Value&) override;
};  // 是否是bool值

struct IsFixnum : Unary {
    IsFixnum(const Expr&);
    virtual Value evalRator(const Value&) override;
};

struct IsSymbol : Unary {
    IsSymbol(const Expr&);
    virtual Value evalRator(const Value&) override;
};

struct IsNull : Unary {
    IsNull(const Expr&);
    virtual Value evalRator(const Value&) override;
};

struct IsPair : Unary {
    IsPair(const Expr&);
    virtual Value evalRator(const Value&) override;
};

struct IsProcedure : Unary {
    IsProcedure(const Expr&);
    virtual Value evalRator(const Value&) override;
};  // 这个 Procedure 是一个函数对象，它有自己的 eval 方法，并且通常由解释器或者运行时系统动态处理

struct Not : Unary {
    Not(const Expr&);
    virtual Value evalRator(const Value&) override;
};  // 取反

struct Car : Unary {
    Car(const Expr&);
    virtual Value evalRator(const Value&) override;
};  // 操作列表的头

struct Cdr : Unary {
    Cdr(const Expr&);
    virtual Value evalRator(const Value&) override;
};  // 操作列表的尾

#endif
