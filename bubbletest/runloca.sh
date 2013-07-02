#!/bin/sh
export loca=/home/lcc/loclab/gem/lib/loca 
export oridatalog=/home/lcc/.loca/data/
export data=./data/
for i in {1..10};do
    $loca run ./eup $i
    mv $oridatalog/*.dat $data/eup_$i.dat
done
for i in {1..10};do
    $loca run ./estream $i
    mv $oridatalog/*.dat $data/estream_$i.dat
done

