import sys

covered = [7, 6, 7, 5, 7, 7]
yes = 0
undecided = []

for line in sys.stdin:
    words = line.split()
    rows, cols = [int(w) for w in words[0][:-1].split('x')]
    freq = [int(w) for w in words[1:]]

    maxfree = (rows // 3) * (cols // 3)
    if sum(freq) <= maxfree:
        yes += 1
        continue

    mincover = sum([freq[i] * covered[i] for i in range(6)])
    if rows * cols < mincover:
        continue

    undecided.append(line)

print('yes:', yes)
print('undecided:')
for line in undecided:
    print(line)