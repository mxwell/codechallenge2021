# Code Challenge T1

These are solutions for Code Challenge T1 round that was held in December 2021 at https://codechallenge.0x14.net .

Solutions in C++ can be compiled with the command: `g++ --std=c++17 -DLL='"%lld"' -Wall -Werror -Wextra -o main main.cpp`. Python scripts were run by Python 3.5.2. In most cases `stdin`/`stdout` are used for input/output.

## Challenge 1 - Roll the dice!

If the sum is less than 12, then print the sum + 1. Otherwise there is no win.

## Challenge 2 - Catch them all

A straightforward approach would take O(P * C * R * N) in time and probably would do just fine. I thought it's more interesting to implement the Aho-Corasick algorithm, that gives a solution with O(P * C * R) time after the trie is constructed in O(P * N).

## Challenge 3 - The night of the hunter

We need to implement parser for weight descriptions of 3 kinds. I just ignored everything except letters, digits and slash. Remaining stuff was grouped into letter -> weight mappings. It is more convenient to convert all weights into fractions. When dealing with fractions we must not forget about normalization: divide numerator and denominator by their GCD after each operation.

## Challenge 4 - Let’s build musical scales

I'm new to musical scales, so it took some time to understand that almost every note has two notations in the task. Exceptions are A, D, E, and G.

Given 7 notes and up to 2 notations, we have 2**7 variants of scale representation. Let's just iterate over them with bit mask and find one that starts with given initial note representation and has different capital letters.

## Challenge 5 - Invictus

The text file has non-ASCII characters, i.e. bytes with value > 127. The bytes form contiguous groups of 4 bytes. Let's try to convert them into ASCII characters.

If we pick some group and try to search it on the web, we can notice that it corresponds to UTF-8 hex representation of symbols like `Tag Latin Small Letter W`. They also start with bytes `F3 A0`, with 2 following bytes describing some offset. It didn't look like we can interpret the whole group of 4 bytes as UTF-8 code. Given first 2 bytes are the same for every group, let's focus on 2 final bytes - byte3 and byte4 - in each group.

Looking at the ranges of the bytes we can tell that we need to subtract `0x80` from each. Then I came up with the following conversion: `ascii_code = byte3 * 0x40 + byte4 + unknown_offset`. Let's iterate over `unknown_offset` and see if we can get `MANDELA` in the decoding. And there is one such decoding!

## Challenge 6 - What day is it?

I used Python's `datetime` to get weekday number and manully collected lists of weekday names. There are some ambiguities in words and their spelling, so it makes sense to check in a couple of sources.

## Challenge 7 - Escape or Die

Firstly notice that we have very convenient command `go to x,y`. It means we can go back to any checked position with just one command, without any backtracking.

We need the shortest path, it means that BFS suits us. When we discover exit, it will be the first time we visit the cell and we will have reached it via the shortest path. We push cell into the BFS queue after it's checked and possible moves are known. At the beginning of the BFS iteration we extract cell and move to its position with `go to x,y`, then visit and check unchecked adjacent cells, pushing them into the queue.

To recover the path we need to save for every cell the cell from which we entered it with one of the commands `north`, `east`, `south`, `west` (not `go to x,y`).

Python has Telnet client in its standard library, hence it would be a good choice to implement solution.

## Challenge 8 - Awesome Sales Inc.!

We need to find articulation points in an undirected graph. This can be solved with a known DFS-based algorithm, where we track times of entry `tin` into a vertex and minimal time `fup` among its children and itself.

## Challenge 9 - Collisions

My approach was pretty straightforward: check every pair of sprites for intersection, which requires `O(P * P)` without the time for intersection.

But since there are few sprites and they're not so large, I precalculated for every pair of sprites and every potentially leading to intersection relative position whether they intersect or not. The precalculation requires O(D * D * W * H), but needs to be done once and allows to check intersection in `O(1)`. Therefore the overall intersection complexity remains `O(P * P)`. The solution is not really fast, given `P = 50000`. But on my laptop it finished in under 10 minutes, which seems to be good enough.

## Challenge 10 - Packets delivery

