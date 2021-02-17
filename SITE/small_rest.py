
import http.server, urllib.parse, sqlite3
import requests
import json
import random
import time
import cgi


class MyHandler(http.server.BaseHTTPRequestHandler):
	def __init__(self, *args, **kwargs):
		super(MyHandler, self).__init__(*args, **kwargs)

	def do_GET(self):
		# if self.path == "/":
        #     res = urllib.parse.urlparse(self.path)
        #     query = urllib.parse.parse_qs(res.query)
        #     rep = self.mysql.insert(res.path,query)
		self.send_response(200)
		self.send_header("Content-type", "text/html")
		self.end_headers()



	def do_POST(self):

        # """Respond to a POST request."""

		if self.path == "/device":
		    print("request line",self.requestline)
		    print("headers",self.headers)
		    res = urllib.parse.urlparse(self.path)
		    query = urllib.parse.parse_qs(res.query)
		    print("res = ",res)
		    print("query = ",query)
		    print("reponse : ",json.loads(self.rfile.read(100)))
		    reponse = json.loads(self.rfile.read(100))
		    print("data = ",reponse['data'])
		    print("device = ",reponse['device'])
		    print("time = ",reponse['time'])

		    self.send_response(200)
		    self.send_header("Content-type", "text/html")
		    self.end_headers()



if __name__ == '__main__':
	server_class = http.server.HTTPServer
	httpd = server_class(("localhost", 8888), MyHandler)
	try:
		httpd.serve_forever()
	except KeyboardInterrupt:
		pass
	httpd.server_close()
