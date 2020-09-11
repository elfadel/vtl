#!/bin/bash

set -e
set -u
set -o pipefail


sudo sysctl -w net.ipv4.tcp_congestion_control=$1

#sudo sysctl -a | grep tcp_congestion_control