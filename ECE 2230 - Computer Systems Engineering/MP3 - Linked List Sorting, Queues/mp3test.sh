#!/bin/bash
# mp3test.sh
# Harlan Russell
# ECE 2230, Fall 2023
#
# A simple shell script for MP3 to generate data for performance analysis
#
# This script should run in the range from about 15 to 30 seconds.  If the
#    run time is much longer then there is a bug in your lab3 sorts.  If
#    the run time is much shorter, then you need to increase the size
#    of the lists (see "sizes" below).
#
# There are two options to execute this file: either make the file executable
#    or run the file in a new shell.
#
# Option 1:  Make the script executable.  In a terminal type:
#      chmod +x mp3test.sh
#      ./mp3test.sh
#
# Option 2: run the program sh (this is a command shell) and tell sh to read
#    the commands from the file mp3test.sh
#     sh mp3test.sh
#
# There are four tests for:
#    Insertion Sort, Recursive Selection Sort, Iterative Selection Sort, and
#    Merge Sort
#
# Each sort is run with random, ascending, and decending lists.
# 
# "sizes" is used to hold the size for each trial
#
# TEST 1 Insertion sort
echo -e "The date today is `date`\t\t"
echo -e "eth insertion sort with random list\t\t"
sizes="1000 4500 6750 9000 13500 18000"
for listsize in $sizes ; do
   ./geninput eth $listsize 1 1 | ./lab3 3
done
echo -e "mc insertion sort with random list\t\t"
for listsize in $sizes ; do
   ./geninput mc $listsize 1 1 | ./lab3 3
done
echo -e "insertion sort with ascending list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 2 1 | ./lab3 3
done
echo -e "insertion sort with descending list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 3 1 | ./lab3 3
done
#
# TEST 2 Recursive selection sort
echo -e "eth recursive selection sort with random list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 1 2 | ./lab3 3
done
echo -e "mc recursive selection sort with random list\t\t"
for listsize in $sizes ; do
   ./geninput mc $listsize 1 2 | ./lab3 3
done
echo -e "recursive selection sort with ascending list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 2 2 | ./lab3 3
done
echo -e "recursive selection sort with descending list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 3 2 | ./lab3 3
done
#
# TEST 3 Iterative selection sort
echo -e "eth iterative selection sort with random list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 1 3 | ./lab3 3
done
echo -e "mc iterative selection sort with random list\t\t"
for listsize in $sizes ; do
   ./geninput mc $listsize 1 3 | ./lab3 3
done
echo -e "iterative selection sort with ascending list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 2 3 | ./lab3 3
done
echo -e "iterative selection sort with descending list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 3 3 | ./lab3 3
done
#
# TEST 4 Merge sort
echo -e "eth merge sort with random list\t\t"
sizes="18000 150000 300000 450000 600000 750000"
for listsize in $sizes ; do
   ./geninput eth $listsize 1 4 | ./lab3 3
done
echo -e "mc merge sort with random list\t\t"
for listsize in $sizes ; do
   ./geninput mc $listsize 1 4 | ./lab3 3
done
echo -e "merge sort with ascending list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 2 4 | ./lab3 3
done
echo -e "merge sort with descending list\t\t"
for listsize in $sizes ; do
   ./geninput eth $listsize 3 4 | ./lab3 3
done
#
echo -e "end\t\t"
