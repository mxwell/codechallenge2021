#include <bits/stdc++.h>

using namespace std;

typedef long long li;

const li MOD = 100000007;

void HandleCase() {
    li n;
    scanf("%lld", &n);
    if (n >= MOD) {
        printf("0\n");
        return;
    }
    li ans = 1;
    for (li i = 2; i <= n; ) {
        li t = i % 100;
        if (t / 10 == 2) {
            i += 10; // ..20 -> ..30
            continue;
        }
        if (i >= 2 * 10000) {
            t = i % 100000;
            if (t / 10000 == 2) {
                assert(t % 10000 == 0);
                i += 10000;
                continue;
            }
        }
        if (i >= 2 * 10000000) {
            t = i % 100000000;
            if (t / 10000000 == 2) {
                assert(t % 10000000 == 0);
                i += 10000000;
                continue;
            }
        }
        ans = (ans * i) % MOD;
        ++i;
    }
    printf("%lld\n", ans);    
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
