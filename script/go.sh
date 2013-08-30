make -j8

#make freerun
#mv log/sched.msg log/freerun.K16
make runnoprediction
mv log/sched.msg log/nopred.K16
make runfootprintsched
mv log/sched.msg log/foot.K16
make runreusedstsched
mv log/sched.msg log/reuse.K16
make runbubblesched
mv log/sched.msg log/bubble.K16
