#!/usr/bin/env python2
import socket
import sys
import string
import zlib
import redis
from thread import *

###############################################################
# Networking information
# 
# HOST should be set to the IP you wish to bind to
# PORT 
# IP should be set to v4 or v6 respectively
#
###############################################################

HOST = ''
PORT = 8888
IP = 'v6'

# define redis info
db = redis.StrictRedis(host='127.0.0.1', port=6379, db=0) 

if IP == 'v4':
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	print 'IPv4 Socket Created'
elif IP == 'v6':
	s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
	print 'IPv6 Socket Created'
else:
	print 'Failed to create socket. Please check your IP variable'
	sys.exit()

# bind socket to host and port. report any error(s) and exit upon failure
try:
	s.bind((HOST, PORT))
except socket.error as msg:
	print 'Bind failed. Error Code: (' + str(msg[0]) + ') ' + msg[1]
	sys.exit()
	
print 'Socket bound successfully'

# start listening for incoming connections
s.listen(5)
print 'Listening for connections'

# Define function for handling connections
def clientthread(conn):
	while True:
		data = string.strip(conn.recv(1024))
		req = data.split(';')
		if not data:
			break
		else:
			if req[0] == 'SRV_EST':
				print 'Server established request from: ' + addr[0]
				srv = '[' + addr[0] + ']:' + req[1]
				db.sadd('server', srv)
			elif req[0] == 'SRV_REM':
				print 'Server removal request from: ' + addr[0]
				srv = '[' + addr[0] +']:' + req[1]
				db.srem('server', srv)
			elif req[0] == 'CLT_REQ':
				print 'Client request from: ' + addr[0]
				lst = db.smembers('server')
				strlst = ""
				for n in lst:
					strlst = strlst + ';' + n
				conn.send(strlst)
				#conn.send(zlib.compress(strlst))
				del strlst
			else:
				print 'Malformed request from: ' + addr[0]
	conn.close()
		
while 1:
	conn, addr = s.accept()
	print 'Connection from [' + addr[0] + ']:' + str(addr[1])
	
	start_new_thread(clientthread ,(conn,))
	
s.close()