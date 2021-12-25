import sys


def main():
    n = int(input().strip())
    for case in range(1, n + 1):
        scores = input().strip().split(":")
        score = sum([int(s) for s in scores])
        if score == 12:
            result = "-"
        else:
            result = str(score + 1)
        print("Case #{}: {}".format(case, result))        
    return 0


if __name__ == "__main__":
    sys.exit(main())