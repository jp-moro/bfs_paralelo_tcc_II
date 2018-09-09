#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Created by Rafael S. Guimaraes and Diego Cardoso
# NERDS - Nucleo de Estudos em Redes Definidas por Software
#
import threading
import csv
import time
from lib.bloomcl.entity.datapath import *


class DPControl(object):
    """
        Main Class
    """
    def __init__(self):
        self._datapath = Datapath()
        self._datapath._tablesbf._generate_table()
        self._datapath._tablesrt._generate_table()
        self.__filename = 'conf/fib_v2-20062017.csv'
        self.is_finished_bf = False
        self.is_finished_rt = False
        # Insert route in BloomFilter table and Route Table
        self._t_add_i_bf = threading.Thread(
                    name="add item bloomfilter",
                    target=self.add_items_bf)
        self._t_add_i_rt = threading.Thread(
                    name="add item route table",
                    target=self.add_items_rt)
        self._t_add_i_bf.start()
        self._t_add_i_rt.start()

    def add_items_bf(self):
        print('{} starting'.format(threading.currentThread().getName()))
        with open(self.__filename, 'r') as f:
            reader = csv.reader(f, delimiter='|')
            for row in reader:
                self._datapath._tablesbf.add_item(row[0])
            #    if netaddr.valid_ipv4(row[1]):
            #        print "{}|{}".format(row[0], row[1])
        print('{} exiting'.format(threading.currentThread().getName()))
        self.is_finished_bf = True

    def add_items_rt(self):
        print('{} starting'.format(threading.currentThread().getName()))
        with open(self.__filename, 'r') as f:
            reader = csv.reader(f, delimiter='|')
            for row in reader:
                self._datapath._tablesrt.add_item(row[0], 1)
            #    if netaddr.valid_ipv4(row[1]):
            #        print "{}|{}".format(row[0], row[1])
        print('{} exiting'.format(threading.currentThread().getName()))
        self._datapath._tablesrt.clCreateBuffers()
        self.is_finished_rt = True

    def find_item_rt(self, ip=None):
        i_rt = []
        if ip:
            # find ip address in determinated bloomfilter
            lst_bf = self._datapath._tablesbf.find_ipaddress(ip)
            # Find index on each router table and
            # return this with the specific table
            start = time.clock()
            i_rt =  self._datapath._tablesrt.find_ipaddress(
                                        ipaddress=ip,
                                        tables=lst_bf)
            end = time.clock()
            return "{}::{}".format(str(i_rt), str(end-start))
        else:
            return i_rt

    def find_item_rt_seq(self, ip=None):
        i_rt = []
        if ip:
            # find ip address in determinated bloomfilter
            lst_bf = self._datapath._tablesbf.find_ipaddress(ip)
            # Find index on each router table and
            # return this with the specific table
            start = time.clock()
            i_rt =  self._datapath._tablesrt.find_ipaddress_seq(
                                        ipaddress=ip,
                                        tables=lst_bf)
            end = time.clock()
            return "{}::{}".format(str(i_rt), str(end-start))
        else:
            return i_rt


def main():
    bclmain = DPControl()
    time.sleep(60)
    res = bclmain.find_item_rt_seq('121.160.0.0')
    time.sleep(2)
    print res

if __name__ == '__main__':
    main()
