All: present.h sched.h sched.cpp 
	g++ -std=c++11 -lpthread present.cpp sched.cpp main.cpp -o ./emain
run:
	./emain 2>xxx
