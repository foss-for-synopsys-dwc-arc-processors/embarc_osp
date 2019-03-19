.. _middleware_lwip:

lwIP
####

lwIP is a small independent implementation of the TCP/IP protocol suite,
initially developed by Adam Dunkels.

The focus of the lwIP TCP/IP implementation is to reduce resource usage while
still providing full-scale TCP. This makes lwIP suitable for use in embedded
systems with as little as 10s of KBs of free RAM and 40 KB of code ROM. Main
features include:

- Protocols: IP, ICMP, UDP, TCP, IGMP, ARP, PPPoS, PPPoE.
- DHCP client, DNS client, AutoIP/APIPA (Zeroconf), SNMP agent (private MIB
  support).
- APIs: specialized APIs for enhanced performance, optional Berkeley-like
  socket API.
- Extended features: IP forwarding over multiple network interfaces, TCP
  congestion control, RTT estimation, and fast recovery/fast retransmit.
- Add-on applications: HTTP server, SNTP client, SMTP client, ping, NetBIOS
  nameserver.

lwIP is licensed under a BSD-style license: https://lwip.fandom.com/wiki/License.