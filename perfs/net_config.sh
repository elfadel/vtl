#!/bin/bash

set -e
set -u
set -o pipefail

rtt=0 		#ms
bw=0  		#Mbps
loss=0		#percentage
iface=""

usage() { 
	echo "Usage: $0 [-u] [-r] [-i <iface>] [-d <RTT>] [-b <Bandwidth>] [-l <Loss rate>]" 1>&2; 
	exit 1; 
}

while getopts uri:d:b:l: o; do
	case $o in
		u)
			sudo tc qdisc delete root dev $iface
			sudo tc qdisc show dev $iface
			exit 1
			;;
		r)
			sudo tc qdisc delete root dev $iface
			;;
		i)
			iface=$OPTARG
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
		?)
			usage
			;;
	esac
done

let latency=$rtt+50

if [ $loss -eq 0 ]; then
	
	sudo tc qdisc add dev $iface root handle 1:0 netem delay "$rtt"ms
	echo "RTT set !"
else
	sudo tc qdisc add dev $iface root handle 1:0 netem delay "$rtt"ms loss "$loss"%
	echo "RTT and loss set !"
fi

sudo tc qdisc add dev $iface parent 1:1 handle 10: tbf rate "$bw"mbit latency "$latency"ms burst 131032
echo "Bandwidth set !"


echo "Fin Config Network"
sudo tc qdisc show dev $iface