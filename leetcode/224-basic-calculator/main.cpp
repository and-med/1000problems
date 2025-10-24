#include <iostream>
#include <vector>
#include <stack>
#include <memory>
using namespace std;

namespace {
enum TokenType {
    PLUS, UNARY_PLUS, MINUS, UNARY_MINUS, LEFT_BRACKET, RIGHT_BRACKET, NUMBER,
};

class Token {
public:
    Token(TokenType type, string value, int realpos): _type(type), _value(value), _realpos(realpos) {}
    Token(TokenType type, char value, int realpos): _type(type), _value{value}, _realpos(realpos) {}
    TokenType type() const { return _type; }
    string value() const { return _value; }
    int pos() const { return _realpos; }
    int asNumber() const { 
        if (!isNumber()) {
            throw new runtime_error("token '" + _value + "' is not a consumeNumber!");
        }
        return atoi(_value.c_str()); 
    }
    bool isBinaryOp() const { return _type == PLUS || _type == MINUS; }
    bool isUnaryOp() const { return _type == UNARY_PLUS || _type == UNARY_MINUS; }
    bool isNumber() const { return _type == NUMBER; }
    bool isLeftBracket() const { return _type == LEFT_BRACKET; }
    bool isRightBracket() const { return _type == RIGHT_BRACKET; }
private:
    TokenType _type;
    string _value;
    int _realpos;
};

class Tokenizer {
public:
    Tokenizer(string s): _data(s), _pos(0), _result({}) {}

    vector<Token> tokenize() {
        while (!isAtEnd()) {
            char c = peek();

            if (c == '+') {
                addToken(TokenType::PLUS);
            } else if (c == '-') {
                addToken(TokenType::MINUS);
            } else if (c == '(') {
                addToken(TokenType::LEFT_BRACKET);
            } else if (c == ')') {
                addToken(TokenType::RIGHT_BRACKET);
            } else if (c == ' ') {
                advance();
            } else if (c >= '0' && c <= '9') {
                addNumber();
            }
        }
        return _result;
    }

    void addToken(TokenType type) {
        _result.push_back(Token(type, peek(), _pos));
        advance();
    }

    void addNumber() {
        string consumeNumber;
        int start = _pos;
        while (!isAtEnd() && peek() >= '0' && peek() <= '9') {
            consumeNumber += peek();
            advance();
        }
        // let's assume its non-empty valid 32 bit signed integer
        _result.push_back(Token(TokenType::NUMBER, consumeNumber, start));
    }

    char peek() {
        return _data[_pos];
    }

    void advance() {
        _pos++;
    }

    bool isAtEnd() {
        return _pos >= _data.length();
    }

private:
    string _data;
    int _pos;
    vector<Token> _result;
};

class Expr {
public:
    virtual int eval() = 0;
    virtual string toString() = 0;
};

class NumberExpr: public Expr {
public:
    NumberExpr(int number): _number(number) {}
    int eval() override {
        return _number;
    }
    string toString() override {
        return std::to_string(_number);
    }
private:
    int _number;
};

class BinaryOp: public Expr {
public:
    BinaryOp(unique_ptr<Expr> left, Token op, unique_ptr<Expr> right): _left(move(left)), _op(op), _right(move(right)) {}
    int eval() override {
        if (_op.type() == TokenType::PLUS) {
            return _left->eval() + _right->eval();
        } else if (_op.type() == TokenType::MINUS) {
            return _left->eval() - _right->eval();
        }
        throw new runtime_error("invalid token type");
    }
    string toString() override {
        return "(" + _left->toString() + _op.value() + _right->toString() + ")";
    }
private:
    unique_ptr<Expr> _left;
    unique_ptr<Expr> _right;
    Token _op;
};

class UnaryOp: public Expr {
public:
    UnaryOp(Token op, unique_ptr<Expr> expr): _op(op), _expr(move(expr)) {}
    int eval() override {
        if (_op.type() == TokenType::PLUS) {
            return _expr->eval();
        } else if (_op.type() == TokenType::MINUS) {
            return -_expr->eval();
        }
        throw new runtime_error("invalid token type");
    }
    string toString() override {
        return _op.value() + _expr->toString();
    }
private:
    unique_ptr<Expr> _expr;
    Token _op;
};

class Evaluator {
public:
    Evaluator(vector<Token> tokens): _tokens(tokens), _pos(0) {}

