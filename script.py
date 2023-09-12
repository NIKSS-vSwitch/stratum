from scapy.all import *
from scapy.layers.inet import IP, UDP
from scapy.layers.l2 import Dot1Q, Ether

sendp(Ether(dst="00:00:00:00:01:04")/IP(dst="192.168.2.104",ttl=(1,4)), iface="veth2")


#pkt = Ether(src=pkt[Ether].src, dst=pkt[Ether].dst)
#        / Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
#        / pkt[Ether].payload


#pkt = testutils.simple_udp_packet(eth_dst="00:00:00:00:00:03")
#pkt = pkt_add_vlan(pkt, vlan_vid=1)

#testutils.send_packet(self, PORT1, pkt) #sendp - scappy
#pkt[Ether].dst = "00:00:00:00:00:02"
#testutils.send_packet(self, PORT1, pkt)pip 
#pkt[Ether].dst = "00:00:00:00:00:01"
#testutils.send_packet(self, PORT2, pkt)


#tcpdump na drugim interfejsie