I looked up how `.pcap` files are opened. It turned out that it contains captured network packets and can be read on Linux by the `tshark` utility. I converted the given file with `tshark` into a JSON file. Every packet was represented with data from several layers, like `frame`, `eth`, `ip`, and `icmp`. The last layer seemed the most promising, especially its fields `icmp.ident`, `icmp.seq`, `data`. The first two fields had two representations each, depending on assumed endianness (BE or LE). I chose representation that produced smaller numbers, because they looked like ASCII codes.

When I extracted `icmp.ident` from each packet and decoded with ASCII, I got the hint `What a mess, you will need to reorder everything to get the price`. I already noticed that values of the second field `icmp.seq` looked like sequential unique numbers that were shuffled. So the hint was useful: I don't need to look into `icmp.ident` anymore and I definitely should order packets according to `icmp.seq`.

After the reorder I noticed that sequence of values from the third field `data` contains `PNG` somewhere at the start. The next step was to save bytes of `data` into a file and open it as a PNG image. It was a QR code with the answer!

## Challenge 11 - ALOT Another library of tools

I implemented a greedy algorithm. Firstly the `N` lines are ordered lexicographically. Then the algorithm calculates score for every contigous range of `K` lines, extracts the best range and adds its score to the answer. This is repeated while there are remaining lines.

The score is calculated only from the first and the last lines of the range, because they differ the most and their score is the same as the score of the range.

Overall complexity is `O(N * N * line_length)`.

## Challenge 12 - The crypto bubble

Here we need to find the best score among shortest paths that give profit.

If we use BFS, it will find a shortest path, but we don't really know when to stop. If we increased amount of BTC, we might have not reached best score. It means we should keep going, but for how long?

My idea was to use a BFS-like algorithm, where we process vertices in batches. First batch consists of one vertex of origin - "BTC".  Every vertex, that is adjacent to first batch vertices and gets larger amount than before, is added to the second batch. When we are done with the first batch, we proceed to the second batch. Every vertex that appears during second batch processing is added to the third batch. And this is repeated. The benefit of batch processing is that vertices of the same batch represent final points in paths of the same length. Once we got amount of BTC > 1, we found a shortest path of length L. Once we finished the current batch, we can be sure we checked all paths of the length L.

We might not improve BTC amount even after we visited all vertices, so when do we stop? The path can go through a chain of almost all vertices before we reach an edge with favourable rate, which could throw us back somewhere at the start of the chain. And we would need to go almost along the whole chain again, before we get got BTC. This is the worst case I came up with. Overall the path would have length of about double of the vertex number. Let's say the number of vertices is `V`. Then to cover this scenario we would need to process around `2 * V` batches or, to be safe, `5 * V`.

Also we should store amount of currency reached at a vertex in the queue, together with the vertex. The reason is we are allowed to improve only upon amounts that were reached in the previous batch. If amounts are stored in a global storage, we have no way to tell in which batch some amount was reached.

## Challenge 13 - Find the New Earth

We are given a string looking like credentials for basic HTTP authentication. It's quite short, which means it should have been encoded with something unsophisticated cipher, e.g. Caesar cipher. The only tricky part is what to do with the underscore signs "_". Should we assume they are part of the alphabet? Is it the only non-letter character? Or we need to include all ASCII characters?

After a while I've tried to ignore the underscores, leave them as is and check all possible shifts inside alphabet of just `a..z`.  And boom! At some offset the first part of the string reads CYLON! This was just a first step of many, but the following steps seemed much easier compared to this initial puzzle.

So I restored the credentials and was able to download a file. It looked like output of `hexdump -C` command. I converted the hexadecimal representation to bytes and stored as a file. It was detected as GZIP format. After unzipping I got a plain text file with a dialog from Lord of the Ring movie. It contained non-ASCII characters, particularly two kinds of zero-width Unicode characters. There were 120 of them. Here I used another hint about 15 digits. 120 / 15 = 8. Looks like every number should be interpreted as a bit in byte. Indeed I got 15 digits of the password.

## Challenge 14 - SEND + MORE = MONEY

My solution consisted of two parts: (1) expression parsing to get reverse Polish notation, (2) recursive brute-force search through letter-to-digit mappings.

