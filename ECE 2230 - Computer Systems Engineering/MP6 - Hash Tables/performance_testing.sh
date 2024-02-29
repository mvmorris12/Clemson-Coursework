#### Q1
# Performance testing with Retrieve driver, random addressess, and various hashing methods and load factors
# echo "######################### -a 0.99" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h linear -a 0.99
# echo "######################### -a 0.95" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h linear -a 0.95
# echo "######################### -a 0.90" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h linear -a 0.90
# echo "######################### -a 0.75" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h linear -a 0.75
# echo "######################### -a 0.50" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h linear -a 0.50

# echo "" && echo ""
# echo "######################### -a 0.99" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h double -a 0.99
# echo "######################### -a 0.95" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h double -a 0.95
# echo "######################### -a 0.90" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h double -a 0.90
# echo "######################### -a 0.75" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h double -a 0.75
# echo "######################### -a 0.50" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h double -a 0.50

#  echo "" && echo ""
# echo "######################### -a 0.99" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h chain -a 0.99
# echo "######################### -a 0.95" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h chain -a 0.95
# echo "######################### -a 0.90" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h chain -a 0.90
# echo "######################### -a 0.75" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h chain -a 0.75
# echo "######################### -a 0.50" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h chain -a 0.50


#### Q2
# echo "########################## rand chain" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h chain -a 0.85
# echo "########################## seq chain" && echo "" && echo ""
# ./lab6 -r -i seq -m 65537 -h chain -a 0.85
# echo "########################## rand double" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h double -a 0.85
# echo "########################## seq double" && echo "" && echo ""
# ./lab6 -r -i seq -m 65537 -h double -a 0.85
# echo "########################## rand linear" && echo "" && echo ""
# ./lab6 -r -i rand -m 65537 -h linear -a 0.85
# echo "########################## seq linear" && echo "" && echo ""
# ./lab6 -r -i seq -m 65537 -h linear -a 0.85


#### Q3

echo "########################## 50k" && echo "" && echo ""
./lab6 -e -h linear -m 65537 -t 50000
echo "########################## 100k" && echo "" && echo ""
./lab6 -e -h linear -m 65537 -t 100000
echo "########################## 1000k" && echo "" && echo ""
./lab6 -e -h linear -m 65537 -t 200000