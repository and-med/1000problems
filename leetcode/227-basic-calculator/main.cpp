#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <optional>
using namespace std;

class Parser {
public:
    Parser(string tokens): _tokens(tokens), _pos(0) {}

    int parse() {
        return addition();
    }

    int addition() {
        int left = multiplication();

        while (check('+') || check('-')) {
            auto op = peek();
            advance();
            auto right = multiplication();
            if (op == '+') {
                left = left + right;
            } else {
                left = left - right;
            }
        }

        return left;
    }

    int multiplication() {
        int left = unary();

        while (check('*') || check('/')) {
            auto op = peek();
            advance();
            auto right = unary();
            if (op == '*') {
                left = left * right;
            } else {
                left = left / right;
            }
        }

        return left;
    }

    int unary() {
        if (check('+') || check('-')) {
            auto op = peek();
            advance();
            int right = unary();

            if (op == '-') {
                return -right;
            }
            return right;
        }

        return primary();
    }

    int primary() {
        if (isNumber()) {
            return getNumber();
        }

        if (check('(')) {
            // consume left bracket
            advance();
            auto inner = parse();
            if (!check(')')) {
                throw error("unmatched left bracket");
            }
            // consume right bracket
            advance();
            return inner;
        }

        throw error("expected expression");
    }

    runtime_error error(string message) {
        string position;

        if (atEnd()) {
            position = "EOF";
        } else {
            position += _pos;
        }

        string value;
        if (atEnd()) {
            value = "EOF";
        } else {
            value = peek();
        }

        string error_message = message + " at position " + position + " but got '" + value + "'";
        return runtime_error(error_message);
    }

    char consumeTop() {
        auto top = peek();
        advance();
        return top;
    }

    bool check(char value) {
        while (!atEnd() && peek() == ' ') {
            advance();
        }
        if (atEnd()) return false;
        return peek() == value;
    }

    bool isNumber() {
        while (!atEnd() && peek() == ' ') {
            advance();
        }
        if (atEnd()) return false;
        auto top = peek();
        return top >= '0' && top <= '9';
    }

    int getNumber() {
        string number;
        while (peek() >= '0' && peek() <= '9') {
            number += peek();
            advance();
        }
        return atoi(number.c_str());
    }

    char peek() {
        return _tokens[_pos];
    }

    bool atEnd() {
        return _pos >= _tokens.size();
    }

    void advance() {
        _pos++;
    }
private:
    string _tokens;
    int _pos;
};

class Solution {
public:
    int calculate(string s) {
        Parser parser(s);
        return parser.parse();
    }
};

int main() {
    Solution sol;

    cout << sol.calculate("1 + 1") << endl;
    cout << sol.calculate("   -1 + 3") << endl;
    cout << sol.calculate(" 2-1 + 2 ") << endl;
    cout << sol.calculate("(1+(4+5+2)-3)+(6+8)") << endl;
    cout << sol.calculate("3+2*2") << endl;
    cout << sol.calculate(" 3/2 ") << endl;
    cout << sol.calculate(" 3+5 / 2 ") << endl;
}
