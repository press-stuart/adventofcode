import sys
import numpy as np
from scipy.optimize import milp, Bounds, LinearConstraint

ans_p1 = 0
ans_p2 = 0

for line in sys.stdin:
    words = line.split()
    words = [word[1:-1] for word in words] # remove brackets
    eqns = len(words[0])
    vars = len(words) - 2

    # Part 2
    A_p2 = np.zeros((eqns, vars))
    for i in range(0, vars):
        for j in words[i+1].split(','):
            A_p2[int(j), i] = 1

    b_p2 = np.array([int(w) for w in words[-1].split(',')])
    constraint_p2 = LinearConstraint(A_p2, lb=b_p2, ub=b_p2)

    c_p2 = np.ones(vars)
    integrality_p2 = np.ones(vars)
    bounds_p2 = Bounds(lb=0, ub=np.inf)
    result_p2 = milp(c_p2, integrality=integrality_p2, bounds=bounds_p2, constraints=constraint_p2)
    ans_p2 += np.round(result_p2.fun).astype(int)

    # Part 1
    A_p1 = np.concatenate((A_p2, np.diagflat([-2] * eqns)), axis=1)
    b_p1 = np.array([1. if words[0][i] == '#' else 0. for i in range(eqns)])
    constraint_p1 = LinearConstraint(A_p1, lb=b_p1, ub=b_p1)

    c_p1 = np.concatenate((c_p2, np.zeros(eqns)))
    integrality_p1 = np.ones(vars + eqns)
    bounds_p1 = Bounds(
        lb=np.zeros(vars + eqns),
        ub=np.concatenate((np.ones(vars), np.full((eqns,), np.inf)))
    )
    result_p1 = milp(
        c_p1, integrality=integrality_p1, bounds=bounds_p1,
        constraints=constraint_p1, options={'presolve': False}
    )
    ans_p1 += np.round(result_p1.fun).astype(int)

print(ans_p1)
print(ans_p2)