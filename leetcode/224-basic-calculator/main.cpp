#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <optional>
using namespace std;

enum TokenType {
    PLUS, MINUS, LEFT_BRACKET, RIGHT_BRACKET, NUMBER,
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
    bool isNumber() const { return _type == NUMBER; }
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
    virtual ~Expr() {}
};

class NumberExpr: public Expr {
public:
    NumberExpr(int number): _number(number) {}
    ~NumberExpr() override {}
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
    ~BinaryOp() override {}
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
    ~UnaryOp() override {}
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

class Parser {
public:
    Parser(vector<Token> tokens): _tokens(tokens), _pos(0) {}

    unique_ptr<Expr> expression() {
        return addition();
    }

    unique_ptr<Expr> addition() {
        unique_ptr<Expr> left = move(unary());

        while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
            auto op = consumeTop();
            auto right = move(unary());
            left = make_unique<BinaryOp>(move(left), op, move(right));
        }

        return left;
    }

    unique_ptr<Expr> unary() {
        if (check(TokenType::PLUS) || check(TokenType::MINUS)) {
            auto op = consumeTop();
            unique_ptr<Expr> right = move(unary());
            return make_unique<UnaryOp>(op, move(right));
        }

        return primary();
    }

    unique_ptr<Expr> primary() {
        if (check(TokenType::NUMBER)) {
            auto top = consumeTop();
            return make_unique<NumberExpr>(top.asNumber());
        }

        if (check(TokenType::LEFT_BRACKET)) {
            // consume left bracket
            consumeTop();
            auto inner = move(expression());
            if (!check(TokenType::RIGHT_BRACKET)) {
                throw error("unmatched left bracket");
            }
            // consume right bracket
            consumeTop();
            return inner;
        }

        throw error("expected expression");
    }

    runtime_error error(string message) {
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

        string error_message = message + " at position " + position + " but got '" + value + "'";
        return runtime_error(error_message);
    }

    Token consumeTop() {
        auto top = peek();
        advance();
        return top;
    }

    bool check(TokenType type) {
        if (atEnd()) return false;
        return peek().type() == type;
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
};

class Solution {
public:
    int calculate(string s) {
        Tokenizer tokenizer(s);
        Parser parser(tokenizer.tokenize());
        return parser.expression()->eval();
    }
};

int main() {
    Solution sol;

    cout << sol.calculate("1 + 1") << endl;
    cout << sol.calculate("   -1 + 3") << endl;
    cout << sol.calculate(" 2-1 + 2 ") << endl;
    cout << sol.calculate("(1+(4+5+2)-3)+(6+8)") << endl;
}
