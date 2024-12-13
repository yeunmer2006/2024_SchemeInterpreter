#ifndef VALUE
#define VALUE

#include <cstring>
#include <memory>
#include <vector>
#include "Def.hpp"
#include "expr.hpp"
#include "shared.hpp"

struct ValueBase {
    ValueType v_type;
    ValueBase(ValueType);
    virtual void show(std::ostream&) = 0;  // 虚函数 具体实现方式由派生类提供
    virtual void showCdr(std::ostream&);
    virtual ~ValueBase() = default;
};  // 所有值的基类，提供基本的接口：

struct Value {
    SharedPtr<ValueBase> ptr;  // 储存派生类 这个才是真正的valuebase 而外部嵌套的Value是一个封装
    Value(ValueBase*);
    void show(std::ostream&);
    ValueBase* operator->() const;
    ValueBase& operator*();
    ValueBase* get() const;  // get 直接指向 ptr.get
};  // 封装了所有值

struct Assoc {
    SharedPtr<AssocList> ptr;
    Assoc(AssocList*);
    AssocList* operator->() const;
    AssocList& operator*();
    AssocList* get() const;
};  // 一个封装类，管理指向 AssocList 的智能指针，提供类似 Value 的操作接口。

struct AssocList {
    // 变量名为 x 的绑定,其值为 v
    std::string x;
    Value v;
    Assoc next;
    AssocList(const std::string&, const Value&, Assoc&);
};  // 一个链表节点，用来实现变量绑定和环境

struct Void : ValueBase {
    Void();
    virtual void show(std::ostream&) override;
};
Value VoidV();  // 值构造函数（返回封装的 Value 对象）

struct Integer : ValueBase {
    int n;
    Integer(int);
    virtual void show(std::ostream&) override;
};
Value IntegerV(int);

struct Boolean : ValueBase {
    bool b;
    Boolean(bool);
    virtual void show(std::ostream&) override;
};
Value BooleanV(bool);

struct Symbol : ValueBase {
    std::string s;
    Symbol(const std::string&);
    virtual void show(std::ostream&) override;
};
Value SymbolV(const std::string&);

struct Null : ValueBase {
    Null();
    virtual void show(std::ostream&) override;
    virtual void showCdr(std::ostream&) override;
};
Value NullV();

struct Terminate : ValueBase {
    Terminate();
    virtual void show(std::ostream&) override;
};
Value TerminateV();

struct Pair : ValueBase {
    Value car;  // 一个对的第一个元素
    Value cdr;  // 一个对的其余部分。

    Pair(const Value&, const Value&);
    virtual void show(std::ostream&) override;
    virtual void showCdr(std::ostream&) override;
};
Value PairV(const Value&, const Value&);

struct Closure : ValueBase {
    std::vector<std::string> parameters;
    Expr e;
    Assoc env;
    Closure(const std::vector<std::string>&, const Expr&, const Assoc&);
    virtual void show(std::ostream&) override;
};
Value ClosureV(const std::vector<std::string>&, const Expr&, const Assoc&);

struct String : ValueBase {
    std ::string s;
    String(const std ::string&);
    virtual void show(std ::ostream&) override;
};
Value StringV(const std ::string&);

std::ostream& operator<<(std::ostream&, Value&);

Assoc empty();                                            // 创建一个空环境
Assoc extend(const std ::string&, const Value&, Assoc&);  // 向环境中添加新的变量绑定。
void modify(const std ::string&, const Value&, Assoc&);   // 修改环境中已有的绑定
Value find(const std::string&, Assoc&);                   // 查找变量绑定的值
#endif