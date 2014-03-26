#!/usr/bin/env python2
#
###################################################
# Ponykart Matchmaking Server Prototype
# 
# To Do: zlib compression
# 			 IPv6 support
#				 Client reporting
#
###################################################
import socket
import sys
import string
import zlib
import redis
from thread import *

# define networking info
HOST = ''
PORT = 8888

# define redis info
db = redis.StrictRedis(host='127.0.0.1', port=6379, db=0) 

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print 'Socket Created'

# bind socket to host and port. report any error(s) and exit upon failure
try:
	s.bind((HOST, PORT))
except socket.error as msg:
	print 'Bind failed. Error Code: (' + str(msg[0]) + ') ' + msg[1]
	sys.exit()
	
print 'Socket bound successfully'

# start listening for incoming connections
s.listen(10)
print 'Listening for connections'

# Define function for handling connections
def clientthread(conn):
	while True:
		data = string.strip(conn.recv(1024))
		req = data.split(':')
		if not data:
			break
		else:
			if req[0] == 'SRV_EST':
				print 'Server established request from: ' + addr[0]
				srv = addr[0] + ':' + req[1]
				db.sadd('server', srv)
			elif req[0] == 'SRV_DST':
				print 'Server removal request from: ' + addr[0]
				srv = addr[0] + ':' + req[1]
				db.srem('server', srv)
			elif req[0] == 'CLT_REQ':
				print 'Client request from: ' + addr[0]
				lst = db.smembers('server')
				# conn.send requires either string or buffer. lst is a set
				# need to convert lst to buffer and compress with zlib
				# then send the results
				conn.send(lst)
			else:
				print 'Malformed request from: ' + addr[0]
	
	conn.close()
		
while 1:
	conn, addr = s.accept()
	print 'Connection from ' + addr[0] + ':' + str(addr[1])
	
	start_new_thread(clientthread ,(conn,))
	
s.close()