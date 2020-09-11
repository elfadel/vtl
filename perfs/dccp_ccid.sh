#!/bin/bash

set -e
set -u
set -o pipefail

if [ $1 -eq 2 ]; then
	sudo modprobe dccp
	#sudo modprobe dccp_ccid2
	sudo sysctl -w net.dccp.default.seq_window=10000
	sudo sysctl -w net.dccp.default.rx_ccid=2
	sudo sysctl -w net.dccp.default.tx_ccid=2
else
	sudo modprobe dccp
	#sudo modprobe dccp_ccid3
	sudo sysctl -w net.dccp.default.seq_window=10000
	sudo sysctl -w net.dccp.default.rx_ccid=3
	sudo sysctl -w net.dccp.default.tx_ccid=3
fi