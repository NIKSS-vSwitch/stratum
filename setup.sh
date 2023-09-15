#!/usr/bin/env bash
#sudo python3 -m pip install --upgrade pip
#pip3 install scapy

# Adding ports
ip link add veth1 type veth peer name veth2
ip link add veth3 type veth peer name veth4

# Adding IP addresses to ports
ip addr add 192.168.2.102 dev veth2
ip addr add 192.168.2.104 dev veth4

# Changing mac addresses
ip link set dev veth2 address 00:00:00:00:01:02
ip link set dev veth4 address 00:00:00:00:01:04

# Setting up ports
ip link set veth1 up
ip link set veth2 up
ip link set veth3 up
ip link set veth4 up

# Run stratum with chassis_config file
bazel-bin/stratum/hal/bin/nikss/stratum_nikss -chassis_config_file stratum/hal/bin/nikss/chassis_config.pb.txt 

# Cleaning
nikss-ctl del-port pipe 1 dev veth1
nikss-ctl del-port pipe 1 dev veth3
nikss-ctl pipeline unload id 1
