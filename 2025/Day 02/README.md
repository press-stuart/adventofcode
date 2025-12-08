# Day 2

This would have been an easy problem if I didn't ~~want to flex~~ over-engineer and over-optimize it.

The procedure to preprocess the input is not too interesting, so I will omit it. Let $`N`$ be the number of ranges and let $`M`$ be the largest ID in the given ranges. In my input, $`30 < N < 40`$ and $`10^{9} < M < 10^{10}`$. The maximum length of an ID is given by $`\lceil \log_{10} M \rceil`$, but I will treat this as a constant ($`10`$) for ease of asymptotic analysis. Thus, many string operations, such as converting strings to integers and vice versa, are assumed to run in constant time.

## Naive Solution

The simplest solution would be to iterate all over IDs. For each ID, check if it is invalid and within any of the given ranges. Sum up the IDs that satisfy both conditions.

The time complexity of this solution is $`O(NM)`$ which is certainly good enough if you're willing to wait for a few minutes.

## Reframing the Problem

It's not difficult to do better. Observe that there aren't that many distinct invalid IDs at all. Let's introduce the following definition:

> An ID obtained by taking some sequence of digits and repeating it $`r`$ times is called *order-*$`r`$.

Taking the maximum length of an ID to be $`10`$, if an invalid ID is formed by repeating a sequence of digits twice, then the sequence is at most $`5`$ digits long, giving $`10^5`$ distinct order-2 IDs in Part 1. More generally, we can approximate the number of order-2 IDs by $`10^{\frac{\lceil \log_{10} M \rceil}{2}} \approx \sqrt{M}`$.

As the number of repetitions increases, the number of invalid IDs decrease exponentially, so $`\sqrt{M}`$ is still a reasonable approximation in Part 2.

Instead of iterating over all IDs and checking if they are invalid, we would prefer to iterate over invalid IDs. Or, rather, we iterate over the sequence of digits that is repeated, and generate invalid IDs.

Similar to the naive solution, we need to check if each invalid ID is within any range. Linear search uses $`O(N \sqrt{M})`$ time. There are several different asymptotically faster approaches using binary search. These are left as an exercise to the reader.

A caveat for Part 2 is that invalid IDs may be counted more than once, and must therefore be appropriately dealt with. For example,

- An order-4 ID is also an order-2 ID, so we do not need to check for order-4 IDs separately from order-2 IDs.
- An order-6 ID is also an order-2 ID and an order-3 ID, so we need to subtract the sum of order-6 IDs.

The answer for Part 2 can be viewed as a weighted sum of sums of order-$`r`$ IDs for all $`r`$. Since the number of digits is small, we can manually compute the multipliers. From the above analysis, we see that the multiplier is $`0`$ for $`r=4`$ and $`-1`$ for $`r=6`$.

In any case, this solution should be good enough to solve the problem in less than one second given the small input limits.

## Asymptotically Optimal Solution

We seek to derive a solution with time complexity $`O(N)`$, which is asymptotically optimal as the input scales with $`N`$. A key assumption here is that the ranges given in the input are disjoint, which is true of my input. (Otherwise, refer to the *Appendix*.) So we only need to solve for the sum of invalid IDs in a single range, which we denote by $`[L,U]`$, and our goal is to do so in constant time.

Crucially, we observe that certain sequences of invalid IDs form arithmetic progressions, such as $`\{ 11, 22, \dots, 99 \}`$ and $`\{ 1010, 1111, \dots, 9999 \}`$. We can state this fact more concretely by introducing a notation for the procedure of repeating digits.

> Let $`i(S,r)`$ be the ID obtained by taking the sequence of digits $`S`$ and repeating it $`r`$ times. We will allow $`S`$ to have leading zeroes, but $`i(S,r)`$ will have its leading zeroes removed. For example, $i(6,3) = 666$ and $`i(06,3) = 60606`$.

Thus, if $`S`$ has no leading zeroes, then $`i(S,r)`$ is an order-$`r`$ ID. Finally, for fixed integers $`k,r`$, we see that the sequence

