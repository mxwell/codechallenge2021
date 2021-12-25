#include <bits/stdc++.h>

using namespace std;

const char ALPHABET_START = 'A';
const int ALPHABET = 26;
const int NO_EDGE = -1;

struct TNode {
	int StringId;
    int Parent;
    int ParentCh;
    int Link;
    int Next[ALPHABET];
    int Go[ALPHABET];
};

vector<TNode> nodes;

void InitNode(size_t id) {
    auto& node = nodes[id];
    node.StringId = NO_EDGE;
    node.Parent = NO_EDGE;
    node.Link = NO_EDGE;
    for (size_t i = 0; i < ALPHABET; ++i) {
        node.Next[i] = NO_EDGE;
        node.Go[i] = NO_EDGE;
    }
}

void Init() {
    nodes.clear();
    nodes.emplace_back(TNode());
    InitNode(0);
}

void AddString(const string& s, int stringId) {
    int v = 0;
    for (char cOffset : s) {
        int c = cOffset - ALPHABET_START;
        if (nodes[v].Next[c] == NO_EDGE) {
            auto newId = nodes.size();
            nodes.emplace_back(TNode());
            InitNode(newId);
            nodes[v].Next[c] = newId;
            auto& node = nodes[newId];
            node.Parent = v;
            node.ParentCh = c;
        }
        v = nodes[v].Next[c];
    }
    nodes[v].StringId = stringId;
}

int Go(int v, int c);

int GetLink(int v) {
    if (nodes[v].Link == NO_EDGE) {
        if (v == 0 || nodes[v].Parent == 0) {
            nodes[v].Link = 0;
        } else {
            nodes[v].Link = Go(GetLink(nodes[v].Parent), nodes[v].ParentCh);
        }
    }
    return nodes[v].Link;
}

int Go(int v, int c) {
    if (nodes[v].Go[c] == NO_EDGE) {
        if (nodes[v].Next[c] != NO_EDGE) {
            nodes[v].Go[c] = nodes[v].Next[c]; 
        } else {
            nodes[v].Go[c] = (v == 0) ? 0 : Go(GetLink(v), c);
        }
    }
    return nodes[v].Go[c];
}

void AddStrings(const vector<string>& strings) {
    Init();
    for (int i = 0; i < (int) strings.size(); ++i) {
        AddString(strings[i], i);
    }
}

bool FindString(const string& text, const vector<bool>& found, bool reversed, int* endPos, int* foundId) {
    int v = 0;
    int n = (int) text.size();
    int start = reversed ? (n - 1) : 0;
    int end = reversed ? -1 : n;
    int step = reversed ? -1 : 1;
    for (int i = start; i != end; i += step) {
        int c = text[i] - ALPHABET_START;
        int go = Go(v, c);
        int stringId = nodes[go].StringId;
        if (stringId != NO_EDGE && !found[stringId]) {
            *endPos = i;
            *foundId = stringId;
            return true;
        }
        v = go;
    }
    return false;
}

void HandleCase() {
    int p, r, c;
    scanf("%d%d%d\n", &p, &r, &c);
    vector<string> strings;
    char buf[110];
    for (int i = 0; i < p; ++i) {
        scanf("%s\n", buf);
        strings.emplace_back(string(buf));
    }
    AddStrings(strings);
    string text;
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            scanf("%s", buf);
            assert(strlen(buf) == 1);
            char c = buf[0];
            assert('A' <= c && c <= 'Z');
            text.push_back(c);
        }
    }
    assert(int(text.size()) == r * c);
    cerr << "orig text: " << text << endl;
    
    vector<bool> found(p, false);
    int endPos;
    int foundId;
    for (int attempt = 0; attempt < p; ++attempt) {
        if (FindString(text, found, false, &endPos, &foundId)) {
            found[foundId] = true;
            const string& found = strings[foundId];
            int startPos = endPos - int(found.size()) + 1;
            cerr << "pokemon " << found << " found at " << startPos << "," << endPos << endl;
            text = text.substr(0, startPos) + text.substr(endPos + 1);
            cerr << "new text: " << text << endl;
        } else if (FindString(text, found, true, &endPos, &foundId)) {
            found[foundId] = true;
            const string& found = strings[foundId];
            int startPos = endPos + int(found.size()) - 1;
            cerr << "pokemon " << found << " found at " << startPos << "," << endPos << endl;
            text = text.substr(0, endPos) + text.substr(startPos + 1);
            cerr << "new text: " << text << endl;
        } else {
            throw 1;
        }
    }
    printf("%s\n", text.c_str());
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