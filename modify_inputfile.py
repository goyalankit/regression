from __future__ import division
from numpy import max

with open("madelon") as f:
    content = f.readlines()

n,d = content[0].split()
n = int(n)+1; d = int(d)

Y = [content[i].split()[0] for i in range(1,n)]

X = [content[x].split()[1:] for x in range(1,n)]

A = [[int(X[y][i]) for i in range(len(X[y]))] for y in range(len(X))]
maxA = max(A)
print maxA

Y = [int(content[i].split()[0])/maxA for i in range(1,n)]

# for i in Y:
# 	print i

Q = [[(i, A[i][j]/maxA) for i in range(len(X)) if A[i][j] != 0] for j in range(len(X[0]))]

# for i in Q:
#     for j in i:
#         print j[0],j[1],
#     print
