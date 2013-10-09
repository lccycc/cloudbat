make -j8

K=20
P=4
order=negetsmall

obj/main FOOTPRINT $K $P $order 1>log/sched.out
time obj/main REUSEDST $K $P $order
