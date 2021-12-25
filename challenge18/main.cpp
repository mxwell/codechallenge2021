#include <bits/stdc++.h>

using namespace std;

template<class T> inline int len(const T &c) {
    return static_cast<int>(c.size());
}

void HandleCase() {
    int m;
    scanf("%d\n", &m);
    char buf[128];
    map<string, int> freq_by_name;
    for (int i = 0; i < m; ++i) {
        scanf("%s", buf);
        freq_by_name[string(buf)]++;
        scanf("%s", buf);
        string arg(buf);
        assert(arg == "UP" || arg == "DOWN" || arg == "RIGHT" || arg == "LEFT");
    }
    vector<int> freqs;
    for (const auto& it : freq_by_name) {
        freqs.push_back(it.second);
    }
    sort(freqs.rbegin(), freqs.rend());
    int k = len(freqs);
    if (k == 1) {
        int size = 3 * m;
        int diff = 0;
        printf("%d, %d\n", size, diff);
        return;
    }
    int counter = 0;
    // freq, height, score, min_leaf_dist, max_leaf_dist, counter
    using Node = tuple<int, int, int, int, int, int>;
    set<Node> heap;
    for (int f : freqs) {
        heap.insert(Node(f, 0, 0, 0, 0, counter++));
    }

#define GET_FREQ(p) get<0>(p)
#define GET_HEIGHT(p) get<1>(p)
#define GET_SCORE(p) get<2>(p)
#define GET_MIN_LEAF(p) get<3>(p)
#define GET_MAX_LEAF(p) get<4>(p)

    while (len(heap) > 1) {
        Node a = *heap.begin();
        heap.erase(heap.begin());
        Node b = *heap.begin();
        heap.erase(heap.begin());
        heap.insert(Node(
            GET_FREQ(a) + GET_FREQ(b),
            1 + max(GET_HEIGHT(a), GET_HEIGHT(b)),
            GET_SCORE(a) + GET_SCORE(b) + GET_FREQ(a) + GET_FREQ(b),
            1 + min(GET_MIN_LEAF(a), GET_MIN_LEAF(b)),
            1 + max(GET_MAX_LEAF(a), GET_MAX_LEAF(b)),
            counter++
        ));
    }
    Node a = *heap.begin();
    printf("%d, %d\n", GET_SCORE(a) + m * 2, GET_MAX_LEAF(a) - GET_MIN_LEAF(a));
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
