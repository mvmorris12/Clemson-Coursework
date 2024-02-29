# Michael Morris
# mvmorri
# ECE2230,1
# MP5 - test script
# This script runs lab5 multiple times against a variety of different test cases
# 
# Tests with insertions only on small trees
./lab5 -o -w 5 -t 10 -v
./lab5 -r -w 5 -t 10 -v -s 1
./lab5 -p -w 5 -t 10 -v -s 1
# Tests with insertions only on large trees
./lab5 -o -w 2 -t 1000
./lab5 -o -w 2 -t 10000
./lab5 -o -w 2 -t 100000
./lab5 -o -w 2 -t 500000
./lab5 -o -w 2 -t 1000000
./lab5 -r -w 2 -t 1000
./lab5 -r -w 2 -t 10000
./lab5 -r -w 2 -t 100000
./lab5 -r -w 2 -t 500000
./lab5 -r -w 2 -t 1000000
./lab5 -o -w 16 -t 1000
./lab5 -o -w 16 -t 10000
./lab5 -o -w 16 -t 100000
./lab5 -o -w 16 -t 500000
./lab5 -o -w 16 -t 1000000
./lab5 -r -w 16 -t 1000
./lab5 -r -w 16 -t 10000
./lab5 -r -w 16 -t 100000
./lab5 -r -w 16 -t 500000
./lab5 -r -w 16 -t 1000000
./lab5 -o -w 20 -t 1000
./lab5 -o -w 20 -t 10000
./lab5 -o -w 20 -t 100000
./lab5 -o -w 20 -t 500000
./lab5 -o -w 20 -t 1000000
./lab5 -r -w 20 -t 1000
./lab5 -r -w 20 -t 10000
./lab5 -r -w 20 -t 100000
./lab5 -r -w 20 -t 500000
./lab5 -r -w 20 -t 1000000
./lab5 -p -w 16 -t 1000
./lab5 -p -w 16 -t 10000
./lab5 -p -w 16 -t 100000
./lab5 -p -w 16 -t 500000
./lab5 -p -w 16 -t 1000000
./lab5 -p -w 20 -t 1000
./lab5 -p -w 20 -t 10000
./lab5 -p -w 20 -t 100000
./lab5 -p -w 20 -t 500000
./lab5 -p -w 20 -t 1000000
# Tests special cases with insertions and removes for all unit drivers
./lab5 -o -u 0
./lab5 -r -u 0
./lab5 -p -u 0
./lab5 -o -u 1
./lab5 -r -u 1
./lab5 -p -u 1
./lab5 -o -u 2
./lab5 -r -u 2
./lab5 -p -u 2
./lab5 -o -u 3
./lab5 -r -u 3
./lab5 -p -u 3
./lab5 -o -u 4
./lab5 -r -u 4
./lab5 -p -u 4
./lab5 -o -u 5
./lab5 -r -u 5
./lab5 -p -u 5
./lab5 -o -u 6
./lab5 -r -u 6
./lab5 -p -u 6
./lab5 -o -u 7
./lab5 -r -u 7
./lab5 -p -u 7
# # Tests with random insertions and deletions on small and large trees
./lab5 -e -w 4 -t 10 -v -s 2
./lab5 -e -w 2 -t 1000
./lab5 -e -w 2 -t 10000
./lab5 -e -w 2 -t 100000
./lab5 -e -w 2 -t 500000
./lab5 -e -w 2 -t 1000000
./lab5 -e -w 16 -t 1000
./lab5 -e -w 16 -t 10000
./lab5 -e -w 16 -t 100000
./lab5 -e -w 16 -t 500000
./lab5 -e -w 16 -t 1000000
./lab5 -e -w 20 -t 1000
./lab5 -e -w 20 -t 10000
./lab5 -e -w 20 -t 100000
./lab5 -e -w 20 -t 500000
./lab5 -e -w 20 -t 1000000
# # Check for memory leaks
valgrind ./lab5 -r
valgrind ./lab5 -o
valgrind ./lab5 -e
valgrind ./lab5 -e -w 2