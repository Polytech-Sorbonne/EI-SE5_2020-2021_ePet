# curl -X GET http://localhost:8888/Etudiant
# curl -X POST http://localhost:8888/Etudiant/\?Nom\=Cionaire\&Prenom\=Dick\&idAd\=2

import http.server, urllib.parse, sqlite3
import requests
import random
import time

ENDPOINT = "things.ubidots.com"
DEVICE_LABEL = "epet"
VARIABLE_LABEL = "temperature"
TOKEN = "BBFF-7AY67IDc1sw2hLeaILMxUMTdHHkdBr"
DELAY = 5  # Delay in seconds

class MyHandler(http.server.BaseHTTPRequestHandler):
	def __init__(self, *args, **kwargs):
		self.mysql = MySQL('epet.db')
		super(MyHandler, self).__init__(*args, **kwargs)

	def do_GET(self):
		"""Respond to a GET request."""

		if self.path == '/favicon.ico':
			return

		res = urllib.parse.urlparse(self.path)
		rep = ""
		#rep = self.mysql.select(res.path)

		"""if self.path == "/User":
			rep = self.mysql.select_user(res.path)
		"""

		"""if len(rep) > 0:
			self.mysql.send_data()

			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()
			self.wfile.write(bytes(str(rep)+'\n', 'UTF-8'))
		else:
			self.send_response(404)
			self.send_header("Content-type", "text/html")
			self.end_headers()
		"""

		if self.path == "/Accueil":
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()
			with open('accueil.html', 'r') as f:
				html = f.read()
				self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))

		if self.path == "/Temp":
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()

			with open('temperature_debut.html', 'r') as f:
				my_str = f.read()

			s = self.mysql.temperature_html()

			print("S")
			print(s)

			print("S 0 1")
			print(s[0][1])

			for i in range(len(s)):

				my_str = my_str + "          ['"
				my_str = my_str + str(s[i][0]) + "', " + str(s[i][1])
				my_str = my_str + "],\n"

			my_str = my_str + "]);\n"


			with open('temperature_fin.html', 'r') as f:
				my_str = my_str + f.read()
			self.wfile.write(bytes(str(my_str)+'\n', 'UTF-8'))

			tmp = open("affichage_temp_finale.html", "w")
			tmp.write(my_str)

	def do_POST(self):
		"""Respond to a POST request."""
		res = urllib.parse.urlparse(self.path)
		query = urllib.parse.parse_qs(res.query)
		rep = self.mysql.insert(res.path,query)
		self.send_response(200)
		self.send_header("Content-type", "text/html")
		self.end_headers()

class MySQL():
	def __init__(self, name):
		self.c = None
		self.req = None
		self.conn = sqlite3.connect(name)
		self.c = self.conn.cursor()

	def __exit__(self, exc_type, exc_value, traceback):
		self.conn.close()

	def select(self,path):
		elem = path.split('/')
		print("LEN= ", len(elem))
		print("ELEM =", elem)

		if len(elem) == 2:
			req = "select * from %s" %(elem[1])
			print(req)
		else:
			req = "select %s from %s where id=%s" %(elem[3],elem[1],elem[2])
		return self.c.execute(req).fetchall()

	def select_user(self,path):
		req = "select * from User"
		print(req)
		return self.c.execute(req).fetchall()

	def temperature_html(self):
		req = "select date_insert, temp from Temperature;"
		s = self.c.execute(req).fetchall()
		return s

	def insert(self,path,query):
		print(query)
		attr = ', '.join(query.keys())
		val = ', '.join('"%s"' %v[0] for v in query.values())
		print(attr,val)
		req = "insert into %s (%s) values (%s)" %(path.split('/')[1], attr, val)
		print(req)
		self.c.execute(req)
		self.conn.commit()

	def send_data(self):
		print("On entre dans le main")
		# Simulates sensor values
		sensor_value = random.randrange(0, 50, 1) + random.random()

		if sensor_value > 39 or sensor_value < 38:
			context_value = "sickpet"
		else:
			context_value = "healthypet"

		# Builds Payload and topíc
		payload = {VARIABLE_LABEL: {"value": sensor_value,"context": {"status": context_value}}}

		post_var(payload)

def post_var(payload, url=ENDPOINT, device=DEVICE_LABEL, token=TOKEN):
    try:
        url = "http://{}/api/v1.6/devices/{}".format(url, device)
        headers = {"X-Auth-Token": token, "Content-Type": "application/json"}

        attempts = 0
        status_code = 400

        while status_code >= 400 and attempts < 5:
            print("[INFO] Sending data, attempt number: {}".format(attempts))
            req = requests.post(url=url, headers=headers,
                                json=payload)
            status_code = req.status_code
            attempts += 1
            time.sleep(1)

        print("[INFO] Results:")
        print(req.text)
    except Exception as e:
        print("[ERROR] Error posting, details: {}".format(e))

    except Exception as e:
        print("[ERROR] Error posting, details: {}".format(e))



if __name__ == '__main__':
	server_class = http.server.HTTPServer
	httpd = server_class(("localhost", 8888), MyHandler)
	try:
		httpd.serve_forever()
	except KeyboardInterrupt:
		pass
	httpd.server_close()
