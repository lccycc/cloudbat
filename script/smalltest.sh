make -j8

K=20
P=2
order=smalltest

obj/main FOOTPRINT $K $P $order 1>log/sched.out
time obj/main REUSEDST $K $P $order
