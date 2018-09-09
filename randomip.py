#!/usr/bin/env python
import socket
import struct
import random
import math
import sys

file = open ("randomip.txt", "w")

def get_max_ips( netmask_length, random_ips ):
    # define maximum number of IP addresses to be generated
    netmask_ips = int( math.pow( 2, 32-netmask_length ) )
    if ( netmask_ips < random_ips ):
        winner = int( math.pow( 2, 32-netmask_length ) )
    else:
        winner = random_ips
    return winner

def get_random_ip_in_subnet( ip, netmask_length ):
    # thanks to Chad Bibler, http://dregsoft.com/blog/?p=24
    # change the IP address into its bytes so we can add a number to it
    n = socket.inet_aton( ip )
    ip_bytes = struct.unpack( '!i', n )[0]
    # we use the netmask_length to get a range of numbers we can add to the IP address
    max_to_add = int( math.pow( 2, 32-netmask_length ) )
    to_add = random.randrange( 0, max_to_add )
    ip_bytes += to_add
    # pack it back up
    and_back = struct.pack( '!i', ip_bytes )
    new_ip = socket.inet_ntoa( and_back )
    return new_ip

if ( len(sys.argv) != 4 ):
    print "Command syntax: "
    print "   python randomip.py   <# of random IPs from subnet>"
    print ""
    print "   example: python randomip.py 192.168.0.0 20 100"
else:
    iplist={}
    max_ips=get_max_ips(int(sys.argv[2]),int(sys.argv[3]))
    for x in range(0, max_ips):
        ip=get_random_ip_in_subnet( sys.argv[1], int(sys.argv[2]))
        while(iplist.has_key(ip)):
            ip=get_random_ip_in_subnet( sys.argv[1], int(sys.argv[2]))
        iplist[ip]=1
       # print ip

        line = ip + '\n'

        file.write(line)


file.close()