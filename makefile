All: present.h sched.h mth.h present.cpp sched.cpp mth.cpp main.cpp
	g++ -std=c++11 -lpthread mth.cpp present.cpp sched.cpp main.cpp -o ./emain
run:
	time ./emain 2>xxx
