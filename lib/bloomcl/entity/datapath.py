#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Created by Rafael S. Guimaraes and Diego Cardoso
# NERDS - Nucleo de Estudos em Redes Definidas por Software
#

import sys
from pybloom import BloomFilter
import numpy as np
import netaddr
import pyopencl as cl
from lib.bloomcl.kernel.match import KERNEL_MATCH


class TableDefault(object):
    """
        Generic class for storage routes in the datapath
        (Abstract Functions)
    """
    def generate_tables(self):
        raise Exception("Method not implemented!")

    def add_item(self, route=None):
        raise Exception("Method not implemented!")

    def find_ipaddress(self, ipaddress=None):
        raise Exception("Method not implemented!")


class TableBloomFilter(TableDefault):
    """
        This class will storage the 32 tables to perform
        probabilistic match function
    """
    def __init__(self, capacity=500000, error_rate=0.001):
        self._table = []
        self.__capacity = capacity
        self.__error_rate = error_rate

    def _generate_table(self):
        """
            This function will generate 32 bloom filter tables
        """
        for i in xrange(32):
            self._table.append(
                BloomFilter(
                    capacity=self.__capacity,
                    error_rate=self.__error_rate
                    )
                )

    def add_item(self, route=None):
        if route:
            try:
                network = netaddr.IPNetwork(route)
                mask = int(network.netmask)
                ipaddress = int(network.ip)
                self._table[network.prefixlen - 1].add(int(ipaddress & mask))
            except Exception, ex:
                print("Error to insert new route: {}".format(str(ex)))

    def find_ipaddress(self, ipaddress=None):
        if ipaddress:
            res = []
            try:
                network = netaddr.IPNetwork(ipaddress)
                for m in reversed(xrange(32)):
                    network.prefixlen = (m + 1)
                    ip = int(network.network)
                    if ip in self._table[m]:
                        res.append(m+1)
            except Exception, ex:
                return res

            return res


class TableRouter(TableDefault):
    """
        This class will storage the 32 tables to perform the correct match
        for eatch table depending of its location.
    """

    def __init__(self, capacity=0, vendor='intel'):
        self._table = []
        self.__capacity = capacity
        if "-vendor" in sys.argv:
            idx = sys.argv.index("-vendor")
            vendor = sys.argv[idx + 1]
        platform = None
        platforms = cl.get_platforms()
        for p in platforms:
            if vendor in p.name.lower():
                platform = p
                break
        dev_type = "GPU"
        device = platform.get_devices(device_type=cl.device_type.GPU)
        if len(device) == 0:
            dev_type = "CPU"
            device = platform.get_devices(device_type=cl.device_type.CPU)
        # print('##### Platform: {p.name}  |  Device: {dt} {d.name} #####'
        #       .format(p=platform, d=device[0], dt=dev_type))
        self.ctx = cl.Context(device)
        self.queue = cl.CommandQueue(self.ctx, properties=cl.command_queue_properties.PROFILING_ENABLE)
        self.program = cl.Program(self.ctx, KERNEL_MATCH).build()

        self.event = None
        self.table_buf = []
        self.tableShape = []
        self.pos = np.array(-1, dtype=np.int32)
        self.pos_buf = None

    def _generate_table(self):
        """
            This function will generate 32 router tables
        """
        for i in xrange(32):
            dest = [0]
            gw = [0]
            self._table.append(
                {'destination': dest, 'gateway': gw}
            )

    def add_item(self, route=None, gateway=None):
        if route:
            try:
                network = netaddr.IPNetwork(route)
                mask = int(network.netmask)
                ipaddress = int(network.ip)
                index = network.prefixlen - 1
                self._table[index]['destination'].append(
                        int(ipaddress & mask)
                    )
            except Exception, ex:
                print("Error to insert a new route: {}".format(str(ex)))

    def find_ipaddress(self, ipaddress=None, tables=None):
        if ipaddress and tables:
            res = []
            try:
                key = netaddr.IPNetwork(ipaddress)
                #print key
                #print tables
                res = self.clFindRoute(key, tables)
                return res
            except Exception, ex:
                return res

            return res

    def find_ipaddress_seq(self, ipaddress=None, tables=None):
        if ipaddress and tables:
            res = []
            try:
                key = netaddr.IPNetwork(ipaddress)
                for mask in tables:
                    key.prefixlen = (mask)
                    ip = int(key.network)
                    cnt = 0
                    end = False
                    for prefix in self._table[mask - 1]['destination']:
                        if (ip ^ prefix) == 0:
                            res = [mask, cnt + 1]
                            end = True
                            break
                        cnt = cnt + 1
                    if end:
                        break
                return res
            except Exception, ex:
                return res

            return res

    def clCreateBuffers(self):
        mf = cl.mem_flags
        for i in xrange(32):
            try:
                aux_np = np.array(self._table[i]['destination'], np.uint32)
                buffer = cl.Buffer(self.ctx, mf.READ_ONLY | mf.COPY_HOST_PTR, hostbuf=aux_np)
                #buffer = cl.Buffer(self.ctx, mf.READ_WRITE, aux_np.nbytes)
                cl.enqueue_write_buffer(self.queue, buffer, aux_np)
                self.table_buf.append(buffer)
                self.tableShape.append(aux_np.shape)
            except Exception, ex:
                print "{}".format(ex)


        self.pos_buf = cl.Buffer(self.ctx, mf.READ_WRITE, self.pos.nbytes)
        cl.enqueue_write_buffer(self.queue, self.pos_buf, self.pos)

    def clFindRoute(self, key, candidateTableList):
        timeKernelHash = 0
        self.pos = np.array(-1, dtype=np.int32)

        cl.enqueue_write_buffer(self.queue, self.pos_buf, self.pos)

        for elem in candidateTableList:
            key.prefixlen = (elem)
            ip = int(key.network)

            event = self.program.match(self.queue,
                                   self.tableShape[elem-1],
                                   None,
                                   self.table_buf[elem-1],
                                   self.pos_buf,
                                   np.int32(ip)
                                   )
            event.wait()
            cl.enqueue_read_buffer(self.queue, self.pos_buf, self.pos)
            timeKernelHash += event.profile.end - event.profile.start
            if (self.pos != -1):
                break
        # print("Measured Time kernel Hash (eventProfiler OpenCL function): {:5.8f}"
        #       .format(1e-9*timeKernelHash))
        return [elem, self.pos]

class Datapath(object):
    def __init__(self, **kwargs):
        self._tablesbf = TableBloomFilter()
        self._tablesrt = TableRouter()

if __name__ == "__main__":
    tb = TableRouter()
    tb._generate_table()
    tb.clCreateBuffers()
