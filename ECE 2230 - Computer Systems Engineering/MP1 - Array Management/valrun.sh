#!/usr/bin/sh
#
# MP1 place in student directory
valgrind --leak-check=yes ./lab1 3 < ../tests/t00testinput_3 > gradingout_t00testinput_3 2> gradingout_valt00testinput_3
valgrind --leak-check=yes ./lab1 2 < ../tests/t01emptylist_2 > gradingout_t01emptylist_2 2> gradingout_valt01emptylist_2
valgrind --leak-check=yes ./lab1 10 < ../tests/t02quit_10 > gradingout_t02quit_10 2> gradingout_valt02quit_10
valgrind --leak-check=yes ./lab1 2 < ../tests/t03oneadd_2 > gradingout_t03oneadd_2 2> gradingout_valt03oneadd_2
valgrind --leak-check=yes ./lab1 4 < ../tests/t04oneaddremove_4 > gradingout_t04oneaddremove_4 2> gradingout_valt04oneaddremove_4
valgrind --leak-check=yes ./lab1 5 < ../tests/t05onereplace_5 > gradingout_t05onereplace_5 2> gradingout_valt05onereplace_5
valgrind --leak-check=yes ./lab1 9 < ../tests/t06nineadds_9 > gradingout_t06nineadds_9 2> gradingout_valt06nineadds_9
valgrind --leak-check=yes ./lab1 9 < ../tests/t07removes_9 > gradingout_t07removes_9 2> gradingout_valt07removes_9
valgrind --leak-check=yes ./lab1 9 < ../tests/t08removes_9 > gradingout_t08removes_9 2> gradingout_valt08removes_9
valgrind --leak-check=yes ./lab1 5 < ../tests/t09rejects_5 > gradingout_t09rejects_5 2> gradingout_valt09rejects_5
valgrind --leak-check=yes ./lab1 5 < ../tests/t10removefail_5 > gradingout_t10removefail_5 2> gradingout_valt10removefail_5
valgrind --leak-check=yes ./lab1 5 < ../tests/t11oneremoveall_5 > gradingout_t11oneremoveall_5 2> gradingout_valt11oneremoveall_5
valgrind --leak-check=yes ./lab1 9 < ../tests/t12incs_9 > gradingout_t12incs_9 2> gradingout_valt12incs_9
valgrind --leak-check=yes ./lab1 10 < ../tests/t13mixed_10 > gradingout_t13mixed_10 2> gradingout_valt13mixed_10
valgrind --leak-check=yes ./lab1 34 < ../tests/t14random_34 > gradingout_t14random_34 2> gradingout_valt14random_34
valgrind --leak-check=yes ./lab1 9 < ../tests/t15removalls_9 > gradingout_t15removalls_9 2> gradingout_valt15removalls_9
valgrind --leak-check=yes ./lab1 5 < ../tests/t16nolist_5 > gradingout_t16nolist_5 2> gradingout_valt16nolist_5
valgrind --leak-check=yes ./lab1 5 < ../tests/t17decs_5 > gradingout_t17decs_5 2> gradingout_valt17decs_5

