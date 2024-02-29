# Retrieve driver testing with various insertion and hashing methods
# ./lab6 -r -i rand -m 65537 -h linear
# ./lab6 -r -i rand -m 65537 -h double
# ./lab6 -r -i rand -m 65537 -h chain
# ./lab6 -r -i seq -m 65537 -h linear
# ./lab6 -r -i seq -m 65537 -h double 
# ./lab6 -r -i seq -m 65537 -h chain

# # Rehash driver testing with verious hashing methods
# ./lab6 -b -v -m 23 -h linear
# ./lab6 -b -v -m 23 -h double
# ./lab6 -b -v -m 23 -h chain

# # Deletion driver testing with various hashing methods
# ./lab6 -d
# ./lab6 -d -h linear
# ./lab6 -d -h double
# ./lab6 -d -h chain

# # Equilibrium driver testing with various hashing methods
# ./lab6 -e -t 2 -v -m 47 -h linear
# ./lab6 -e -t 2 -v -m 47 -h double
# ./lab6 -e -t 2 -v -m 47 -h chain

# # Retrieval driver testing with varous load factors and hashing methods
# ./lab6 -r -m 655373 -i rand -h linear -a 0.9
# ./lab6 -r -m 655373 -i rand -h linear -a 0.7
# ./lab6 -r -m 655373 -i rand -h linear -a 0.5
# ./lab6 -r -m 655373 -i rand -h linear -a 0.3
# ./lab6 -r -m 655373 -i rand -h linear -a 0.1
# ./lab6 -r -m 655373 -i rand -h double -a 0.9
# ./lab6 -r -m 655373 -i rand -h double -a 0.7
# ./lab6 -r -m 655373 -i rand -h double -a 0.5
# ./lab6 -r -m 655373 -i rand -h double -a 0.3
# ./lab6 -r -m 655373 -i rand -h double -a 0.1
# ./lab6 -r -m 655373 -i rand -h chain -a 0.9
# ./lab6 -r -m 655373 -i rand -h chain -a 0.7
# ./lab6 -r -m 655373 -i rand -h chain -a 0.5
# ./lab6 -r -m 655373 -i rand -h chain -a 0.3
# ./lab6 -r -m 655373 -i rand -h chain -a 0.1

# # Equilibrium driver testing with large tables
# ./lab6 -e -m 65537 -h linear -t 100000
# ./lab6 -e -m 65537 -h double -t 100000
# ./lab6 -e -m 65537 -h chain -t 100000

# # Rehash driver with small tables with even # of entries, may fail
# ./lab6 -b -m 20 -h linear -v
# ./lab6 -b -m 20 -h double -v
# ./lab6 -b -m 20 -h chain -v

# In the small table testing with even number of entries, the double hashed test fails as the 
# double hashing algorith never reaches an empty space and continues to loop through the table
# indefinitely. In my code, I added a feature that marks the initial hash location, and exits
# the program when that same location is landed on after iterating through the rest of the table. 
# Essentially, this shows that the double hash algorithm is unable to find an appropriate location
# for the new entry. This can also be seen on line 3261 of testing_output.txt:
# 
# repeated insert loop detected, exiting..
# lab6: lab6.c:445: RehashDriver: Assertion `code == 0' failed.
# ./testing.sh: line 49: 87716 Aborted                 (core dumped) ./lab6 -b -m 20 -h double -v


# Valgrind to show no memory leks exist
valgrind --leak-check=full -s ./lab6 -e -m 65537 -h linear
valgrind --leak-check=full -s ./lab6 -e -m 65537 -h double
valgrind --leak-check=full -s ./lab6 -e -m 65537 -h chain
