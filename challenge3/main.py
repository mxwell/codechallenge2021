import logging
import sys


def gcd(x, y):
    while x != 0:
        y = y % x
        x, y = y, x
    return y


class Weight(object):

    def __init__(self, numerator, denominator=1):
        self.numerator = numerator
        self.denominator = denominator
        self.norm()
    
    def norm(self):
        g = gcd(self.numerator, self.denominator)
        self.numerator //= g
        self.denominator //= g
    
    def __str__(self):
        return "{} / {}".format(self.numerator, self.denominator)

    def __add__(self, other):
        assert isinstance(other, Weight)
        n = self.numerator * other.denominator + other.numerator * self.denominator
        d = self.denominator * other.denominator
        return Weight(n, d)
    
    def __gt__(self, other):
        assert isinstance(other, Weight)
        n = self.numerator * other.denominator
        other_n = other.numerator * self.denominator
        return n > other_n


def parse_weight(s):
    if "/" in s:
        n, d = s.split("/")
        return Weight(int(n), int(d))
    return Weight(int(s))


def parse_weights(weights_str):
    logging.info("weights string: %s", weights_str)
    result = dict()
    cur_key = ""
    in_key = False
    cur_value = ""
    in_value = False
    for c in " {} ".format(weights_str):
        looks_key = c.isalpha()
        looks_value = c.isdigit() or c == "/"
        if looks_key:
            cur_key += c
            in_key = True
        elif looks_value:
            cur_value += c
            in_value = True
        elif in_key:
            assert len(cur_key) > 0
            in_key = False
        elif in_value:
            assert len(cur_value) > 0
            assert len(cur_key) > 0
            assert not in_key
            value = parse_weight(cur_value)
            result[cur_key] = value
            logging.info("Added pair %s => %s", cur_key, str(value))
            in_value = False
            cur_key = ""
            cur_value = ""
    return result


def word_score(word, weights):
    result = Weight(0)
    for c in word:
        if c in weights:
            result += weights[c]
    return result


def handle_case():
    task = input().strip()
    words_str, weights_str = task.split("|")
    words = words_str.split("-")
    assert len(words) == 2
    weights = parse_weights(weights_str)
    scores = [word_score(w, weights) for w in words]
    for i in range(len(words)):
        logging.info("word %s, score %s", words[i], str(scores[i]))
    if scores[0] > scores[1]:
        print(words[0])
    elif scores[1] > scores[0]:
        print(words[1])
    else:
        print("-")
    

def main():
    n = int(input().strip())
    for case in range(1, n + 1):
        print("Case #{}: ".format(case), end="")
        handle_case()
    return 0


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    sys.exit(main())