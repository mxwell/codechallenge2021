#! /usr/bin/env python3

"""
Sample session:

> 8 15
< ? 1 2
> 4
..
< ! 4 5 6 7 8
> <password>

"""

import logging
import random
import sys
import telnetlib

TN_HOST = "codechallenge-daemons.0x14.net"
TN_PORT = 7162


def dump_password(p):
    with open("output.txt", "w") as a_file:
        a_file.write(p)


def get_primes():

    def is_prime(n):
        if n <= 1:
            return False
        if n <= 3:
            return True
        if n % 2 == 0:
            return False
        i = 3
        while i * i <= n:
            if n % i == 0:
                return False
            i += 1
        return True

    result = []    
    for i in range(1, 101):
        if is_prime(i):
            result.append(i)
    
    logging.info("Got primes: %s", str(result))
    return result


def calc_gcd(x, y):
    while x != 0:
        y = y % x
        x, y = y, x
    return y


def get_stats(n):
    stats = dict()
    for i in range(1, 101):
        if i != n:
            g = calc_gcd(n, i)
            stats[g] = 1 + stats.get(g, 0)
    return stats


class Solution(object):

    def __init__(self, tn):
        self.tn = tn
        self.n = None
        self.q = None
        self.cache = dict()
        # dict(prime -> set(multiples..))
        self.position_by_div = dict()
        # permutation position -> dict(prime -> degree)
        self.factorizations = dict()
        self.requests_count = 0
        self.primes = get_primes()
        self.have_under20 = 0
        # permutation position -> number of "1" responses
        self.ones = dict()
        self.scanned = set()
    
    def read_invite(self):
        response = self.tn.read_until(b"\n").decode("ascii").strip()
        logging.info("Received %s", response)
        nums = [int(x) for x in response.split()]
        self.n = nums[0]
        self.q = nums[1] - 2
        self.primes = [p for p in self.primes if p <= self.n]
    
    def request(self, message):
        assert not message.endswith("\n")
        self.tn.write(message.encode("ascii") + b"\n")
        response = self.tn.read_until(b"\n").decode("ascii").strip()
        logging.info("Requested: %s => %s", message, response)
        self.requests_count += 1
        return response
    
    def update_factorization(self, pos, prime, degree):
        if pos not in self.factorizations:
            self.factorizations[pos] = {prime: degree}
        else:
            self.factorizations[pos][prime] = max(self.factorizations[pos].get(prime, 0), degree)
    
    def get_div_count(self, pos):
        if pos not in self.factorizations:
            return 0
        return sum(self.factorizations[pos].values())
    
    def check_div(self, pos, prime):
        if pos not in self.factorizations:
            return False
        return self.factorizations[pos].get(prime, 0) > 0
    
    def get_gcd(self, x, y):
        if x == y:
            logging.error("Invalid request: both positions are equal to %d", x)
            return
        u, v = x, y
        if x > y:
            x, y = y, x
        key = (x, y)
        if key in self.cache:
            return self.cache[key]
        message = "? {} {}".format(u, v)
        response = self.request(message)
        result = int(response)
        if result > 1:
            for p in self.primes:
                if p > result:
                    break
                if result % p != 0:
                    continue
                
                degree = 1
                tmp = result // p
                while tmp % p == 0:
                    degree += 1
                    tmp //= p

                self.update_factorization(x, p, degree)
                self.update_factorization(y, p, degree)

                if p not in self.position_by_div:
                    self.position_by_div[p] = {x ,y}
                    if p < 20:
                        self.have_under20 += 1
                else:
                    self.position_by_div[p].add(x)
                    self.position_by_div[p].add(y)
        else:
            self.ones[x] = self.ones.get(x, 0) + 1
            self.ones[y] = self.ones.get(y, 0) + 1
        self.cache[key] = result
    
    def submit(self):
        prime_positions = []
        report = []
        for i in range(1, self.n + 1):
            if self.get_div_count(i) < 2:
                prime_positions.append(str(i))
                report.append("{}({})".format(i, self.get_div_count(i)))
        logging.info("Detected %d positions as prime: %s", len(prime_positions), " ".join(report))
        expected = len(self.primes) + 1
        assert len(prime_positions) >= expected
        prime_positions = prime_positions[:expected]
        message = "! {}".format(" ".join(prime_positions))
        response = self.request(message)
        dump_password(response)

    def time_is_up(self):
        return self.requests_count >= self.q        
    
    def solve(self):
        self.read_invite()
        for i in range(1, self.n, 2):
            self.get_gcd(i, i + 1)
            if self.have_under20 >= 8:
                break
        logging.info("first pass is done: have items for %d primes under 20", self.have_under20)
        while self.have_under20 < 8 and self.requests_count < min(self.q, 200):
            for i in range(1, self.n - 1):
                j = random.randint(i + 1, self.n)
                self.get_gcd(i, j)
                if self.have_under20 >= 8:
                    break
            logging.info("random requests are done: have items for %d primes under 20, used %d requests", self.have_under20, self.requests_count)
        while not self.time_is_up():
            compound_count = 0
            saved_count = self.requests_count
            prev_primes = len(self.position_by_div)
            for i in range(1, self.n + 1):
                if self.get_div_count(i) >= 2:
                    compound_count += 1
                    continue
                for p in self.primes:
                    if p not in self.position_by_div:
                        continue
                    if self.check_div(i, p):
                        continue
                    # choose first valid position from the set
                    for other_pos in self.position_by_div[p]:
                        break
                    if self.time_is_up():
                        break
                    self.get_gcd(i, other_pos)
                    if self.get_div_count(i) >= 2:
                        break
                    if len(self.position_by_div) > prev_primes:
                        logging.info("New prime appeared, start again")
                        break
                if len(self.position_by_div) > prev_primes:
                    logging.info("New prime appeared, start again")
                    break

            if compound_count + len(self.primes) + 1 >= self.n:
                break

            if len(self.position_by_div) > prev_primes:
                continue

            to_scan = []
            for special in [5, 7]:
                if special not in self.position_by_div:
                    continue
                for i in self.position_by_div[special]:
                    to_scan.append((i, "div by {}".format(special)))
            for i in range(1, self.n + 1):
                f = self.factorizations
                if i in f and len(f[i]) == 1:
                    if f[i].get(2, 0) == 1:
                        to_scan.append((i, "div by 2 only"))
                    elif f[i].get(3, 0) == 1:
                        to_scan.append((i, "div by 3 only"))

            for i in range(1, self.n + 1):
                to_scan.append((i, False))

            for i, special in to_scan:
                if self.get_div_count(i) < 2:
                    i_ones = self.ones.get(i, 0)
                    if i not in self.scanned and (special or (i_ones > 5 and random.random() < 0.3)):
                        logging.info("full scan for %d (got '1' %d times), special %s", i, i_ones, str(special))
                        for j in range(1, self.n + 1):
                            if i == j:
                                continue
                            if self.time_is_up():
                                break
                            self.get_gcd(i, j)
                            if self.get_div_count(i) >= 2:
                                break
                            if len(self.position_by_div) > prev_primes:
                                logging.info("New prime appeared, stopping scan")
                                break
                        if j >= self.n:
                            self.scanned.add(i)
                if self.time_is_up():
                    break
                if len(self.position_by_div) > prev_primes:
                    logging.info("New prime appeared, start again")
                    break
            logging.info("detected %d compound positions out of %d, have items for %d primes (%s), used %d requests",
                compound_count, self.n,
                len(self.position_by_div), str(sorted(self.position_by_div.keys())),
                self.requests_count)
        self.submit()


def main():
    tn = telnetlib.Telnet(host=TN_HOST, port=TN_PORT)
    solution = Solution(tn)
    solution.solve()
    return 0


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    sys.exit(main())