There are at most 10! mappings. Reverse Polish notation allows us to evaluate each expression in linear time. Expressions in the test cases were rather short, 5 operands at most. Therefore brute-force solution was quick enough.

## Challenge 15 - The revenge of the non-Tuentistic numbers

The problem is to multiply a very long sequence of numbers, which looks hard at first. But notice that we should use multiplication modulo some not so big number. It means that once we reach it and multiply by it, our remainder turns into zero. And all following multiplication will produce only 0.

Consequently our problem reduces to multiplication of needed numbers below 1e8+7. We can just check them all for being non-Tuentistic. The check is basically that number does not contain `2` at the same positions as `20`, `20,000`, `20,00,000`.

I didn't do it, but there is also quite obvious optimization with re-use of multiplications from previous test cases. This is easy since test cases are already ordered from smallest to largest. We just save each answer, then in the next test case take it and multiply on numbers, that were not included previously, to get the answer for that test case.

## Challenge 16 - Where are the primes at?

Here my solution was quite messy and included a few ad-hoc approaches, and I don't see the point in describing it in details.

Conceptually, I tried find positions of numbers that are multiples of primes and collect map from primes to their known multiples. The more primes I got covered, the more reliably I can check a number at arbitrary position. If I know that position `X` contains number divisible by prime `P`, I can check arbitrary position `Y` by the request `X Y` and if GCD in the response is a multiple of `P`, I can be sure the number at `Y` is also divisible by `P`, otherwise it's not divisible.

My goal was to check as many positions as possible, whether they are prime or not. Firstly I checked a position with the known multiples of primes. If I was unable to find prime divisors this way, then I tried brute-force approach and checked the position in pair with all other positions. This is costly, but I was likely to discover multiple of new prime during the sweep. It also made sense to re-check previous numbers with the new multiple.

The solution was not 100% guaranteed to detect all primes, but after several attemps and with some luck my solution guessed right and got the password.

## Challenge 17 - Super Digger Bros and the bulldozer of infinite buckets

This problem can be solved with Sprague–Grundy theorem. First we need to find a way to calculate Grundy value for piles. Them we need to calculate XOR of Grundy values of all piles. If it gives zero, then the initial state is losing, otherwise it is winning.

We can calculate Grundy values with the mex (minimum exclusion) function, but it is not practical for large piles. Often times the Grundy values can be described by periodic function. To see it we calculate with mex several first values, like 100, and print out. After that it is easy to notice that Grundy value for pile of size `V` is equal to `V modulo 3`. Though I have no proof for that.

## Challenge 18 - Bit Saver

Here we need to construct a Huffman tree from operation codes. But besides the optimality in size after coding, we need to minimize difference between the shortest and the longest code lengths. In other words, we need to minimize difference between leaf paths in the Huffman tree, or make it more balanced.

Huffman tree construction is a greedy algorithm that takes two nodes with **lowest frequency** and merges them into a new node. In order to improve balancing let's change that to selection of two nodes with **lowest frequency and lowest height** if frequencis are equal. Height of a node is the longest path length in a tree under the node. After merge it is calculated as a maximum of heights of two original nodes increased by one.

If we use something like heap to store nodes, we can implement this algorithm with O(N log N) time complexity, where N is a number of unique operation codes.

## Challenge 19 - Code Red Chaos

This is the first time I encountered type of math that is behind the CRC algorithm, so I've tried to draw an analogy with number theory.

Here is a vague description of the thought process:

I viewed polynomials as numbers, inference as addition of some number multiplied by a degree of two. Then we need to find a divisor `D` that is oblivious to addition of the inference numbers with any allowed degree, especially with `2^0 = 1`. Oblivious is in a sense that remainder won't change after the inference. Let's call inference as `I`. Then we need `I modulo D = 0`, that is `D` must be a divisor of all inference numbers. It also needs to be as large as possible and be an odd number. So we should find a GCD of inference numbers and then divide it by 2 as long as it is divisible by 2.

Usually GCD can be calculated with Euclid algorithm that needs modulo operation. Modulo operation is the same polynomial division with XOR as in the CRC algorithm, except that part with right padding with zeros.

If GCD happened to be an even number and has some number of contigous zeros at the end of binary representation, then we need to drop all those zeros to get an odd number.