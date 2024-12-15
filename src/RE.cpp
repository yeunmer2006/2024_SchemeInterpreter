#include "RE.hpp"
#include <cstring>

RuntimeError ::RuntimeError(std ::string s1)
    : s(s1) {}
std ::string RuntimeError ::message() const {
    return s;
}

void checkArgCount(int expected, int actual) {
    if (expected != actual) {
        throw RuntimeError("Expected " + std::to_string(expected - 1) + " arguments, but got " + std::to_string(actual - 1));
    }
}  // 一个定义的 检查参数数量是否达标