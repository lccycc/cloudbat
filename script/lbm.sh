make -j8

P=4
#order=lbmtail8
order=lbm
K=20
echo $K
time obj/main FOOTPRINT $K $P $order
time obj/main BUBBLE $K $P $order
#time obj/main FOOTPRINT $K $P $order
#time obj/main BUBBLE $K $P $order
#time obj/main NOPREDICTION $K $P $order

#order=lbm6
#time obj/main FOOTPRINT $K $P $order
#time obj/main BUBBLE $K $P $order
#time obj/main NOPREDICTION $K $P $order
