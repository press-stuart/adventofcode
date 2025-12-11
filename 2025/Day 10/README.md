# Day 10

One of those problems where you need to dig up some library written by CS/math gods, so you head over to Python and wrestle with a package manager, the package, and finally the code, because even gods make mistakes.

## Preamble: Brute Force

I first completed Part 1 using C++.

Notice that the order of pressing buttons does not matter in the end. Moreover, pressing any button twice is equivalent to doing nothing, so we should press each button at most once. This means two choices for each of $N$ buttons, and we can easily brute force all $2^N$ choices given the small inputs. If there are $M$ light indicators, directly simulating the toggling of each light takes $O(2^N NM)$ time. In my input, $N \leq 13$ and $M \leq 10$.

Since the input has very few lights, a simple optimization is to use binary to represent the state of the lights. We can convert the goal state of the lights to an $M$-bit integer $b$. The lights toggled by a button can also be converted to $M$-bit integers $a_i$, and the end state after pushing some buttons is the XOR of their respective $a_i$. My program `d10p1.cpp` implements this approach.

There is a [meet-in-the-middle](https://usaco.guide/gold/meet-in-the-middle?lang=cpp) approach to brute force faster. We split the buttons into two roughly-equal sets and brute force the $2^{N/2}$ combinations of each set. Place the results of the first half (each possible state of lights and the minimum number of button presses needed for each state) into a hashed set. Then for each result state $r$ of the second half, it is possible to reach the goal state $b$ if the XOR-expression $r \oplus b$ exists in the hashed set. The time complexity of this solution is $O(2^{N/2} NM)$.

Unfortunately, it is inefficient to extend the above solutions to Part 2 as the search space grows dramatically. Let $K$ be the maximum value of a counter. In my input, $K < 300$. To continue using the 0-1 approach of selecting buttons, we would need $K$ copies of each button which would take $O(2^{NK/2} NMK)$ time. A classic speedup is to use $\lceil \log(K) \rceil$ versions of each button, where each version is equivalent to pressing the button $2^0, 2^1, 2^2, \dots$ times. This would take $O(2^{N \log(K)/2} NM \log(K))$ time which is still too slow.

## Integer Linear Programming (ILP)

I turned to [ILP](https://en.wikipedia.org/wiki/Integer_programming) for Part 2.

Let the number of presses of each button be $x_0, x_1, \dots, x_{N-1}$. The counter values in the goal state determine $M$ linear equations on the button presses. Take the first test case in the example.

```
[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
```

Only buttons $4$ and $5$ increment counter $0$ which has a goal state of $3$. Thus, we have $x_4 + x_5 = 3$.

Represent the variables using a vector

$$\mathbf{x} = \begin{bmatrix} x_0 & x_1 & \dots & x_{N-1} \end{bmatrix}^\top.$$

After constructing the $M$ equations for each counter, we have the matrix equation

$$\mathbf{A} \mathbf{x} = \mathbf{b}$$

where $\mathbf{A}$ is a $M \times N$ matrix of ones and zeros describing which counters get incremented by each button, and $\mathbf{b}$ is a $M \times 1$ vector holding the goal state of the counters. We want all $x_i$ to be non-negative integers while minimizing

$$x_0 + x_1 + \dots + x_{N-1}.$$

Thus, we have the $N \times 1$ coefficient vector

$$\mathbf{c} = \begin{bmatrix} 1 & 1 & \dots & 1 \end{bmatrix}^\top.$$

We have reduced the problem to an ILP in standard form. Since I didn't want to read textbooks and papers and write hundreds of lines of code yet, I used the [`scipy.optimize.milp`](https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.milp.html#scipy.optimize.milp) implementation provided in the Python SciPy package.

## ILP with Modulo-2 Arithmetic

For the sake of completion, I tried to solve Part 1 with ILP.

Recall from the first section that toggling lights with button presses is equivalent to XOR, or modulo-2 addition. Hence, for the first light indicator in the example

```
[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
```

we have the equation

$$x_4 \oplus x_5 = 0.$$

That is, there exists some non-negative integer $x_{-1}$ such that

$$x_4 + x_5 - 2x_{-1} = 0.$$

Similarly, the equation for the second light indicator is

$$x_1 + x_5 - 2x_{-2} = 1.$$

In this way, we introduce $M$ new variables $x_{-1}, x_{-2}, \dots, x_{-M}$, one for each equation. We use ILP to solve for the $N+M$ variables, though the function that we minimize,

$$x_0 + x_1 + \dots + x_{N-1}$$

ignores the extra $M$ variables. Hence, the coefficient vector consists of $N$ ones followed by $M$ zeros. In my solution, $x_0, x_1, \dots, x_{N-1}$ are bounded to the range $[0,1]$, while $x_{-1}, x_{-2}, \dots, x_{-M}$ are bounded to $[0,\infty)$, though it's probably fine to bound all of them to $[0,\infty)$ since the minimizer function works either way.

To my horror, my answer for Part 1 using ILP was off by exactly $1$ from the correct answer using brute force. After isolating the offending test case, I found that the $\mathbf{x}$ returned by `scipy.optimize.milp` did not satisfy the $\mathbf{A} \mathbf{x} = \mathbf{b}$ constraint. I was stumped as all matrices and vectors that I used were exactly as I expected them to be.

Finally, I stumbled upon [this StackOverflow post](https://or.stackexchange.com/questions/11618/equivalent-constraints-in-milp-give-different-answers). While OP's issue is different from mine and uses `linprog` instead of `milp`, the post suggested that there was a bug in the presolve capabilities. Turning off presolve gave me the expected answer.

<details>
    <summary>(SPOILER) The pesky test case</summary> 

    [.#...] (0,1) (2,3) (1,2,3) (0,3) (1,4) {21,13,9,21,1}
</details>
