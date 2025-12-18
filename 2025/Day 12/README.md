# Day 12

The entire problem description is a troll, as preliminary processing suffices to solve the problem completely.

Consider each test case separately. Let the region's dimensions be $R \times C$.

Since each present fits in a $3 \times 3$ square, the region can definitely fit

$$\left\lfloor \frac{R}{3} \right\rfloor \times \left\lfloor \frac{C}{3} \right\rfloor$$

presents of any shape. If the total number of presents is at most this value, we can immediately determine that the answer is yes.

Suppose there are $a_i$ presents of type $i$, and each such present occupies $b_i$ units of space. All the presents would occupy at least

$$\sum a_i b_i$$

units of space. If this is larger than $RC$, we can immediately determine that the answer is no.

It turns out that all test cases in my input fall in either of the above cases.