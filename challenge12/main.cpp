#include <bits/stdc++.h>

using namespace std;

template<class T> inline int len(const T &c) {
  return static_cast<int>(c.size());
}

typedef long long li;
typedef pair<int, li> pt;

const int N = 20010;
const string BTC = "BTC";
const li INF = 1000000000;

int currencies;
map<string, int> currency_ids;
vector<pt> g[N];
li amount[N];

void Init() {
    currency_ids.clear();
    for (int i = 0; i < currencies; ++i) {
        g[i].clear();
        amount[i] = 0;
    }
    currencies = 0;
}

inline
int GetCurrencyId(const string& s) {
    if (!currency_ids.count(s)) {
        int new_id = currencies++;
        currency_ids.emplace(s, new_id);
        return new_id;
    } else {
        return currency_ids[s];
    }
}

void HandleCase() {
    Init();

    int m;
    scanf("%d\n", &m);
    char buf[128];

    for (int i = 0; i < m; ++i) {
        int k;
        scanf("%s%d\n", buf, &k);
        for (int j = 0; j < k; ++j) {
            scanf("%s", buf);
            string line(buf);
            size_t d1 = line.find('-');
            assert(d1 != string::npos);
            size_t d2 = line.find('-', d1 + 1);
            assert(d2 != string::npos);
            int rate = stoi(line.substr(d1 + 1, d2 - d1 - 1));
            if (rate < 0)
                continue;
            int src = GetCurrencyId(line.substr(0, d1));
            int dst = GetCurrencyId(line.substr(d2 + 1));
            g[src].emplace_back(dst, rate);
        }
    }

    int btc = GetCurrencyId(BTC);
    amount[btc] = 1;
    queue<pt> q;
    q.push({btc, 1});
    int it = 0;
    for (; amount[btc] == 1 && it <= currencies * 5 && !q.empty(); ++it) {
        queue<pt> nextQ;
        while (!q.empty()) {
            pt p = q.front();
            int fm = p.first;
            li value = p.second;
            q.pop();
            for (const auto& edge : g[fm]) {
                int to = edge.first;
                li rate = edge.second;
                li nextValue = value * rate;
                if (nextValue > INF)  // might be loop
                    continue;
                if (amount[to] < nextValue) {
                    amount[to] = nextValue;
                    nextQ.push({to, nextValue});
                }
            }
        }
        swap(q, nextQ);
    }
    fprintf(stderr, "%d iterations, %d currencies, %d markets, amount %lld\n", it, currencies, m, amount[btc]);
    printf("%lld\n", amount[btc]);
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
