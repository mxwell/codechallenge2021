#include <bits/stdc++.h>

using namespace std;

template<class T> inline int len(const T &c) {
  return static_cast<int>(c.size());
}

vector<vector<string>> kNotes = {
    {"A"},
    {"A#", "Bb"},
    {"B", "Cb"},
    {"C", "B#"},
    {"C#", "Db"},
    {"D"},
    {"D#", "Eb"},
    {"E"},
    {"F", "E#"},
    {"F#", "Gb"},
    {"G"},
    {"G#", "Ab"}
};

int GetInitial(const string& name) {
    for (int i = 0; i < len(kNotes); ++i) {
        for (const auto& s : kNotes[i]) {
            if (s == name) {
                return i;
            }
        }
    }
    throw 1;
}

bool Check(const vector<int>& positions, int mask) {
    int countMask = 0;
    for (int i = 0; i < len(positions); ++i) {
        /* if b == 0 then we choose the first note name, otherwise - the second name */
        int b = (mask & (1 << i)) ? 1 : 0;
        int pos = positions[i];
        /* if we choose the second name, then check if it exists */
        if (b && len(kNotes[pos]) == 1) {
            return false;
        }
        int offset = kNotes[pos][b][0] - 'A';
        int charMask = 1 << offset;
        /* check if we already got this letter */
        if (countMask & charMask) {
            return false;
        }
        countMask |= charMask;
    }
    /* check that we got all the letters A..G */
    return countMask == ((1 << 7) - 1);
}

void HandleCase() {
    char buf[128];
    scanf("%s\n", buf);
    string initial(buf);
    scanf("%s\n", buf);
    string steps(buf);

    int pos = GetInitial(initial);
    int initialName = 0;
    if (kNotes[pos][initialName] != initial) {
        ++initialName;
    }
    vector<int> positions;
    positions.push_back(pos);
    for (char step : steps) {
        int incr = step == 'T' ? 2 : 1;
        pos = (pos + incr) % len(kNotes);
        if (len(positions) < 7) {
            positions.push_back(pos);
        }
    }
    vector<string> results;
    for (int mask = (1 << 7) - 1; mask >= 0; --mask) {
        if ((mask & 1) == initialName && Check(positions, mask)) {
            string result;
            string first;
            for (int i = 0; i < len(positions); ++i) {
                int b = (mask & (1 << i)) ? 1 : 0;
                const auto& note = kNotes[positions[i]][b];
                result += note;
                if (first.empty()) {
                    first = note;
                }
            }
            result += first;
            results.emplace_back(result);
        }
    }
    assert(!results.empty());
    sort(results.begin(), results.end());
    printf("%s\n", results[0].c_str());
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
