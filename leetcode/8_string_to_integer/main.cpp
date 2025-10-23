#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
using namespace std;

// A class representing large integer
// Internally integer is stored as an array of digits in the reverse order.
// This makes it easier to handle additions and subtractions.
// The last integer additionally stores the sign of the operation.
// for example -50,342 - is represented as digits = { 2, 4, 3, 0, -5}
class Integer {
public:
    Integer(const vector<int>& digits): _digits(digits.size()) {
        for (int i = 0; i < digits.size(); i++) {
            _digits[i] = digits[i];
        }
    }
    
    enum ParsingStage {
        Whitespace, Signedness, ConversionLeadingZero, ConversionFirstDigit, Conversion, Rounding, Invalid
    };

    static Integer fromString(string s) {
        ParsingStage stage = ParsingStage::Whitespace;
        vector<int> digits;
        bool isNegative = false;

        for (int i = 0; i < s.size(); i++) {
            char c = s[i];
            switch(stage) {
            case ParsingStage::Whitespace: {
                if (c == ' ') {
                    break;
                }
                stage = ParsingStage::Signedness;
            }
            case ParsingStage::Signedness: {
                // Always change the stage since this is one character!
                stage = ParsingStage::ConversionLeadingZero;
                if (c == '+' || c == '-') {
                    isNegative = c == '-';
                    break;
                }
            }
            case ParsingStage::ConversionLeadingZero: {
                if (c == '0') {
                    // Go to next character
                    break;
                }
                stage = ParsingStage::ConversionFirstDigit;
            }
            case ParsingStage::ConversionFirstDigit: {
                if (c >= '1' && c <= '9') {
                    int digit = c - '0';
                    if (isNegative) {
                        digit = -digit;
                    }
                    digits.push_back(digit);
                    stage = ParsingStage::Conversion;
                    // Go to next character
                    break;
                } else {
                    stage = ParsingStage::Invalid;
                    break;
                }
            }
            case ParsingStage::Conversion: {
                if (c >= '0' && c <= '9') {
                    digits.push_back(c - '0');
                    // Go to next character
                    break;
                } else {
                    stage = ParsingStage::Rounding;
                }
            }
            case ParsingStage::Rounding: {
                // Exit - seems like the string contains something other than 
                break;
            }
            case ParsingStage::Invalid: {
                break;
            }
            }
        }

        if (stage == ParsingStage::Conversion || stage == ParsingStage::Rounding) {
            reverse(digits.begin(), digits.end());
            Integer res(digits);
            return res;
        }

        return Integer({0});
    }

    int toInt() const {
        int res = _digits[_digits.size() - 1];
        for (int i = _digits.size() - 2; i >= 0; i--) {
            res *= 10;
            res += sign() * _digits[i];
        }
        return res;
    }

    int compare(const Integer& other) const {
        if (isPositive() && other.isPositive()) {
            if (_digits.size() != other._digits.size()) {
                return _digits.size() - other._digits.size();
            } else {
                // both positive, equal digits, compare digit to digit.
                // find first position starting from the highest order digit
                // where digits are not equal, compare them.
                for (int i = _digits.size() - 1; i >= 0; i--) {
                    if (_digits[i] != other._digits[i]) {
                        return _digits[i] - other._digits[i];
                    }
                }

                // All equal
                return 0;
            }
        } else if (isNegative() && other.isNegative()) {
            if (_digits.size() != other._digits.size()) {
                // if other.size() - this.size() > 0 => this > other => this - other > 0
                // if other.size() - this.size() < 0 => this < other => this - other < 0
                return other._digits.size() - _digits.size();
            } else {
                // both negative, equal digits, compare digit to digit

                // -51 < -41
                // -5 - (-4) < 0
                // -51 > -52
                // 1 - 2 < 0
                // do first digit separately since it stores the sign!!
                int lastIdx = _digits.size() - 1;
                if (_digits[lastIdx] != other._digits[lastIdx]) {
                    return _digits[lastIdx] - other._digits[lastIdx];
                }

                for (int i = lastIdx - 1; i >= 0; i--) {
                    if (_digits[i] != other._digits[i]) {
                        return other._digits[i] - _digits[i];
                    }
                }

                // All equal
                return 0;
            }
        } else {
            // different signs!
            if (other.isNegative()) {
                // this > other => this - other > 0
                return 1;
            }
            return -1;
        }
    }

    string toString() const {
        string output;

        int order = 0;
        for (int i = 0; i < _digits.size(); i++) {
            if (order == 3) {
                // Put nice commas in here
                output += ',';
                order = 0;
            }

            if (i == _digits.size() - 1 && _digits[i] < 0) {
                // Handle the negative value in last integer
                output += '0' + -_digits[i];
                output += '-';
            } else {
                output += '0' + _digits[i];
            }

            order++;
        }

        reverse(output.begin(), output.end());
        return output;
    }

    int sign() const {
        return isNegative() ? -1 : 1;
    }

    bool isPositive() const {
        return _digits[_digits.size() - 1] > 0;
    }

    bool isNegative() const {
        return _digits[_digits.size() - 1] < 0;
    }

private:
    vector<int> _digits;
};

