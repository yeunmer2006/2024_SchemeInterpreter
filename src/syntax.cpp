#include "syntax.hpp"
#include <cstring>
#include <vector>

Syntax ::Syntax(SyntaxBase* stx)
    : ptr(stx) {}
SyntaxBase* Syntax ::operator->() const {
    return ptr.get();
}
SyntaxBase& Syntax ::operator*() {
    return *ptr;
}
SyntaxBase* Syntax ::get() const {
    return ptr.get();
}

Number ::Number(int n)
    : n(n) {}
void Number::show(std::ostream& os) {
    os << "the-number-" << n;
}

void TrueSyntax::show(std::ostream& os) {
    os << "#t";
}

void FalseSyntax::show(std::ostream& os) {
    os << "#f";
}

Identifier ::Identifier(const std ::string& s1)
    : s(s1) {}
void Identifier::show(std::ostream& os) {
    os << s;
}

List ::List() {}
void List::show(std::ostream& os) {
    os << '(';
    for (auto stx : stxs) {
        stx->show(os);
        os << ' ';
    }
    os << ')';
}

std::istream& readSpace(std::istream& is) {
    while (isspace(is.peek()))  // is.peak()查看输入流的下一个字符 但不提取
        is.get();               // 如果是空白字符 则提取
    return is;                  // 返回第一个非空白字符或者是流的结束
}

Syntax readList(std::istream& is);

// no leading space
Syntax readItem(std::istream& is) {
    if (is.peek() == '(' || is.peek() == '[') {
        is.get();
        return readList(is);
    }
    if (is.peek() == '\'') {
        is.get();
        return readList(is);
    }
    std::string s;
    do {
        int c = is.peek();
        if (c == '(' || c == ')' || c == '[' || c == ']' || isspace(c) || c == EOF)
            break;
        is.get();
        s.push_back(c);
    } while (true);
    // try parsing a integer
    // 尝试解析字符串 可能为 整数 #t #f 或者是标识符
    bool neg = false;
    int n = 0;
    int i = 0;
    if (s.size() == 1 && (s[0] == '+' || s[0] == '-'))
        goto identifier;
    if (s[0] == '-') {
        i += 1;
        neg = true;
    } else if (s[0] == '+')
        i += 1;
    for (; i < s.size(); i++)
        if ('0' <= s[i] && s[i] <= '9')
            n = n * 10 + s[i] - '0';
        else
            goto identifier;
    if (neg)
        n = -n;
    return Syntax(new Number(n));
identifier:
    // not a number
    if (s == "#t")
        return Syntax(new TrueSyntax());
    if (s == "#f")
        return Syntax(new FalseSyntax());
    return Syntax(new Identifier(s));
}

Syntax readList(std::istream& is) {
    // 此处递归实现对语法树的解析
    List* stx = new List();
    while (readSpace(is).peek() != ')' && readSpace(is).peek() != ']')
        stx->stxs.push_back(readItem(is));  // 相当于对一对括号之间的字符进行解析 如果其中仍然有括号 那么递归嵌套
    is.get();                               // 此处相当于读入了')'
    return Syntax(stx);
}

Syntax readSyntax(std::istream& is) {
    return readItem(readSpace(is));
}

std::istream& operator>>(std::istream& is, Syntax& stx) {
    stx = readSyntax(is);
    return is;
}
