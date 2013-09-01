make -j8

K=20
P=4
for ((i=0;i<=4;i++)) do
	order=rand"$i";
	echo $order
	#time obj/main FREERUN $K $P $order
	#time obj/main NOPREDICTION $K $P $order
	time obj/main FOOTPRINT $K $P $order
	time obj/main REUSEDST $K $P $order
#	time obj/main BUBBLE $K $P $order
done;

order=bak
K=20
time obj/main FOOTPRINT $K $P $order
time obj/main REUSEDST $K $P $order
K=16
time obj/main FOOTPRINT $K $P $order
time obj/main REUSEDST $K $P $order
K=12
time obj/main FOOTPRINT $K $P $order
time obj/main REUSEDST $K $P $order
K=8
time obj/main FOOTPRINT $K $P $order
time obj/main REUSEDST $K $P $order
