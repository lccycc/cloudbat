make runnoprediction
mv log/sched.msg log/nopred.msg_neg
make runfootprintsched
mv log/sched.msg log/foot.msg_neg
make runreusedstsched
mv log/sched.msg log/reuse.msg_neg
make runbubblesched
mv log/sched.msg log/bubble.msg_neg
