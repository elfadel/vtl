#!/bin/bash

set -e
set -u
set -o pipefail

rtt=0 		#ms
bw=0  		#Mbps
loss=0		#percentage

usage() { 
	echo "Usage: $0 [-u] [-i] [-d <RTT>] [-b <Bandwidth>] [-l <Loss rate>]" 1>&2; 
	exit 1; 
}

while getopts uid:b:l: o; do
	case $o in
		u)
			sudo tc qdisc delete root dev enp0s3
			sudo tc qdisc show dev enp0s3
			exit 1
			;;
		
		d) 
			rtt=$OPTARG
			;;
		b) 
			bw=$OPTARG
			;;
		l) 
			loss=$OPTARG
			;;
		i)
			sudo tc qdisc delete root dev enp0s3
			;;
		?)
			usage
			;;
	esac
done

let latency=$rtt+50

if [ $loss -eq 0 ]; then
	
	sudo tc qdisc add dev enp0s3 root handle 1:0 netem delay "$rtt"ms
	echo "RTT set !"
else
	sudo tc qdisc add dev enp0s3 root handle 1:0 netem delay "$rtt"ms loss "$loss"%
	echo "RTT and loss set !"
fi

sudo tc qdisc add dev enp0s3 parent 1:1 handle 10: tbf rate "$bw"mbit latency "$latency"ms burst 131032
echo "Bandwidth set !"


echo "Fin Config Network"
sudo tc qdisc show dev enp0s3