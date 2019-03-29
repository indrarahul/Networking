#!/usr/bin/gnuplot -persist
set terminal png
set output ARG2
set xlabel "Payload Size(in Bytes)"
set ylabel "Cum RTT"
set xrange [ 0 : 1000 ]
set yrange [ 0 : 500 ]
set datafile separator ","
plot ARG1 using 1:2
#pause -1
quit