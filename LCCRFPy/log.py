#!/usr/bin/env python
#coding:utf-8

import sys
import logging

import socket
myname = socket.getfqdn(socket.gethostname())
myaddr = socket.gethostbyname(myname)

logger = logging.getLogger()
hdlr = logging.StreamHandler(sys.stderr)
fmt = "%(asctime)s [%(levelname)-8s] [{0}] %(message)s".format(myname)
formatter = logging.Formatter( fmt, "%m-%d %H:%M:%S")
hdlr.setFormatter(formatter)
logger.addHandler(hdlr)
logger.setLevel(logging.NOTSET)

class log:   
    @staticmethod
    def info(msg):
        global logger
        logger.info(msg)
        #print >> sys.stderr, msg
        #sys.stdout.flush()

    @staticmethod
    def warning(msg):
        global logger
        logger.warning(msg)
        
    @staticmethod
    def debug(msg):
        global logger
        logger.debug(msg)
