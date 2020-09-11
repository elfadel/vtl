#!/bin/bash
echo "
Setting up the testbed network 
	+----------------------------------------+               +----------------------------------------+
	| End-point A: 192.168.130.158           |               |	     End-point B: 192.168.130.157 |
        |                                +-------+               +-------+				  |
        |				 | veth0 + <-----------> + veth1 |				  |
        |				 +-------+               +-------+				  |
        |				 	 |               |					  |
	+----------------------------------------+               +----------------------------------------+
"

echo "Cleaning previous testbed"
# sudo ip netns del ns1
sudo ip netns del ns2

## echo "Creating namespace 1 for End-point A"
## sudo ip netns add ns1
echo "Creating namespace 2 for End-point B"
sudo ip netns add ns2

## TODO: Retrieve the interface name on params
echo "Creating and peering the virtual interfaces veth0 and veth1"
sudo ip link add veth0 type veth peer name veth1

## echo "Placing veth0 on namespace ns1"
## sudo ip link set veth0 netns ns1
# echo "Placing veth1 on namespace ns2"
# sudo ip link set veth1 netns ns2

echo "Turning on veth0 interface"
sudo ip link set veth0 up
echo "Turning on veth1 interface"
sudo ip link set veth1 up
## sudo ip netns exec ns2 ip link set veth1 up

echo "Setting up interfaces ip addresses"
sudo ip addr add 192.168.130.158/24 dev veth0
sudo ip addr add 192.168.130.157/24 dev veth1
# sudo ip netns exec ns2 ip addr add 192.168.130.157/24 dev veth1