    // rules:
    // <unary_expr> = <un_op>(<expr>) (<bin_op> <expr>)*
    // <expr> = "(" <expr> ")" (<bin_op> <expr>)*
    // <expr> = <consumeNumber> (<bin_op> <expr>)*
    // <expr> = <un_op><consumeNumber> (<bin_op> <expr>)*
    // <bin_op> = +|-
    // <un_op> = +|-

    bool expression() {
        if (isNumber()) {
            auto number = consumeTop();

            if (isOperation()) {
                auto op = consumeTop();

                if (!expression()) {
                    return error("expected expression after binary operation");
                }
                auto rightExpr = move(_expressions.top());
                _expressions.pop();
                _expressions.push(make_unique<BinaryOp>(make_unique<NumberExpr>(number.asNumber()), op, move(rightExpr)));
            } else {
                _expressions.push(make_unique<NumberExpr>(number.asNumber()));
            }
            return true;
        }

        // nested expression
        if (isLeftBracket()) {
            consumeTop();
            if (!expression()) {
                return error("expected expression after left bracket");
            }

            if (!isRightBracket()) {
                return error("expected right bracket after opening left bracket and expression");
            }
            consumeTop();

            auto leftExpr = move(_expressions.top());
            _expressions.pop();

            if (isOperation()) {
                auto op = consumeTop();

                if (!expression()) {
                    return error("expected expression after binary operation");
                }
                auto rightExpr = move(_expressions.top());
                _expressions.pop();
                _expressions.push(make_unique<BinaryOp>(move(leftExpr), op, move(rightExpr)));
            } else {
                _expressions.push(move(leftExpr));
            }
            return true;
        }

        // Unary Operation
        if (isOperation()) {
            auto op = consumeTop();
            if (isNumber()) {
                auto number = consumeTop().asNumber();
                auto expr = make_unique<UnaryOp>(op, make_unique<NumberExpr>(number));

                if (isOperation()) {
                    auto op = consumeTop();

                    if (!expression()) {
                        return error("expected expression after binary operation");
                    }
                    auto rightExpr = move(_expressions.top());
                    _expressions.pop();
                    _expressions.push(make_unique<BinaryOp>(move(expr), op, move(rightExpr)));
                } else {
                    _expressions.push(move(expr));
                }
            } else if (isLeftBracket()) {
                consumeTop();
                if (!expression()) {
                    return error("expected expression after left bracket");
                }

                if (!isRightBracket()) {
                    return error("expected right bracket after opening left bracket and expression");
                }
                consumeTop();

                auto expr = move(_expressions.top());
                _expressions.push(make_unique<UnaryOp>(op, move(expr)));
            }
            return true;
        }

        return error("unexpected token");
    }

    int value() {
        if (!isValid()) {
            throw new runtime_error("stack is invalid!");
        }

        return _expressions.top()->eval();
    }

    bool isValid() {
        return _expressions.size() == 1;
    }

    bool error(string message) {
        string position;

        if (atEnd()) {
            position = "EOF";
        } else {
            position += to_string(peek().pos());
        }

        string value;
        if (atEnd()) {
            value = "EOF";
        } else {
            value = peek().value();
        }

        cout << message + " at position " + position + " but got '" + value + "'" << endl;
        return false;
    }

    bool isOperation() { return !atEnd() && peek().isBinaryOp(); }
    bool isNumber() { return !atEnd() && peek().isNumber(); }
    bool isLeftBracket() { return !atEnd() && peek().isLeftBracket(); }
    bool isRightBracket() { return !atEnd() && peek().isRightBracket(); }
    Token consumeTop() {
        auto top = peek();
        advance();
        return top;
    }

    Token peek() {
        return _tokens[_pos];
    }

    bool atEnd() {
        return _pos >= _tokens.size();
    }

    void advance() {
        _pos++;
    }
private:
    vector<Token> _tokens;
    int _pos;
    stack<unique_ptr<Expr>> _expressions;
};
};

class Solution {
public:
    int calculate(string s) {
        // Convert string to token for ease of processing
        Tokenizer tokenizer(s);
        vector<Token> tokens = tokenizer.tokenize();

        Evaluator eval(tokens);

        if (eval.expression() && eval.isValid()) {
            return eval.value();
        }

        return -1;
    }
};

int main() {
    Solution sol;

    cout << sol.calculate("1 + 1") << endl;
    cout << sol.calculate(" 2-1 + 2 ") << endl;
    cout << sol.calculate("(1+(4+5+2)-3)+(6+8)") << endl;
}