class Solution {
public:
    int myAtoi(string s) {
        Integer myInt = Integer::fromString(s);
        Integer minInt = Integer::fromString("-2147483648");
        Integer maxInt = Integer::fromString("2147483647");

        if (myInt.compare(minInt) < 0) {
            return minInt.toInt();
        } else if (myInt.compare(maxInt) > 0) {
            return maxInt.toInt();
        }
        return myInt.toInt();
    }
};

struct comparisonTest {
    string left;
    string right;
    bool lessThan;
    bool equal;
    bool greaterThan;
};

string toStr(bool value) {
    return value ? "true" : "false";
}

bool outputTestResult(comparisonTest t, string symbol, bool actual, bool expected) {
    if (actual == expected) {
        // No need to output successes!
        // cout << "PASS: " << t.left << " " << symbol << " " << t.right << " is " << toStr(expected) << endl;
        return true;
    } else {
        cout << "FAIL: "  << t.left << " " << symbol << " " << t.right << ", expected=" << toStr(expected) << ", actual=" << toStr(actual) << endl;
        return false;
    }
}

int main() {
    Integer testInt({ 2, 4, 3, 0, -5 });
    cout << testInt.toString() << endl;

    Integer testInt2({ 3, 2, 1, 3, 2, -1});
    cout << testInt2.toString() << endl;

    Integer testInt3({ 3, 2, 1, 3, 2, 1, -3});
    cout << testInt3.toString() << endl;

    Integer testInt4({ -3});
    cout << testInt4.toString() << endl;

    Integer testInt5 = Integer::fromString("-50342abc");
    cout << testInt5.toString() << endl;

    cout << "2^2 = " << (1<<2) << endl;
    cout << "2^31 - 1 = " << (1<<30) + ((1<<30) - 1) << endl;
    cout << "-2^31 = " << -(1<<30)-(1<<30) << endl;

    cout << "running some tests" << endl;

    vector<comparisonTest> tests {
        comparisonTest{"-5", "-4", lessThan: true },
        comparisonTest{"-51", "-52", greaterThan: true },
        comparisonTest{"-51", "52", lessThan: true },
        comparisonTest{"51", "51", equal: true },
        comparisonTest{"51", "512", lessThan: true },
        comparisonTest{"512", "51", greaterThan: true },
        comparisonTest{"52", "51", greaterThan: true },
        comparisonTest{"-51234", "-41234", lessThan: true },
        comparisonTest{"-51234", "-4123", lessThan: true },
        comparisonTest{"-5123", "-41234", greaterThan: true },
    };

    int success = 0;
    int failures = 0;

    for (auto test : tests) {
        auto left = Integer::fromString(test.left);
        auto right = Integer::fromString(test.right);
        int comparison = Integer::fromString(test.left).compare(Integer::fromString(test.right));

        if (outputTestResult(test, "<", left.compare(right) < 0, test.lessThan)) {
            success++;
        } else {
            failures++;
        }
        if (outputTestResult(test, ">", left.compare(right) > 0, test.greaterThan)) {
            success++;
        } else {
            failures++;
        }
        if (outputTestResult(test, "==", left.compare(right) == 0, test.equal)) {
            success++;
        } else {
            failures++;
        }
    }

    cout << "successes = " << success << ", failures = " << failures << endl;

    cout << "Integer::fromString(100).toInt() = " << (Integer::fromString("100").toInt()) << endl;
    cout << "Integer::fromString(-51234).toInt() = " << (Integer::fromString("-51234").toInt()) << endl;
    cout << "Integer::fromString(51234).toInt() = " << (Integer::fromString("51234").toInt()) << endl;
    cout << "Integer::fromString(0).toInt() = " << (Integer::fromString("0").toInt()) << endl;

    Solution sol;
    cout << "sol.myAtoi(100) = " << (sol.myAtoi("100")) << endl;
    cout << "sol.myAtoi(-51234) = " << (sol.myAtoi("-51234")) << endl;
    cout << "sol.myAtoi(51234) = " << (sol.myAtoi("51234")) << endl;
    cout << "sol.myAtoi(0) = " << (sol.myAtoi("0")) << endl;
    cout << "sol.myAtoi(-2147483647) = " << (sol.myAtoi("-2147483647")) << endl;
    cout << "sol.myAtoi(-2147483648) = " << (sol.myAtoi("-2147483648")) << endl;
    cout << "sol.myAtoi(-2147483649) = " << (sol.myAtoi("-2147483649")) << endl;
    cout << "sol.myAtoi(-2147483650) = " << (sol.myAtoi("-2147483650")) << endl;
    cout << "sol.myAtoi(2147483647) = " << (sol.myAtoi("2147483647")) << endl;
    cout << "sol.myAtoi(2147483648) = " << (sol.myAtoi("2147483648")) << endl;
    cout << "sol.myAtoi(2147483649) = " << (sol.myAtoi("2147483649")) << endl;
    cout << "sol.myAtoi(2147483650) = " << (sol.myAtoi("2147483650")) << endl;
    cout << "sol.myAtoi(0-1) = " << (sol.myAtoi("0-1")) << endl;
    cout << "sol.myAtoi(+-12) = " << (sol.myAtoi("+-12")) << endl;
    cout << "sol.myAtoi(-91283472332) = " << (sol.myAtoi("-91283472332")) << endl;
}