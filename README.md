# AA-tree
Set collection based on AA-tree

## Description

Set collection realisation based on [AA-tree](https://en.wikipedia.org/wiki/AA_tree) data structure. As a [standard set](https://en.cppreference.com/w/cpp/container/set) from [STL](https://en.wikipedia.org/wiki/Standard_Template_Library) library, this data structure implements insert, erase, find and lower_bound functions all having real-time O(log(n)) [time complexity](https://en.wikipedia.org/wiki/Time_complexity) and supports bidirectional iterators.

## Benchmarks

The benchmark uses compiler GCC v9.3.0 on Linux 64-bits (Ubuntu 20.04). The reference system uses a Core i3-8130U CPU @ 2.2GHz. All evaluations made in single-thread mode.
|  Input Data                      |  Operations   |   STL set     |  This set   |
|  ------------                    |  ----------   |  ---------    |  --------   |
|  10^7 numbers, Sorted sequense   |    insert     |    15.28s     |   15.67s    |
|  10^7 numbers, Sorted sequense   | insert, erase |    26.34s     |   40.11s    |
|  10^7 numbers, Sorted sequense   | insert, find  |    23.61s     |   20.29s    |
|  10^7 numbers, Random sequense   |    insert     |    23.63s     |   34.82s    |
|  10^7 numbers, Random sequense   | insert, erase |   	48.86s     |   78.53s    |
|  10^7 numbers, Random sequense   | insert, find  |   	44.03s     |   53.72s    |
