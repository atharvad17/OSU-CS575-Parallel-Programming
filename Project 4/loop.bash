#!/bin/bash
for ArrSize in 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304
do
	g++ Project4.cpp -DARRAYSIZE=$ArrSize  -o Project4 -lm -fopenmp
	./Project4
done
