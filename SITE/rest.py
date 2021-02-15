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

id_utilisateur = -1
id_animal = -1

class MyHandler(http.server.BaseHTTPRequestHandler):
	def __init__(self, *args, **kwargs):
		self.mysql = MySQL('epet.db')
		super(MyHandler, self).__init__(*args, **kwargs)

	def do_GET(self):
		global id_animal
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

		if self.path == "/Log" :
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()
			with open('log.html', 'r') as f:
				html = f.read()
				self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))
				#res = urllib.parse.urlparse(self.path)
				#rep = self.mysql.select_user(res.path)
				#print('rep')

				#for i in range(len(rep)):
					#print(rep[i][2]) #le nom du User
					#print(rep[i][1]) #le mot de passe correspondant




		if self.path[0:5] == "/Temp":
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()
			try:
				id_animal = int(self.path[6:]) + 1

			except:
				id_animal = 1


			with open('temperature_debut.html', 'r') as f:
				my_str = f.read()

			s = self.mysql.temperature_html()

			print("S")
			print(s)

			for i in range(len(s)):

				my_str = my_str + "          ['"
				print("SI = ", s[i])
				my_str = my_str + str(s[i][0]) + "', " + str(s[i][1]) #date d'insertion et température
				my_str = my_str + "],\n"

			my_str = my_str + "]);\n"


			with open('temperature_suite.html', 'r') as f:
				my_str = my_str + f.read()

			a = self.mysql.select_animals()

			for i in range(len(a)):
				my_str = my_str + '<a href="/Temp-' + str(i) + '" <button>' + a[i][0] + ' </button></a>'
				#affichage noms des animaux


			with open('temperature_fin.html', 'r') as f:
				my_str = my_str + f.read()

			self.wfile.write(bytes(str(my_str)+'\n', 'UTF-8'))

			tmp = open("affichage_temp_finale.html", "w")
			tmp.write(my_str)

		if self.path == "/select_animal":
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()

			a = self.mysql.select_animals()
			print('a')
			print(a)

		if self.path[0:4] == "/Loc":
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()

			try:
				id_animal = int(self.path[5:]) + 1

			except:
				id_animal = 1

			with open('localisation_debut.html', 'r') as f:
				my_str = f.read()

			s = self.mysql.localisation_html()

			loc = self.mysql.select_loc()

			print("LOC:",loc)

			y0 = loc[0]
			x0 = loc[1]

			y1 = s[1]
			x1 = s[2]

			y2 = s[3]
			x2 = s[4]

			y3 = s[5]
			x3 = s[6]


			long = str(x0)
			lat = str(y0)
			api = open("key.txt", "r")
			lignes = api.read()
			print('lignes',lignes)
			lignes = lignes.rstrip('\n')


			print("classe = ", type(lignes).__name__)



			with open('localisation_suite.html', 'r') as f:
				my_str = my_str + f.read()

			a = self.mysql.select_animals()
			for i in range(len(a)):
				my_str = my_str + '<a href="/Loc-' + str(i) + '" <button>' + a[i][0] + ' </button></a>'
				#affichage noms des animaux

			with open('localisation_suite_2.html', 'r') as f:
				my_str = my_str + f.read()

			my_str += lat + "," + long + "], 11);\nL.tileLayer('https://api.mapbox.com/styles/v1/{id}/tiles/{z}/{x}/{y}?access_token=" +str(lignes)+"', {\n\tmaxZoom: 18,\n\tattribution: 'Map data &copy; <a href=\"https://www.openstreetmap.org/copyright\">OpenStreetMap</a> contributors, ' + 'Imagery © <a href=\"https://www.mapbox.com/\">Mapbox</a>',\n\tid: 'mapbox/streets-v11',\n\ttileSize: 512,\n\tzoomOffset: -1\n}).addTo(mymap);\nL.marker([" + lat + "," + long + "]).addTo(mymap)\nL.polygon([[" + str(y1) + "," + str(x1) + "],[" + str(y2) + "," + str(x2) + "],[" + str(y3) + "," + str(x3) + "]]).addTo(mymap);"


			with open('localisation_fin.html', 'r') as f:
				my_str += f.read()

			self.wfile.write(bytes(str(my_str)+'\n', 'UTF-8'))

			tmp = open("config_test.html", "w")
			tmp.write(my_str)





	def do_POST(self):
		global id_utilisateur
		global id_animal
		# """Respond to a POST request."""
		# res = urllib.parse.urlparse(self.path)
		# query = urllib.parse.parse_qs(res.query)
		# rep = self.mysql.insert(res.path,query)
		# self.send_response(200)
		# self.send_header("Content-type", "text/html")
		# self.end_headers()



		if self.path[0:7] =="/device":
			res = urllib.parse.urlparse(self.path)
			query = urllib.parse.parse_qs(res.query)

			print("query")
			print(query)
			print("device")
			print(query['device'])
			print("time")
			print(query['time'])
			print("data")
			print(query['data'])

			loc_mode= query['data'][0][18:20]

			if(loc_mode==0x0):
				bssid = twos_complement(query['data'][0][0:12],48)
				rssi = twos_complement(query['data'][0][12:14],8)
				print('rssi',query['data'][0][12:14])
				print('bssid',query['data'][0][0:2]+':'+query['data'][0][2:4]+':'+query['data'][0][4:6]+':'+query['data'][0][6:8]+':'+query['data'][0][8:10]+':'+query['data'][0][10:12])
			elif(loc_mode==0x1):
				distx = twos_complement(query['data'][0][0:12],48)
				print('distx',query['data'][0][0:2]+':'+query['data'][0][2:4]+':'+query['data'][0][4:6]+':'+query['data'][0][6:8]+':'+query['data'][0][8:10]+':'+query['data'][0][10:12])
			elif(loc_mode==0x2):
				disty = twos_complement(query['data'][0][0:12],48)
				print('disty',query['data'][0][0:2]+':'+query['data'][0][2:4]+':'+query['data'][0][4:6]+':'+query['data'][0][6:8]+':'+query['data'][0][8:10]+':'+query['data'][0][10:12])
			elif(loc_mode==0x3):
				bssid = twos_complement(query['data'][0][0:12],48)
				rssi = twos_complement(query['data'][0][12:14],8)
				temp = twos_complement(query['data'][0][14:18],16)
				print('bssid',query['data'][0][0:2]+':'+query['data'][0][2:4]+':'+query['data'][0][4:6]+':'+query['data'][0][6:8]+':'+query['data'][0][8:10]+':'+query['data'][0][10:12])
				temp = float(temp/100)
				print('temp',temp)
			elif(loc_mode==0x4):
				distx = twos_complement(query['data'][0][0:12],48)
				temp = twos_complement(query['data'][0][14:18],16)
				print('distx',query['data'][0][0:2]+':'+query['data'][0][2:4]+':'+query['data'][0][4:6]+':'+query['data'][0][6:8]+':'+query['data'][0][8:10]+':'+query['data'][0][10:12])
				temp = float(temp/100)
				print('temp',temp)
			elif(loc_mode==0x5):
				disty = twos_complement(query['data'][0][0:12],48)
				temp = twos_complement(query['data'][0][14:18],16)
				print('disty',query['data'][0][0:2]+':'+query['data'][0][2:4]+':'+query['data'][0][4:6]+':'+query['data'][0][6:8]+':'+query['data'][0][8:10]+':'+query['data'][0][10:12])
				temp = float(temp/100)
				print('temp',temp)
			elif(loc_mode==0x6):
				temp = twos_complement(query['data'][0][14:18],16)
				temp = float(temp/100)
				print('temp',temp)






			"""print("température")
			print(query['data'][0][14:18])
			temp = twos_complement(query['data'][0][14:18],16)
			temp = float(temp/100)
			print(temp)

			print("accélération z")
			print(query['data'][0][18:20])
			z = twos_complement(query['data'][0][18:20],8)
			z = float(z/10)
			print(z)

			print("accélération y")
			print(query['data'][0][16:18])
			y = twos_complement(query['data'][0][16:18],8)
			y = float(y/10)
			print(y)

			print("accélération x")
			print(query['data'][0][14:16])
			x = twos_complement(query['data'][0][14:16],8)
			x = float(x/10)
			print(x)

			print("RSSID ")
			print(query['data'][0][12:14])
			rssid = int(query['data'][0][12:14],16)
			print(rssid)

			print("BSSID ")
			print(query['data'][0][0:2]+':'+query['data'][0][2:4]+':'+query['data'][0][4:6]+':'+query['data'][0][6:8]+':'+query['data'][0][8:10]+':'+query['data'][0][10:12])"""


			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()

		elif self.path == "/index.html":
			print("Validation")
			q = self.rfile.read(int(self.headers['content-length'])).decode(encoding="utf-8")
			query = urllib.parse.parse_qs(q,keep_blank_values=1,encoding='utf-8')


			res = urllib.parse.urlparse(self.path)
			rep = self.mysql.select_user()


			valide = False
			#print('rep[0][0]')
			#print(rep[0][0])
			#print('rep')
			#print (rep)
			#print (rep[0][1])

			for i in range(len(rep)):

				if (int(rep[i][0]) == int(query['id'][0])):
					print('id identiques')
					if (rep[i][1] == query['password'][0]):
						print('id et mdp identiques')
						valide = True
						break
				else:
					print(int(rep[i][0]), " différent de ", int(query['id'][0]))



			if (valide):
				print("C'est valide")
				id_utilisateur = int(query['id'][0])
				self.send_response(200)
				self.send_header("Content-type", "text/html")
				self.end_headers()
				with open('accueil.html', 'r') as f:
					html = f.read()
					self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))


			else:
				self.send_response(200)
				self.send_header("Content-type", "text/html")
				self.end_headers()
				with open('log-error.html', 'r') as f:
					html = f.read()
					self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))






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

	def select_user(self):
		req = "select * from User"
		print(req)
		return self.c.execute(req).fetchall()

	def select_animals(self):
		req = "select name from Animal where owner = +" + str(id_utilisateur) + ";"
		s = self.c.execute(req).fetchall()
		return s

	def select_loc(self):
		req = "select y0,x0 from Animal where owner = +" + str(id_utilisateur) + ";"
		s = self.c.execute(req).fetchall()
		print("S:",s)
		s = s[id_animal-1]
		return s

	def temperature_html(self):
		req = "select date_insert, temp, animal from Temperature where owner = " + str(id_utilisateur) + ";"
		s = self.c.execute(req).fetchall()

		print("tous_les_s", s)

		ss = []

		for e in s:
			if e[2] == id_animal:
				ss.append(e)
		#s = s[id_animal]

		print("ID_ANIMAL", id_animal)
		print("nouveau_ss", ss)


		return ss

	def localisation_html(self):
		req1 = "select secuperim from Animal where owner = " + str(id_utilisateur) + ";"
		num_p = self.c.execute(req1).fetchall()[0][0]
		print("num_p:", num_p)
		req = "select * from Perimeter;"
		s = self.c.execute(req).fetchall()
		s = s[num_p-1]
		print("S:",s)
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


def twos_complement(hexstr,bits):
	value = int(hexstr,16)
	if value & (1 << (bits-1)):
		value -= 1 << bits
	return value


if __name__ == '__main__':
	server_class = http.server.HTTPServer
	httpd = server_class(("localhost", 8888), MyHandler)
	try:
		httpd.serve_forever()
	except KeyboardInterrupt:
		pass
	httpd.server_close()