$$\left( i(n,r) \right)_{n = 10^k}^{10^{k+1}-1}$$

forms an arithmetic progression with difference

$$i(\underbrace{0 \dots 0}_{k \text{ digits}} 1,r).$$

It is well known that the sum of terms of any arithmetic progression can be computed in constant time using a formula.

We also note that for a fixed $`r`$, $`i(n,r)`$ strictly increases with $`n`$. Hence, there exists some contiguous range of integers $`[s,e]`$ (denote an empty range with $`s > e`$) such that $`n`$ lies in $`[s,e]`$ if and only if $`i(n,r)`$ lies in $`[L,U]`$. $`[s,e]`$ can be split into disjoint ranges using the powers of $`10`$ as boundary points. Hence, the $`i(n,r)`$ form disjoint arithmetic progressions, and the number of arithmetic progressions is at most the number of digits $`\lceil \log_{10} M \rceil`$.

Let $`s'`$ be the smallest $`n`$ such that $`i(n,r) \geq L`$. Let $`e'`$ be the largest $`n`$ such that $`i(n,r) \leq U`$, or $0$ if none exist. Then the range $`[s',e']`$ is a valid choice of $`[s,e]`$ above, because

- If some $`i(n,r)`$ lie in $`[L,U]`$, then $`[s,e]`$ is uniquely determined and we indeed have $`s'=s, e'=e`$.
- Otherwise, $`s' = e' + 1`$, which indeed denotes an empty range.

We find $`e'`$ as follows. Let $`d`$ be the number of digits in $`U`$. Note that the number of digits in an order-$`r`$ ID is necessarily a multiple of $`r`$. Consider two cases.

1. If $`d`$ is not a multiple of $`r`$, then no order-$`r`$ ID has $`d`$ digits. The next largest possible number of digits is the largest multiple of $`r`$ that is smaller than $`d`$. The largest order-$`r`$ ID is solely composed of '$9$'s. We have $`e' = 10^{\lfloor \frac{d}{r} \rfloor} - 1`$.
2. If $`d`$ is a multiple of $`r`$, then let $`n'`$ be the first $`\frac{d}{r}`$ digits of $`U`$, so that $`10^{\frac{d}{r}-1} \leq n' < 10^{\frac{d}{r}}`$. We construct $`i(n',r)`$ which has $`d`$ digits and the first $`\frac{d}{r}`$ digits are the same as $`U`$, so $`|U - i(n',r)| < 10^{d - \frac{d}{r}}`$. Also note that $`i(n',r)`$ is in the arithmetic progression with difference $`i(\underbrace{0 \dots 0}_{\frac{d}{r} - 1 \text{ digits}} 1,r) > 10^{d - \frac{d}{r}}`$. Thus, we further consider two cases.
   1. Suppose $`i(n',r) \leq U`$. If we add the common difference, we get $`i(n'+1,r) > U`$, so we return $`n'`$.
   2. Suppose $`i(n',r) > U`$. If we subtract the common difference, we get $`i(n'-1,r) < U`$, so we return $`n'-1`$.

To find $`s'`$, we reuse the above procedure to find the largest $`n`$ such that $`i(n,r) \leq L-1`$, then add one. Thus, we have found the sum of order-$`r`$ IDs inside a given range for a fixed $`r`$ in constant time, ignoring the length term $`\lceil \log_{10} M \rceil`$.

Finally, note that this solution also counts invalid IDs more than once. We deal with it in the same way as the second solution.

This completes the solution with time complexity $`O(N)`$.

## Appendix

We could try to include the maximum length term $`m := \lceil \log_{10} M \rceil`$. For all possible orders $r$ with $2 \leq r \leq m$, we sum over at most $m$ arithmetic progressions, which seems to suggest a time complexity of $`O(Nm^2)`$. In reality, many other operations which are typically assumed to run in constant time, including basic arithmetic operations, scale with $`m`$ as well.

If the $`N`$ given ranges are not disjoint, it is possible to merge non-disjoint ranges in $`O(N \log N)`$ time, after which the above solution can be run. Details are left as an exercise to the reader.