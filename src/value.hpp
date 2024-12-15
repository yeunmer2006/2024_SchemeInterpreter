#ifndef VALUE
#define VALUE

#include <cstring>
#include <memory>
#include <vector>
#include "Def.hpp"
#include "expr.hpp"
#include "shared.hpp"

// 所有值的基类，提供基本的接口：
struct ValueBase {
    ValueType v_type;
    ValueBase(ValueType);
    virtual void show(std::ostream&) = 0;  // 虚函数 具体实现方式由派生类提供
    virtual void showCdr(std::ostream&);
    virtual ~ValueBase() = default;
};

// 封装了所有值
struct Value {
    // SharedPtr<ValueBase> ptr;  // 储存派生类 这个才是真正的valuebase 而外部嵌套的Value是一个封装
    std::shared_ptr<ValueBase> ptr;
    Value(ValueBase*);
    void show(std::ostream&);
    ValueBase* operator->() const;
    ValueBase& operator*();
    ValueBase* get() const;  // get 直接指向 ptr.get
};

// 一个封装类，管理指向 AssocList 的智能指针，提供类似 Value 的操作接口。
struct Assoc {
    // SharedPtr<AssocList> ptr;
    std::shared_ptr<AssocList> ptr;
    Assoc(AssocList*);
    AssocList* operator->() const;
    AssocList& operator*();
    AssocList* get() const;
};

// 一个链表节点，用来实现变量绑定和环境
struct AssocList {
    // 变量名为 x 的绑定,其值为 v
    // 如果链表中存在多个同名变量，第一个匹配到的（即离当前作用域最近的）变量就是当前有效的变量，而后续的同名变量相当于被遮蔽了。
    std::string x;
    Value v;
    Assoc next;
    AssocList(const std::string&, const Value&, Assoc&);
};

struct Void : ValueBase {
    Void();
    virtual void show(std::ostream&) override;
};
Value VoidV();  // 值构造函数（返回封装的 Value 对象）

// 整数value
struct Integer : ValueBase {
    int n;
    Integer(int);
    virtual void show(std::ostream&) override;
};
Value IntegerV(int);

// 布尔value
struct Boolean : ValueBase {
    bool b;
    Boolean(bool);
    virtual void show(std::ostream&) override;
};
Value BooleanV(bool);

// 符号字符串value
struct Symbol : ValueBase {
    std::string s;
    Symbol(const std::string&);
    virtual void show(std::ostream&) override;
};
Value SymbolV(const std::string&);

// 空value
struct Null : ValueBase {
    Null();
    virtual void show(std::ostream&) override;
    virtual void showCdr(std::ostream&) override;
};
Value NullV();

// 结束值
struct Terminate : ValueBase {
    Terminate();
    virtual void show(std::ostream&) override;
};
Value TerminateV();

// Pair 的value
struct Pair : ValueBase {
    Value car;  // 一个对的第一个元素
    Value cdr;  // 一个对的其余部分。

    Pair(const Value&, const Value&);
    virtual void show(std::ostream&) override;
    virtual void showCdr(std::ostream&) override;
};
Value PairV(const Value&, const Value&);

// 函数Value 将一个函数的参数、函数体以及它的环境封装在一起。
struct Closure : ValueBase {
    // Closure 类将一个函数的参数、函数体以及它的环境封装在一起。
    std::vector<std::string> parameters;
    Expr e;
    Assoc env;
    Closure(const std::vector<std::string>&, const Expr&, const Assoc&);
    virtual void show(std::ostream&) override;
};  // 它是一个表示闭包（closure）的数据结构。闭包是函数式编程中的一个重要概念，表示一个函数以及该函数的环境（即变量的绑定）。
Value ClosureV(const std::vector<std::string>&, const Expr&, const Assoc&);

// string value
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