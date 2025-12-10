# Day 10

One of those problems where you need to dig up some library written by CS/math gods, so you head over to Python and wrestle with a package manager, the package, and finally the code, because even gods make mistakes.

## Preamble: Brute Force

I first completed Part 1 using C++.

Notice that the order of pressing buttons does not matter in the end. Moreover, pressing any button twice is equivalent to doing nothing, so we should press each button at most once. This means two choices for each of $N$ buttons, and we can easily brute force all $2^N$ choices given the small inputs. If there are $M$ light indicators, directly simulating the toggling of each light takes $O(2^N NM)$ time. In my input, $N \leq 13$ and $M \leq 10$.

Since the input has very few lights, a simple optimization is to use binary to represent the state of the lights. We can convert the goal state of the lights to an $M$-bit integer $b$. The lights toggled by a button can also be converted to $M$-bit integers $a_i$, and the end state after pushing some buttons is the XOR of their respective $a_i$. My program `d10p1.cpp` implements this approach.

There is a [meet-in-the-middle](https://usaco.guide/gold/meet-in-the-middle?lang=cpp) approach to brute force faster. We split the buttons into two roughly-equal sets and brute force the $2^{N/2}$ combinations of each set. Place the results of the first half (each possible state of lights and the minimum number of button presses needed for each state) into a hashed set. Then for each result state $r$ of the second half, it is possible to reach the goal state $b$ if the XOR-expression $r \oplus b$ exists in the hashed set. The time complexity of this solution is $O(2^{N/2} NM)$.

Unfortunately, it is inefficient to extend the above solutions to Part 2 as the search space grows dramatically. Let $K$ be the maximum value of a counter. In my input, $K < 300$. To continue using the 0-1 approach of selecting buttons, we would need $K$ copies of each button which would take $O(2^{NK/2} NMK)$ time. A classic speedup is to use $\lceil \log(K) \rceil$ versions of each button, where each version is equivalent to pressing the button $2^0, 2^1, 2^2, \dots$ times. This would take $O(2^{N \log(K)/2} NM \log(K))$ time which is still too slow.

## Integer Linear Programming

[ILP](https://en.wikipedia.org/wiki/Integer_programming)

[`scipy.optimize.milp`](https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.milp.html#scipy.optimize.milp)

Exception case: `[##.#.##..] (1,4,5,6,8) (2,3,4,6,8) (1,3,5,7) (0,1,2,3,4,5,7,8) (0,3) (1,2,3,4,5,7,8) (0,3,6,7,8) (0,1,3,4,5,6,7,8) {44,47,31,59,56,47,39,40,58}`