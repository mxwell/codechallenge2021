#include <bits/stdc++.h>

using namespace std;

template<class T> inline int len(const T &c) {
    return static_cast<int>(c.size());
}

int FromHex(char c) {
    if (c <= '9') {
        return c - '0';
    }
    return c - 'a' + 10;
}

char ToHex(int c) {
    if (c <= 9) {
        return '0' + c;
    }
    return 'a' + c - 10;
}

string Printable(const string& a) {
    string result;
    for (char c : a) {
        result.push_back(c + '0');
    }
    return result;
}

string ToBitString(const string& hex) {
    string result;
    for (char c : hex) {
        int fromHex = FromHex(c);
        for (int i = 3; i >= 0; --i) {
            if (fromHex & (1 << i)) {
                result.push_back(1);
            } else if (!result.empty()) {
                result.push_back(0);
            }
        }
    }
    return result;
}

string FromBitString(string a) {
    reverse(a.begin(), a.end());
    while (len(a) % 4) {
        a.push_back(0);
    }
    string result;
    for (int i = 0; i < len(a); i += 4) {
        int num = 0;
        for (int j = 0; j < 4; ++j) {
            if (a[i + j]) {
                num |= 1 << j;
            }
        }
        result.push_back(ToHex(num));
    }
    reverse(result.begin(), result.end());
    return result;
}

int GetDegree(const string& a) {
    for (int i = 0; i < len(a); ++i) {
        if (a[i] > 0) {
            return len(a) - i - 1;
        }
    }
    return 0;
}

string GetMod(string a, const string& b) {
    assert(GetDegree(a) >= GetDegree(b));
    size_t offset = a.find(1);
    while (offset != string::npos) {
        if (int(offset) + len(b) > len(a)) {
            return a.substr(offset);
        }
        for (int i = 0; i < len(b); ++i) {
            a[offset + i] ^= b[i];
        }
        assert(a[offset] != 1);
        offset = a.find(1, offset);
    }
    return string();
}

string GetGcd(string a, string b) {
    while (!b.empty()) {
        if (GetDegree(a) < GetDegree(b)) {
            swap(a, b);
            continue;
        }
        string remainder = GetMod(a, b);
        cerr << Printable(a) << " MOD " << Printable(b) << " => " << Printable(remainder) << endl;
        a = remainder;
        swap(a, b);
    }
    return a;
}

string DropTrailingZeros(string a) {
    while (a.back() == 0) {
        a.pop_back();
    }
    return a;
}

void HandleCase() {
    int n;
    scanf("%d", &n);
    vector<string> inferences;
    char buf[128];
    for (int i = 0; i < n; ++i) {
        scanf("%s", buf);
        inferences.push_back(ToBitString(string(buf)));
        cerr << "inference #" << i << ": " << Printable(inferences.back()) << endl;
    }
    string g = inferences[0];
    for (int i = 1; i < n; ++i) {
        string nextG = GetGcd(g, inferences[i]);
        cerr << "GCD(" << Printable(g) << ", " << Printable(inferences[i]) << ") => " << Printable(nextG) << endl;
        g = nextG;
    }
    g = DropTrailingZeros(g);
    assert(!g.empty());
    printf("%s\n", FromBitString(g).c_str());
}

int main() {
    int tests;
    scanf("%d", &tests);
    for (int test = 1; test <= tests; ++test) {
        printf("Case #%d: ", test);
        HandleCase();
    }
    return 0;
}
