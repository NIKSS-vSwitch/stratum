#!/usr/bin/env bash
#sudo python3 -m pip install --upgrade pip
#pip3 install scapy

#dodawanie par interfejsow
ip link add veth1 type veth peer name veth2
ip link add veth3 type veth peer name veth4

#dodawanie adresow ip do veth
ip addr add 192.168.2.101 dev veth1 #zbedne
ip addr add 192.168.2.102 dev veth2
ip addr add 192.168.2.103 dev veth3 #zbedne
ip addr add 192.168.2.104 dev veth4

#zmiana adresow mac
ip link set dev veth1 address 00:00:00:00:01:01 #zbedne
ip link set dev veth2 address 00:00:00:00:01:02
ip link set dev veth3 address 00:00:00:00:01:03 #zbedne
ip link set dev veth4 address 00:00:00:00:01:04

#wlaczanie wirtualnych interfejsow
ip link set veth1 up
ip link set veth2 up
ip link set veth3 up
ip link set veth4 up

#dodawanie portow
#nikss-ctl add-port pipe 1 dev veth1
#nikss-ctl add-port pipe 1 dev veth3
#nikss-ctl pipeline show id 1

#bash

#szybki reset - tymczasowe
#nikss-ctl table delete pipe 1 ingress_tbl_switching
#nikss-ctl multicast-group delete pipe 1 id 2

#dodawnie grupy multicast

#tymczasowo
#nikss-ctl multicast-group create pipe 1 id 2
#nikss-ctl multicast-group add-member pipe 1 id 2 egress-port 3 instance 0
#nikss-ctl multicast-group add-member pipe 1 id 2 egress-port 5 instance 0
#nikss-ctl multicast-group get pipe 1 id 2

#nikss-ctl table add pipe 1 ingress_tbl_switching action name ingress_forward key 00:00:00:00:01:02 0 data 3

#nikss-ctl table add pipe 1 ingress_tbl_switching action name ingress_forward key 00:00:00:00:01:04 0 data 5

#run stratum
bazel-bin/stratum/hal/bin/nikss/stratum_nikss -chassis_config_file stratum/hal/bin/nikss/chassis_config.pb.txt 

nikss-ctl del-port pipe 1 dev veth1
nikss-ctl del-port pipe 1 dev veth3

nikss-ctl pipeline unload id 1

#uruchamianie skryptu
#python3 script.py

#nikss-ctl pipeline unload id 1
