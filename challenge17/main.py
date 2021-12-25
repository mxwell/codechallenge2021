#! /usr/bin/env python3

import logging
import sys


def get_g(x):
    return x % 3


def verify():
    g = [0]
    for i in range(1, 10000):
        used = set()
        for j in range(i):
            p2 = 2 ** j
            if p2 > i:
                break
            used.add(g[i - p2])
        for t in range(i + 1):
            if t not in used:
                g.append(t)
                break
    
    for i, value in enumerate(g):
        assert value == get_g(i), "unexpected: {} instead of {}".format(value, get_g(i))


def handle_case():
    m = int(input().strip())
    parts = input().strip().split()
    vs = [int(part) for part in parts]

    xor = 0
    for v in vs:
        xor = xor ^ get_g(v)
    if xor > 0:
        print("Edu")
    else:
        print("Alberto")


def main():
    verify()
    n = int(input().strip())
    for case in range(1, n + 1):
        print("Case #{}: ".format(case), end="")
        handle_case()
    return 0


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    sys.exit(main())
