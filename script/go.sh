make -j8

K=14
P=4
order=bak

time obj/main FREERUN $K $P $order
time obj/main NOPREDICTION $K $P $order
time obj/main FOOTPRINT $K $p $order
time obj/main REUSEDST $K $p $order
time obj/main BUBBLE $K $p $order

K=10
time obj/main FREERUN $K $P $order
time obj/main NOPREDICTION $K $p $order
time obj/main FOOTPRINT $K $p $order
time obj/main REUSEDST $K $p $order
time obj/main BUBBLE $K $p $order
