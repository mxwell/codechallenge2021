#include <bits/stdc++.h>

using namespace std;

template<class T> inline int len(const T &c) {
  return static_cast<int>(c.size());
}

typedef long long li;

struct Token {
    bool Operation;
    bool Number;
    char Sign;
    string Chars;
    li Value;

    string ToString() const {
        string result;
        if (Operation) {
            result.push_back(Sign);
        } else if (Number) {
            result += to_string(Value);
        } else {
            for (char c : Chars) {
                result.push_back(c + 'A');
            }
        }
        return result;
    }
};

vector<string> Split(const string& s, const char sep) {
    vector<string> result;
    string cur;
    for (char c : s) {
        if (c == sep) {
            if (!cur.empty()) {
                result.push_back(cur);
                cur.clear();
            }
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) {
        result.push_back(cur);
    }
    return result;
}

bool IsAb(char c) {
    return 'A' <= c && c <= 'Z';
}

bool IsDigit(char c) {
    return '0' <= c && c <= '9';
}

bool IsOp(const string& s) {
    return (
        s == "+" ||
        s == "-" ||
        s == "*" ||
        s == "/"
    );
}

int GetPriority(char op) {
    if (op == '*' || op == '/') {
        return 2;
    }
    return 1;
}

template<class T>
void PopPush(vector<T>& src, vector<T>& dst) {
    dst.push_back(src.back());
    src.pop_back();
}

vector<Token> Parse(const vector<string>& parts) {
    vector<Token> result;
    vector<Token> ops;
    for (const auto& p : parts) {
        if (IsOp(p)) {
            Token op{
                .Operation = true,
                .Number = false,
                .Sign = p[0],
                .Chars = "",
                .Value = 0
            };
            while (!ops.empty() && GetPriority(ops.back().Sign) >= GetPriority(op.Sign)) {
                PopPush(ops, result);
            }
            ops.push_back(op);
        } else if (IsAb(p[0])) {
            string shifted;
            for (char c : p) {
                assert(IsAb(c));
                shifted.push_back(c - 'A');
            }
            result.push_back(Token{
                .Operation = false,
                .Number = false,
                .Sign = ' ',
                .Chars = shifted,
                .Value = 0
            });
        } else {
            li value = stoll(p);
            result.push_back(Token{
                .Operation = false,
                .Number = true,
                .Sign = ' ',
                .Chars = "",
                .Value = value                
            });
        }
    }
    while (!ops.empty()) {
        PopPush(ops, result);
    }
    return result;
}

string wholeExpr;
vector<Token> leftExpr;
vector<Token> rightExpr;
vector<bool> used;
string ab;
vector<int> numByChar;
vector<string> answers;

bool Substitute(const string& s, li* substResult) {
    if (len(s) > 1) {
        if (numByChar[s[0]] == 0) {
            return false;
        }
    }
    li res = 0;
    for (char c : s) {
        li digit = numByChar[c];
        res = res * 10 + digit;
    }
    *substResult = res;
    return true;
}

string Repr() {
    string result;
    for (char c : wholeExpr) {
        if (IsAb(c)) {
            result.push_back('0' + numByChar[c - 'A']);
        } else {
            result.push_back(c);
        }
    }
    return result;
}

bool Eval(const vector<Token>& expr, li* exprResult) {
    vector<li> st;
    for (const Token& tok : expr) {
        if (tok.Operation) {
            li right = st.back();
            st.pop_back();
            li left = st.back();
            st.pop_back();
            li res;
            switch (tok.Sign) {
                case '+':
                    res = left + right;
                    break;
                case '-':
                    res = left - right;
                    break;
                case '*':
                    res = left * right;
                    break;
                case '/': {
                    if (right == 0 || left % right != 0) {
                        return false;
                    }
                    res = left / right;
                    break;
                }
                default:
                    throw 1;                   
            }
            st.push_back(res);
        } else if (tok.Number) {
            st.push_back(tok.Value);
        } else {
            li value;
            if (!Substitute(tok.Chars, &value)) {
                return false;
            }
            st.push_back(value);
        }
    }
    assert(len(st) == 1);
    *exprResult = st.back();
    return true;
}

void Recursive(int k, int n) {
    if (k >= n) {
        li leftVal;
        li rightVal;
        if (Eval(leftExpr, &leftVal) && Eval(rightExpr, &rightVal) && leftVal == rightVal) {
            answers.push_back(Repr());
        }
        return;
    }
    for (int i = 0; i < 10; ++i) {
        if (!used[i]) {
            used[i] = true;
            numByChar[ab[k]] = i;
            Recursive(k + 1, n);
            used[i] = false;
        }
    }
}

void Init() {
    used.assign(10, false);
    ab.clear();
    numByChar.assign(26, -1);
    answers.clear();
}

void HandleCase() {
    Init();

    getline(cin, wholeExpr);
    
    for (char c : wholeExpr) {
        if (IsAb(c)) {
            ab.push_back(c - 'A');
        }
    }
    sort(ab.begin(), ab.end());
    ab.erase(unique(ab.begin(), ab.end()), ab.end());
    size_t equalPos = wholeExpr.find(" = ");
    assert(equalPos != string::npos);
    auto leftParts = Split(wholeExpr.substr(0, equalPos), ' ');
    auto rightParts = Split(wholeExpr.substr(equalPos + 3), ' ');
    leftExpr = Parse(leftParts);
    rightExpr = Parse(rightParts);

    Recursive(0, len(ab));

    if (answers.empty()) {
        printf("IMPOSSIBLE\n");
    } else {
        sort(answers.begin(), answers.end());
        bool first = true;
        for (const auto& ans : answers) {
            if (first) {
                first = false;
            } else {
                printf(";");
            }
            printf("%s", ans.c_str());
        }
        printf("\n");
    }
}

int main() {
    int tests;
    scanf("%d\n", &tests);
    for (int test = 1; test <= tests; ++test) {
        printf("Case #%d: ", test);
        HandleCase();
    }
    return 0;
}
