All:sched bubblebuild

sched: present.h sched.h mth.h present.cpp sched.cpp mth.cpp main.cpp
	g++ -std=c++11 -lpthread mth.cpp present.cpp sched.cpp main.cpp -o ./obj/emain
bubblebuild:
	g++ -std=c++11 -lpthread bubblebuild.cpp -o ./obj/ebubblebuild
runsched:
	time ./obj/emain 2>xxx
runbubblebuild:
	./obj/ebubblebuild 1>log/bubblebuildresult 2>xxx
