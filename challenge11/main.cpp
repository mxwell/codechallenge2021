#include <bits/stdc++.h>

using namespace std;

template<class T> inline int len(const T &c) {
  return static_cast<int>(c.size());
}

typedef pair<int, int> pt;

vector<string> funcs;
map<pt, int> cache;

int GetCommonPrefix(int x, int y) {
    if (x > y)
        swap(x, y);
    pt key(x, y);
    auto iter = cache.find(key);
    if (iter != cache.end()) {
        return iter->second;
    }
    int prefix = 0;
    int m = len(funcs[x]);
    for (int i = 0; i < m; ++i) {
        if (i >= len(funcs[y]) || funcs[x][i] != funcs[y][i])
            break;
        ++prefix;
    }
    cache[key] = prefix;
    return prefix;
}

void HandleCase() {
    int n, k;
    scanf("%d%d\n", &n, &k);
    char buf[128];
    funcs.clear();
    cache.clear();
    for (int i = 0; i < n; ++i) {
        scanf("%s\n", buf);
        funcs.push_back(string(buf));
    }
    sort(funcs.begin(), funcs.end());
    vector<bool> used(n, false);
    int ans = 0;
    int left = n;
    while (left > 0) {
        int lf = 0;
        while (lf < n && used[lf])
            ++lf;
        assert(lf < n);
        int rg = lf;
        int covered = 1;
        while (covered < k && rg + 1 < n) {
            if (!used[rg + 1]) {
                ++covered;
            }
            ++rg;
        }
        assert(covered == k);
        int bestLf = lf;
        int bestRg = rg;
        assert(!used[lf]);
        assert(!used[rg]);
        int bestScore = GetCommonPrefix(lf, rg);
        while (rg < n) {
            ++lf;
            --covered;
            while (lf < n && used[lf])
                ++lf;
            if (lf >= n)
                break;
            while (covered < k && rg + 1 < n) {
                if (!used[rg + 1]) {
                    ++covered;
                }
                ++rg;
            }
            if (covered < k || rg < lf)
                break;
            if (rg > n)
                break;
            assert(!used[lf]);
            assert(!used[rg]);
            int score = GetCommonPrefix(lf, rg);
            if (score > bestScore) {
                bestLf = lf;
                bestRg = rg;
                bestScore = score;
            }
        }
        ans += bestScore;
        int marked = 0;
        for (int i = bestLf; i <= bestRg; ++i) {
            if (!used[i]) {
                ++marked;
                used[i] = true;
            }
        }
        assert(marked == k);
        left -= marked;
    }
    printf("%d\n", ans);
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
