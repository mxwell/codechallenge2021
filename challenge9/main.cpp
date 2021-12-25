#include <bits/stdc++.h>

using namespace std;

template<class T> inline int len(const T &c) {
  return static_cast<int>(c.size());
}

typedef long long li;
typedef pair<int, int> pt;

struct Definition {
    int Width;
    int Height;
    vector<string> V;
};

struct Intersection {
    int MaxShiftDown;
    int MinShiftHor;
    int MaxShiftHor;
    vector<vector<bool>> V;
};

struct Instance {
    int Id;
    pt Pos;
};

vector<Definition> definitions;
vector<vector<Intersection>> intersections;

void ReadDefinitions() {
    int d;
    scanf("%d", &d);
    char buf[1024];
    for (int mask = 0; mask < d; ++mask) {
        Definition def;
        scanf("%d%d", &def.Width, &def.Height);
        for (int i = 0; i < def.Height; ++i) {
            scanf("%s\n", buf);
            def.V.push_back(string(buf));
            assert(len(def.V.back()) == def.Width);
        }
        definitions.emplace_back(std::move(def));
    }
}

/* pt: first - vertical, second - horizontal coordinate */
bool CheckIntersection(int m1, int m2, const pt& pos1, const pt& pos2) {
    if (pos1.first > pos2.first) {
        return CheckIntersection(m2, m1, pos2, pos1);
    }
    const Intersection& intersection = intersections[m1][m2];
    int down = pos2.first - pos1.first;
    if (down > intersection.MaxShiftDown) {
        return false;
    }
    int hor = pos2.second - pos1.second;
    if (hor < intersection.MinShiftHor || hor > intersection.MaxShiftHor) {
        return false;
    }
    int j = hor - intersection.MinShiftHor;
    return intersection.V[down][j];
}

Intersection GetIntersection(const Definition& first, const Definition& second) {
    Intersection result;
    result.MaxShiftDown = first.Height - 1;
    result.MinShiftHor = -second.Width + 1;
    result.MaxShiftHor = first.Width - 1;
    result.V.resize(result.MaxShiftDown + 1);
    for (int down = 0; down <= result.MaxShiftDown; ++down) {
        for (int hor = result.MinShiftHor; hor <= result.MaxShiftHor; ++hor) {

            bool collide = false;
            for (int i = 0; !collide && i < second.Height; ++i) {
                for (int j = 0; j < second.Width; ++j) {
                    if (second.V[i][j] == '1') {
                        int x = i + down;
                        int y = j + hor;
                        if (x < first.Height && y >= 0 && y < first.Width && first.V[x][y] == '1') {
                            collide = true;
                            break;
                        }
                    }
                }
            }

            result.V[down].push_back(collide);
        }
    }
    return result;
}

void PreCalc() {
    int d = len(definitions);
    intersections.resize(d);
    for (int i = 0; i < d; ++i) {
        for (int j = 0; j < d; ++j) {
            intersections[i].emplace_back(GetIntersection(definitions[i], definitions[j]));
        }
    }
}

void HandleCase() {
    int p;
    scanf("%d", &p);
    vector<Instance> sprites;
    for (int i = 0; i < p; ++i) {
        int a, x, y;
        scanf("%d%d%d", &a, &x, &y);
        sprites.push_back({
            .Id = a,
            .Pos = {y, x}
        });
    }
    li ans = 0;
    for (int i = 0; i < p; ++i) {
        auto& first = sprites[i];
        for (int j = 0; j < i; ++j) {
            auto& second = sprites[j];
            if (CheckIntersection(first.Id, second.Id, first.Pos, second.Pos)) {
                ++ans;
            }
        }
    }
    printf("%lld\n", ans);
}

int main() {
  int tests;
  scanf("%d", &tests);
  ReadDefinitions();
  PreCalc();
  for (int test = 1; test <= tests; ++test) {
    printf("Case #%d: ", test);
    HandleCase();
  }
  return 0;
}
