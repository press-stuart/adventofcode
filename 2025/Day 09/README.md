# Day 9

This is up there on the list of programs that caused me immense emotional damage to write. Probably because I have no experience with working with polygons.

While I obtained the correct answer in mid-December, I realized that my solution was wrong, so I rewrote it after Christmas.

My solution also feels suspiciously long, so I'm probably solving the task wrongly. But it's (probably somewhat) fast theoretically *and* actually fast practically, running in less than 40 ms with my input (copium).

## Side Quest: Visualization

I visualized my input using Python's `matplotlib` library. The code and the plot representing my input are given in `d09vis.py` and `plot.png` respectively. The plot certainly hints the rough locations to check for the rectangle, but I'm stubborn and want to find a more general solution.

## Part 1

Try to use every pair of red tiles as opposite corners and take the largest result.

Time complexity: $O(N^2)$ where $N$ is the number of red tiles in the input.

## Part 2

A natural way to proceed with Part 2 is still to try every pair of red tiles, but with an additional check that the rectangle defined by the pair lies entirely within the polygon defined by the input. It remains to implement this check.

### Introduction: Point in Polygon

Let's first consider an easier problem: [check whether a point lies in the polygon](https://en.wikipedia.org/wiki/Point_in_polygon). It is simple to check whether the point lies on any of the borders, so it remains to check whether the point is inside or outside of the polygon. We can use **ray casting**: a point is inside the polygon if and only if every ray starting from the point and going in any direction intersects (the boundaries of) the polygon an odd number of times. Since the polygon's boundaries are aligned to the rectilinear axes, it suffices to check horizontal and vertical rays.

Clearly, a rectangle is in the polygon if and only if all points in the rectangle are also in the polygon. We aim to solve the point-in-polygon problem for all points simultaneously.

### Finding Boundaries and Polygon Orientation

You might ask, *"Aren't the boundaries already given in the input?"* Unfortunately, the input actually gives us the outermost cells of the polygon, which are adjacent to the boundaries (lines/edges) which we are interested in. Moreover, given any two consecutive red tiles (corner cells) in isolation, there are two possible ways to place boundaries, and we specifically want the outward-facing boundary.

Observe that we can *cycle* or *reverse* the list of input points without changing the polygon. To orient ourselves, we can find a reference edge with special properties and move it to the start of the list. Using the positive $(x,y)$-plane with the origin at the bottom-left corner, in my implementation, this reference edge is the **leftmost vertical edge** (smallest $x$-coordinate and aligned to the $y$-axis), oriented **downwards** (the first point has larger $y$-coordinate than the $x$-coordinate). After moving this edge to the start of the list by cycling and/or reversing, we know that the polygon is defined in the counterclockwise (CCW) direction, and the outside of the polygon is on the left of the first edge.

We process all sets of three consecutive points of the polygon in order. In any step, let the three points be `curr`, `succ`, `suc2`. We find the boundaries adjacent to these cells:

1. strictly between `curr` and `succ`, called *edge boundaries*; and
2. `succ`, called *corner boundaries*.

The edge boundaries are given by the boundary direction with respect to the `curr`-`succ` edge. By the reordering of the list, the outside of the polygon is on the left of the first edge, so the initial boundary direction is "left". All boundary directions can be computed inductively. Suppose we know the `curr`-`succ` boundary direction. By comparing the directions of the edges from `curr` to `succ` and from `succ` to `suc2`, we can determine the orientation of the corner at `succ` (CW or CCW) and, therefore, the `succ`-`suc2` boundary direction.

Next, note that only outward-facing corners have corner boundaries. By the previous definition of orientation, a corner is outward-facing if and only if it is CCW. A CCW corner has two boundaries, one on the `curr`-`succ` boundary direction and the other on the `succ`-`suc2` boundary direction.

While not present in my input, it is possible for two different boundaries to exist on the line between the same two cells. Observe the middle rows in the following example, where only outermost cells are indicated.

```
........
.#XXXX#.
.X....X.
.#XX#.X.
.#XX#.X.
.X....X.
.#XXXX#.
........
```

If two different boundaries exist on the same line, we assume no boundaries exist.

Thus, we fill up the 2D Boolean arrays `boundary_x` and `boundary_y`, where `boundary_x[x][y]` denotes whether there exists a boundary between $(x,y)$ and $(x-1,y)$, and `boundary_y[x][y]` denotes whether there exists a boundary between $(x,y)$ and $(x,y-1)$.

### Ray Casting and Rectangle in Polygon

Recall that a cell $(x,y)$ is in the polygon if and only if each of the horizontal and vertical rays starting from the cell intersect the boundaries an odd number of times. We can rewrite the constraint more concretely: there is an odd number of `true` values in `boundary_x[0..x][y]`, as well as in `boundary_y[x][0..y]`. Let the Boolean `valid[x][y]` denote whether $(x,y)$ is in the polygon.

We proceed to handle rectangle-in-polygon queries. Suppose the rectangle is defined by $a \leq x \leq b, c \leq y \leq d$. We could check if all `valid[x][y]` in range are `true`, but that would be slow. Instead, we can gain inspiration from [prefix sums](https://usaco.guide/silver/more-prefix-sums?lang=cpp#2d-prefix-sums) and precompute `p[x][y]`, the number of `true` values in `valid[0..x][0..y]`. For a rectangle query, we use `p` to find the number of `true` values in `valid[a..b][c..d]` which represents the number of cells that are in the rectangle and also in the polygon. Finally, we compare this value against the number of cells in the rectangle $(b-a+1) \times (d-c+1)$.

### Coordinate Compression

The solution explained above works in theory, but fails in practice. We iterate over huge sections of the grid and index our arrays using grid cells, but the grid is too large! Let the grid's range be $X \times Y$. Both the time and space complexities are $O(XY)$.

Observe that the polygon comprises of many rectangles, and similarly the grid can be subdivided into many large rectangular regions which are either all in the polygon or all outside the polygon. We transform the grid into a grid of rectangular regions of cells. Specifically, for each red point $(x,y)$ in the input, we draw four dividing lines between columns $x-1, x, x+1$ and between rows $y-1, y, y+1$. We have at most $2N$ horizontal and $2N$ vertical dividing lines. It is possible to show that

- Boundaries can only lie on dividing lines; and
- Each rectangular region of the grid defined by the dividing lines is either entirely in the polygon or entirely outside the polygon.

These properties are sufficient to allow us to use the old solution on the compressed grid. We map the coordinates of all points to the range $[0, 2N]$. The size of the grid, and therefore the time and space complexities, are reduced to $O(N^2)$.

This is a form of [coordinate compression](https://usaco.guide/silver/sorting-custom?lang=cpp#coordinate-compression).

### Summary

The solution can be split up into the following broad steps:

1. Reorder the list of points so that the first two points define the leftmost vertical edge oriented downwards
2. Determine important coordinates and dividing lines to compress the grid
3. Identify all boundaries by walking along the polygon and considering sets of three consecutive points
4. Identify regions that lie in the polygon using the parity of the numbers of boundaries along rows and columns
5. Precompute the 2D prefix sum array of regions that lie in the polygon
6. For each pair of input points, check whether the rectangle defined by them lie in the polygon using prefix sums and update the maximum area

Time and space complexities: $O(N^2)$

I have a hunch that there is an asymptotically faster solution involving a line sweep but I don't know how it would work.