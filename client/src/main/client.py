import socket
import argparse

PORT = 60085

def get_parser():
	parser = argparse.ArgumentParser(description="Make call to po node")
	parser.add_argument("method", choices=["PUSH","PULL"])
	parser.add_argument("--to", dest="to",help="destination ip")
	parser.add_argument("--msg", dest="msg",help="message contents")

	return parser

def main():
	args = get_parser().parse_args()
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect(("localhost",PORT))

	if args.method == "PULL":
		sock.send("PULL")
		resp = sock.recv(2048)
		print resp
		if resp == "FAIL":
			print "Pull failed"
		num_msg = int(resp[8:])
		for _i in range(num_msg):
			print sock.recv(2048)
		# TODO: Parse messages to print out nicely
		# TODO: if queue is too big, make successive reads
		# TODO: Ensure we have gotten all messages

	elif args.method == "PUSH":
		request_str = "PUSH " + "IP:" + args.to + " MSG:" + args.msg
		print request_str
		sock.send(request_str)
		resp = sock.recv(2048)
		print "Response %s" % (resp)

if __name__ == "__main__":
	main()