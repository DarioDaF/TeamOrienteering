from collections import defaultdict
from colorsys import hsv_to_rgb

import numpy as np
import matplotlib.pyplot as plt

import os

class Output:
  def __init__(self, ls):
    self.points = []
    self.hops = defaultdict(list)
    i = 0

    n = int(ls[i].split(' ')[1])
    i += 1

    self.m = int(ls[i].split(' ')[1])
    i += 1

    self.tmax = float(ls[i].split(' ')[1])
    i += 1

    for j in range(n):
      x, y, p = ls[i].split('\t')
      i += 1
      self.points.append((float(x), float(y), int(p)))

    h = int(ls[i].split(' ')[1])
    i += 1

    for j in range(h):
      c, p = ls[i].split('\t')
      i += 1
      self.hops[int(c)].append(int(p))

fname = './OutputsKevin/p7.4.t.out'
with open(fname, 'r') as fp:
  ls = fp.readlines()
o = Output(ls)

xs = []
ys = []
ps = []
for x, y, p in o.points:
  xs.append(x)
  ys.append(y)
  ps.append(p)

plt.scatter(xs, ys, c=ps)
plt.colorbar()

# for car, chops in o.hops.items():
#   x = [ o.points[0][0] ]
#   y = [ o.points[0][1] ]
#   for hop in chops:
#     px, py, pp = o.points[hop]
#     x.append(px)
#     y.append(py)
#   x.append(o.points[-1][0])
#   y.append(o.points[-1][1])
#   plt.plot(x, y, '-')
head_width = 1
hue = 0
step = 1.0 / o.m
for car, chops in o.hops.items():
  color = hsv_to_rgb(hue, 1, 0.8)
  lastx = o.points[0][0]
  lasty = o.points[0][1]
  for hop in chops:
    currx, curry, _ = o.points[hop]
    plt.arrow(lastx, lasty, currx - lastx, curry - lasty, head_width=head_width, length_includes_head=True, color=color)
    lastx, lasty = currx, curry
  plt.arrow(lastx, lasty, o.points[-1][0] - lastx, o.points[-1][1] - lasty, head_width=head_width, length_includes_head=True, color=color)
  hue += step

plt.title(fname)
plt.show()
