make -j8

K=12
P=4
order=bak

#time obj/main FREERUN $K $P $order
time obj/main NOPREDICTION $K $P $order
time obj/main FOOTPRINT $K $P $order
time obj/main REUSEDST $K $P $order
time obj/main BUBBLE $K $P $order

K=8
time obj/main FREERUN $K $P $order
time obj/main NOPREDICTION $K $P $order
time obj/main FOOTPRINT $K $P $order
time obj/main REUSEDST $K $P $order
time obj/main BUBBLE $K $P $order

K=20
time obj/main FREERUN $K $P $order
time obj/main NOPREDICTION $K $P $order
time obj/main FOOTPRINT $K $P $order
time obj/main REUSEDST $K $P $order
time obj/main BUBBLE $K $P $order
