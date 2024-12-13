#ifndef SYNTAX
#define SYNTAX

#include <cstring>
#include <memory>
#include <vector>
#include "Def.hpp"
#include "shared.hpp"

struct SyntaxBase {
    // Syntax 的基类 派生出子类 Number 等
    virtual Expr parse(Assoc&) = 0;        // 一个纯虚函数，所有派生类都必须提供自己的 parse 实现，用于解析该语法结构。
    virtual void show(std::ostream&) = 0;  // 一个纯虚函数，所有派生类都必须提供自己的 show 实现，用于解析该语法结构。
    virtual ~SyntaxBase() = default;
};

struct Syntax {
    SharedPtr<SyntaxBase> ptr;
    // 此处SharedPtr是模仿share_ptr写的智能指针
    Syntax(SyntaxBase*);
    SyntaxBase* operator->() const;
    SyntaxBase& operator*();
    SyntaxBase* get() const;  // 调用Syntax中的Syntaxbase(因为实际是share_ptr内的指针)
    Expr parse(Assoc&);
};

struct Number : SyntaxBase {
    int n;
    Number(int);
    virtual Expr parse(Assoc&) override;
    virtual void show(std::ostream&) override;
};

struct TrueSyntax : SyntaxBase {
    // TrueSyntax();
    virtual Expr parse(Assoc&) override;
    virtual void show(std ::ostream&) override;
};

struct FalseSyntax : SyntaxBase {
    // FalseSyntax();
    virtual Expr parse(Assoc&) override;
    virtual void show(std ::ostream&) override;
};

struct Identifier : SyntaxBase {
    std::string s;
    Identifier(const std::string&);
    virtual Expr parse(Assoc&) override;        // 解析标识符，查找其在环境中的绑定并返回相应的值
    virtual void show(std::ostream&) override;  // 将标识符显示为其名称
};  // 表示一个标识符（变量名或函数名）

struct List : SyntaxBase {
    std ::vector<Syntax> stxs;  // 存储列表中的语法单元
    List();
    virtual Expr parse(Assoc&) override;        // 解析列表，将每个列表元素递归解析为表达式，并返回解析后的结果（可能是一个链表形式的表达式）。
    virtual void show(std::ostream&) override;  // 将列表显示为括号包围的元素
};  // 表示一个由多个语法单元组成的列表，通常用于表示表达式列表（如函数调用或变量赋值中的参数）。

Syntax readSyntax(std::istream&);

std::istream& operator>>(std::istream&, Syntax);
#endif