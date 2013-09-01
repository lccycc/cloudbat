make -j8

K=20
P=4
order=rand1

for ((i=0;i<=4;i++)) do
	order=rand"$i"_nomcf;
	echo $order
	#time obj/main FREERUN $K $P $order
	time obj/main NOPREDICTION $K $P $order
	time obj/main FOOTPRINT $K $P $order
#	time obj/main REUSEDST $K $P $order
#	time obj/main BUBBLE $K $P $order
done;
