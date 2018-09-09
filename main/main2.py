#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Created by Rafael S. Guimaraes and Diego Cardoso
# NERDS - Nucleo de Estudos em Redes Definidas por Software
#
import threading
import csv
import netaddr
import time
import sys
import os

from dpcontrol import *
from lib.bloomcl.entity.datapath import *

class BCLApp(object):

    def __init__(self):
        self.dp_control = DPControl()

    def show_result(self, cmd=0, fpath=None, rtimes=None):
        try:
            with open(fpath) as f:
                flines = f.readlines()
        except IOError as err:
            print("{}".format(err))
            return

        searchResult = ""
        vendor = ""
        if "-vendor" in sys.argv:
            idx = sys.argv.index("-vendor")
            vendor = sys.argv[idx + 1]

        fileNameStr = "EXEC" + cmd
        if (vendor!=""):
            fileNameStr += "-" + vendor
        fileNameStr += ".txt"

        fileObject = open(fileNameStr, 'w')
        if cmd == "opencl":
            for ip in flines:
                for t in range(int(rtimes)):
                    index_rt = self.dp_control.find_item_rt(ip.rstrip())
                    searchResult = str(ip.rstrip()) + "::" + str(index_rt.split("::")[1]) + "\n"
                    fileObject.write(searchResult)
                    # print("{}|{}|{}".format("normal", ip.rstrip(), index_rt))
        elif cmd == "normal":
            for ip in flines:
                for t in range(int(rtimes)):
                    index_rt = self.dp_control.find_item_rt_seq(ip.rstrip())
                    searchResult = str(ip.rstrip()) + "::" + str(index_rt.split("::")[1]) + "\n"
                    fileObject.write(searchResult)
                    # print("{}|{}|{}".format("normal", ip.rstrip(), index_rt))
        else:
            pass
        fileObject.close()

def main(fpath=None, cmd=0, rtimes=None):
    dp_main = BCLApp()
    print("Adding routes in BloomFilter Table and Router Table")
    time.sleep(60)
    dp_main.show_result(fpath=fpath, cmd=cmd, rtimes=rtimes)


def usage():
    print("\nBloomCL command\n")
    print('Usage: '+sys.argv[0]+' -f <file> -o <normal,opencl> -times <val>')


if __name__ == '__main__':
    if len(sys.argv) > 1:
        fpath = None
        opc = None
        rtimes = None
        try:
            if "-f" in sys.argv:
                idx = sys.argv.index("-f")
                fpath = sys.argv[idx + 1]
            if "-o" in sys.argv:
                idx = sys.argv.index("-o")
                opc = sys.argv[idx + 1]
            if "-times" in sys.argv:
                idx = sys.argv.index("-times")
                rtimes = sys.argv[idx + 1]
            main(fpath, opc, rtimes)
        except IndexError as err:
            usage()

    else:
        usage()
