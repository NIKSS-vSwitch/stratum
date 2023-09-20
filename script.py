from scapy.all import *
from scapy.layers.inet import IP, UDP
from scapy.layers.l2 import Dot1Q, Ether

sendp(Ether(dst="00:00:00:00:01:04")/IP(dst="192.168.2.104",ttl=(1,4)), iface="veth2")
