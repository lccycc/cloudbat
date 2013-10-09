make -j8
P=4
K=20
for ((i=0;i<=4;i++)); do
	order=rand2v$i
	echo $order
	obj/main NOPREDICTION $K $P $order
	obj/main FOOTPRINT $K $P $order
	order=rand4v$i
	echo $order
	obj/main NOPREDICTION $K $P $order
	obj/main FOOTPRINT $K $P $order
done
