#include <bits/stdc++.h>

using namespace std;

template<class T> inline int len(const T &c) {
  return static_cast<int>(c.size());
}

const int N = 1510;

vector<string> cities;
map<string, int> city_ids;
vector<int> g[N];
bool used[N];
int timer;
int tin[N];
int fup[N];
bool cut_point[N];

void Init() {
    int n = len(cities);

    cities.clear();
    city_ids.clear();
    for (int i = 0; i < n; ++i) {
        g[i].clear();
        used[i] = false;
        cut_point[i] = false;
    }
    timer = 0;
}

inline
int GetCityId(const string& s) {
    if (!city_ids.count(s)) {
        auto new_id = len(cities);
        city_ids.emplace(s, new_id);
        cities.push_back(s);
        return new_id;
    } else {
        return city_ids[s];
    }
}

void Dfs(int v, int p) {
    used[v] = true;
    tin[v] = fup[v] = timer++;
    int children = 0;
    for (int to : g[v]) {
        assert(to != v);
        if (to == p) {
            continue;
        }
        if (used[to]) {
            fup[v] = min(fup[v], tin[to]);
        } else {
            Dfs(to, v);
            fup[v] = min(fup[v], fup[to]);
            if (fup[to] >= tin[v] && p != -1) {
                cut_point[v] = true;
            }
            ++children;
        }
    }
    if (p == -1 && children > 1) {
        cut_point[v] = true;
    }
}

void HandleCase() {
    Init();

    int m;
    scanf("%d\n", &m);
    char buf[128];
    for (int i = 0; i < m; ++i) {
        scanf("%s\n", buf);
        string line(buf);
        auto comma = line.find(",");
        assert(comma != string::npos);
        string src = line.substr(0, comma);
        string dst = line.substr(comma + 1);
        int s = GetCityId(src);
        int d = GetCityId(dst);
        g[s].push_back(d);
        g[d].push_back(s);
    }

    Dfs(0, -1);

    vector<string> result;
    for (int i = 0; i < len(cities); ++i) {
        if (cut_point[i]) {
            result.push_back(cities[i]);
        }
    }
    sort(result.begin(), result.end());
    if (result.empty()) {
        printf("-\n");
    } else {
        bool first = true;
        for (const auto& name : result) {
            if (first) {
                first = false;
            } else {
                printf(",");
            }
            printf("%s", name.c_str());
        }
        printf("\n");
    }
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
