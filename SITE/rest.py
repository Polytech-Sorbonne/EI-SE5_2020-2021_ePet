# curl -X GET http://localhost:8888/Etudiant
# curl -X POST http://localhost:8888/Etudiant/\?Nom\=Cionaire\&Prenom\=Dick\&idAd\=2

import http.server, urllib.parse, sqlite3
import requests
import random
import time
import Geoloc_example
import json

ENDPOINT = "things.ubidots.com"
DEVICE_LABEL = "epet"
VARIABLE_LABEL = "temperature"
TOKEN = "BBFF-7AY67IDc1sw2hLeaILMxUMTdHHkdBr"
DELAY = 5  # Delay in seconds

id_utilisateur = -1
id_animal = -1
data1 = ""
query1 = ""
cpt_device = 0
is_logged = False

class MyHandler(http.server.BaseHTTPRequestHandler):
	def __init__(self, *args, **kwargs):
		self.mysql = MySQL('epet.db')
		super(MyHandler, self).__init__(*args, **kwargs)

	def do_GET(self):
		global id_animal
		global is_logged
		"""Respond to a GET request."""

		if self.path == '/favicon.ico':
			return

		res = urllib.parse.urlparse(self.path)
		rep = ""
		#rep = self.mysql.select(res.path)

		 #test dans le navigateur
		if self.path[0:7] =="/device":
			#res = urllib.parse.urlparse(self.path)
			#query = urllib.parse.parse_qs(res.query)

			tab = []
			tab.append("8A9F6FE10E74CD0000000000")
			tab.append("010001010100010000010000")
			tab.append("010001010100010000020000")
			tab.append("8A9F6FE10E74CD00FE030000")
			tab.append("0100010101000100FE050000")
			tab.append("0100010101000100FE060000")
			tab.append("0000000000000000FE060000")

			#bonnes trames
			trame1 ="5897BDCD9260CD00FE030000"
			trame2 ="7062B8512620CD00FE030000"

			#loc_mode= query['data'][0][18:20]
			#récupération de données selon la valeur du loc_mode
			for i in range(0,6):
				data = tab[i]
				loc_mode = data[18:20]

				print("LOC_MODE", loc_mode)
				device = "C5061B"

				if(loc_mode==00):
					bssid = twos_complement(data[0:12],48)
					rssi = twos_complement(data[12:14],8)
					print('rssi',data[12:14])
					print('bssid',data[0:2]+':'+data[2:4]+':'+data[4:6]+':'+data[6:8]+':'+data[8:10]+':'+data[10:12])
					location = Geoloc_example.get_location(trame1,trame2)
					self.mysql.update_localisation(device,location)

				elif(loc_mode=="01"):
					distx = twos_complement(data[0:12],48)
					print('distx',data[0:2]+':'+data[2:4]+':'+data[4:6]+':'+data[6:8]+':'+data[8:10]+':'+data[10:12])
				elif(loc_mode=="02"):
					disty = twos_complement(data[0:12],48)
					print('disty',data[0:2]+':'+data[2:4]+':'+data[4:6]+':'+data[6:8]+':'+data[8:10]+':'+data[10:12])
				elif(loc_mode=="03"):
					bssid = twos_complement(data[0:12],48)
					rssi = twos_complement(data[12:14],8)
					temp = twos_complement(data[14:18],16)
					print('bssid',data[0:2]+':'+data[2:4]+':'+data[4:6]+':'+data[6:8]+':'+data[8:10]+':'+data[10:12])
					temp = float(temp/100)
					print('temp',temp)
					location = Geoloc_example.get_location(trame1,trame2)
					self.mysql.update_localisation(device,location)
				elif(loc_mode=="04"):
					distx = twos_complement(data[0:12],48)
					temp = twos_complement(data[14:18],16)
					print('distx',data[0:2]+':'+data[2:4]+':'+data[4:6]+':'+data[6:8]+':'+data[8:10]+':'+data[10:12])
					temp = float(temp/100)
					print('temp',temp)
				elif(loc_mode=="05"):
					disty = twos_complement(data[0:12],48)
					temp = twos_complement(data[14:18],16)
					print('disty',data[0:2]+':'+data[2:4]+':'+data[4:6]+':'+data[6:8]+':'+data[8:10]+':'+data[10:12])
					temp = float(temp/100)
					print('temp',temp)
				elif(loc_mode=="06"):
					temp = twos_complement(data[14:18],16)
					temp = float(temp/100)
					print('temp',temp)


			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()

		if self.path == "/Accueil":
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()

			if not is_logged:#la page d'accueil qui renvoie sur log si l'utilisateur s'est pas connecté
				with open('log.html', 'r') as f:
					html = f.read()
					self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))
			else:
				with open('accueil.html', 'r') as f:
					html = f.read()
					self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))

		if self.path == "/Log" or self.path == "/" or self.path == "" :
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()
			with open('log.html', 'r') as f:
				html = f.read()
				self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))

		if self.path[0:5] == "/Temp":
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()

			if not is_logged:#la page de température qui renvoie sur log si l'utilisateur s'est pas connecté
				with open('log.html', 'r') as f:
					html = f.read()
					self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))
			else:
				try:
					id_animal = int(self.path[6:]) + 1

				except:
					id_animal = 1


				with open('temperature_debut.html', 'r') as f:
					my_str = f.read()

				s = self.mysql.temperature_html()

				print("S")
				print(s)

				my_str += "\t\t\t\t\tdata.addRows(" + str(len(s)) + ");"

				for i in range(len(s)):

					my_str += "\n\t\t\t\tdata.setCell(" + str(i) + ", 0, '" + str(s[i][1]) + "');"
					my_str += "\n\t\t\t\tdata.setCell(" + str(i) + ", 1, '" + str(s[i][0]) + "');"


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

		if self.path[0:4] == "/Loc":
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()

			if not is_logged:#la page de localisation qui renvoie sur log si l'utilisateur s'est pas connecté
				with open('log.html', 'r') as f:
					html = f.read()
					self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))

			else:

				try:
					id_animal = int(self.path[5:]) + 1

				except:
					id_animal = 1

				with open('localisation_debut.html', 'r') as f:
					my_str = f.read()

				s = self.mysql.localisation_html()

				loc = self.mysql.select_loc()

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
				#affichage noms des animaux
				for i in range(len(a)):
					my_str = my_str + '<a href="/Loc-' + str(i) + '" <button>' + a[i][0] + ' </button></a>'

				#test du périmètre
				mx1 = x1-x0
				mx2 = x2-x0
				mx3 = x3-x0
				my1 = y1-y0
				my2 = y2-y0
				my3 = y3-y0

				e1 = mx1*my2 - my1*mx2
				e2 = mx2*my3 - my2*mx3
				e3 = mx3*my1 - my3*mx1

				print("e1:",str(e1))
				print("e2:",str(e2))
				print("e3:",str(e3))

				if ((e1 >= 0 and e2 >= 0 and e3 >= 0) or(e1 <= 0 and e2 <= 0 and e3 <= 0)):
					print("Le point est dans le périmètre")

				else:
					print("Le point est hors du périmètre!")
					my_str += '<br><br><br><center><p style="color:red;font-size:30px" ;>Sortie du périmètre détectée!</p></center><br>'

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
		global data1
		global query1
		global cpt_device
		global is_logged
		# """Respond to a POST request."""
		# res = urllib.parse.urlparse(self.path)
		# query = urllib.parse.parse_qs(res.query)
		# rep = self.mysql.insert(res.path,query)
		# self.send_response(200)
		# self.send_header("Content-type", "text/html")
		# self.end_headers()


		if self.path == "/index.html":
			print("Validation")
			q = self.rfile.read(int(self.headers['content-length'])).decode(encoding="utf-8")
			query = urllib.parse.parse_qs(q,keep_blank_values=1,encoding='utf-8')


			res = urllib.parse.urlparse(self.path)
			rep = self.mysql.select_user()


			valide = False

			for i in range(len(rep)):

				if (int(rep[i][0]) == int(query['id'][0])):
					print('id identiques')
					if (rep[i][1] == query['password'][0]):
						print('id et mdp identiques')
						valide = True
						is_logged = True
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


		if self.path[0:7] =="/device":
			#print(self.path)
			#res = urllib.parse.urlparse(self.path)
			#query = urllib.parse.parse_qs(res.query)

			query = json.loads(self.rfile.read(100))
			print(query)

			if cpt_device == 0:
				data1 = query['data']
				query1 = query

			cpt_device += 1

			if cpt_device == 2:

				cpt_device = 0

				print("query")
				print(query)
				print("device")
				print(query['device'])
				print("time")
				print(query['time'])
				print("data")
				print(query['data'])

				data = str(query['data'])
				loc_mode = data[18:20]
				print("LOC_MODE:",loc_mode)

				device = str(query['device'])

				data1 = str(query1['data'])
				loc_mode1 = data1[18:20]
				print("LOC_MODE1:",loc_mode1)

				device1 = str(query1['device'])


				if(loc_mode=="00"):
					# bssid = twos_complement(query['data'][0][0:12],48)
					# rssi = twos_complement(query['data'][0][12:14],8)
					# print('rssi',query['data'][0][12:14])
					# print('bssid',query['data'][0][0:2]+':'+query['data'][0][2:4]+':'+query['data'][0][4:6]+':'+query['data'][0][6:8]+':'+query['data'][0][8:10]+':'+query['data'][0][10:12])
					position = Geoloc_example.get_location(data,data1)
					self.mysql.update_localisation(device,position)
				elif(loc_mode=="01"):
					distx = twos_complement(query['data'][0:12],48)
					print('distx',query['data'][0:2]+':'+query['data'][2:4]+':'+query['data'][4:6]+':'+query['data'][6:8]+':'+query['data'][8:10]+':'+query['data'][10:12])
				elif(loc_mode=="02"):
					disty = twos_complement(query['data'][0:12],48)
					print('disty',query['data'][0:2]+':'+query['data'][2:4]+':'+query['data'][4:6]+':'+query['data'][6:8]+':'+query['data'][8:10]+':'+query['data'][10:12])
				elif(loc_mode=="03"):
					#bssid = twos_complement(query['data'][0][0:12],48)
					#rssi = twos_complement(query['data'][0][12:14],8)
					temp = twos_complement(query['data'][14:18],16)
					#print('bssid',query['data'][0][0:2]+':'+query['data'][0][2:4]+':'+query['data'][0][4:6]+':'+query['data'][0][6:8]+':'+query['data'][0][8:10]+':'+query['data'][0][10:12])
					temp = float(temp/100)
					print('temp',temp)
					position = Geoloc_example.get_location(data,data1)
					self.mysql.update_localisation(device,position)
					self.mysql.insert_temp(device,temp)
				elif(loc_mode=="04"):
					distx = twos_complement(query['data'][0:12],48)
					temp = twos_complement(query['data'][14:18],16)
					print('distx',query['data'][0:2]+':'+query['data'][2:4]+':'+query['data'][4:6]+':'+query['data'][6:8]+':'+query['data'][8:10]+':'+query['data'][10:12])
					temp = float(temp/100)
					print('temp',temp)
					self.mysql.insert_temp(device,temp)
				elif(loc_mode=="05"):
					disty = twos_complement(query['data'][0:12],48)
					temp = twos_complement(query['data'][14:18],16)
					print('disty',query['data'][0:2]+':'+query['data'][2:4]+':'+query['data'][4:6]+':'+query['data'][6:8]+':'+query['data'][8:10]+':'+query['data'][10:12])
					temp = float(temp/100)
					print('temp',temp)
					self.mysql.insert_temp(device,temp)
				elif(loc_mode=="06"):
					temp = twos_complement(query['data'][14:18],16)
					temp = float(temp/100)
					print('temp',temp)
					self.mysql.insert_temp(device,temp)

				#deuxième trame
				if(loc_mode1=="00"):
					#bssid1 = twos_complement(query1['data'][0][0:12],48)
					#rssi1 = twos_complement(query1['data'][0][12:14],8)
					#print('rssi',query1['data'][0][12:14])
					#print('bssid',query1['data'][0][0:2]+':'+query1['data'][0][2:4]+':'+query1['data'][0][4:6]+':'+query1['data'][0][6:8]+':'+query1['data'][0][8:10]+':'+query1['data'][0][10:12])
					pass
				elif(loc_mode1=="01"):
					distx = twos_complement(query1['data'][0:12],48)
					print('distx',query1['data'][0:2]+':'+query1['data'][2:4]+':'+query1['data'][4:6]+':'+query1['data'][6:8]+':'+query1['data'][8:10]+':'+query1['data'][10:12])
				elif(loc_mode1=="02"):
					disty = twos_complement(query1['data'][0:12],48)
					print('disty',query1['data'][0:2]+':'+query1['data'][2:4]+':'+query1['data'][4:6]+':'+query1['data'][6:8]+':'+query1['data'][8:10]+':'+query1['data'][10:12])
				elif(loc_mode1=="03"):
					#bssid = twos_complement(query1['data'][0][0:12],48)
					#rssi = twos_complement(query1['data'][0][12:14],8)
					temp = twos_complement(query1['data'][14:18],16)
					#print('bssid',query1['data'][0][0:2]+':'+query1['data'][0][2:4]+':'+query1['data'][0][4:6]+':'+query1['data'][0][6:8]+':'+query1['data'][0][8:10]+':'+query1['data'][0][10:12])
					temp = float(temp/100)
					print('temp',temp)
					self.mysql.insert_temp(device,temp)
				elif(loc_mode1=="04"):
					distx = twos_complement(query1['data'][0:12],48)
					temp = twos_complement(query1['data'][14:18],16)
					print('distx',query1['data'][0:2]+':'+query1['data'][2:4]+':'+query1['data'][4:6]+':'+query1['data'][6:8]+':'+query1['data'][8:10]+':'+query1['data'][10:12])
					temp = float(temp/100)
					print('temp',temp)
					self.mysql.insert_temp(device,temp)
				elif(loc_mode1=="05"):
					disty = twos_complement(query1['data'][0:12],48)
					temp = twos_complement(['data'][14:18],16)
					print('disty',query1['data'][0:2]+':'+query1['data'][2:4]+':'+query1['data'][4:6]+':'+query1['data'][6:8]+':'+query1['data'][8:10]+':'+query1['data'][10:12])
					temp = float(temp/100)
					print('temp',temp)
					self.mysql.insert_temp(device,temp)
				elif(loc_mode1=="06"):
					temp = twos_complement(query1['data'][14:18],16)
					temp = float(temp/100)
					print('temp',temp)
					self.mysql.insert_temp(device,temp)

				else:
					with open('log-error.html', 'r') as f:
						html = f.read()
						self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))

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


	def insert_temp(self,device,temp):
		req = "select owner, iddevice from Animal where iddevice = " + device + ";"
		print("REQ1 = ", req )
		liste_animaux = self.c.execute(req).fetchall()
		rang = 0

		print("LISTE ANIMAUX", liste_animaux)

		try:
			id_owner = liste_animaux[0][0]
			print("ID_OWNER", id_owner)
		except:
			pass

		req = "select iddevice from Animal where owner = " + str(id_owner) + ";"
		print("REQ2 = ", req )
		liste_animaux = self.c.execute(req).fetchall()

		for a in liste_animaux:
			rang += 1
			print("A",a)
			print("A1", '"' + a[0] + '"')
			print("Device", device)
			if (device == '"' + a[0] + '"'):
				break

		req = "insert into Temperature (temp,date_insert,animal,owner) VALUES (" + str(temp) + ",CURRENT_TIMESTAMP, " + str(rang) + "," + str(id_owner) + ");"
		print("REQ3 = ", req )
		self.c.execute(req)
		self.conn.commit()

	def update_localisation(self,device,localisation):
		print("DEVICE:", device)
		req = "update Animal set y0 = " + str(localisation[0]) + ", x0 = " + str(localisation[1]) + " where iddevice = \"" + device +  "\";"
		print("REQ = ", req )

		secuperim = self.c.execute("select secuperim from Animal where iddevice = \"" + device + "\";").fetchall()
		print('perimetre',secuperim)

		perim = self.c.execute("select * from Perimeter where id = " + str(secuperim[0][0]) + ";").fetchall()
		print('perim',perim)

		yp = localisation[0]
		xp = localisation[1]

		y1 = perim[0][1]
		x1 = perim[0][2]
		y2 = perim[0][3]
		x2 = perim[0][4]
		y3 = perim[0][5]
		x3 = perim[0][6]

		print('y1',y1)
		print('x1',x1)
		print('y2',y2)
		print('x2',x2)
		print('y3',y3)
		print('x3',x3)
		#calcul des vecteurs pour voir si l'animal est hors périmètre

		mx1 = x1-xp
		mx2 = x2-xp
		mx3 = x3-xp
		my1 = y1-yp
		my2 = y2-yp
		my3 = y3-yp

		e1 = mx1*my2 - my1*mx2
		e2 = mx2*my3 - my2*mx3
		e3 = mx3*my1 - my3*mx1

		print("e1:",str(e1))
		print("e2:",str(e2))
		print("e3:",str(e3))

		if ((e1 >= 0 and e2 >= 0 and e3 >= 0) or(e1 <= 0 and e2 <= 0 and e3 <= 0)):
			print("Le point est dans le périmètre")

		else:
			print("Le point est hors du périmètre!")

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
