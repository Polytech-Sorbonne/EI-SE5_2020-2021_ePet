import requests,json

# fonction qui permet de formater une mac adress (BSSID) reçue pour l'envoyer a Google API
def macAdress_formatage(mac):
    mac_format = ""
    for i in range(len(mac)):
        mac_format +=  mac[i].swapcase()
        if (i%2 != 0 and i != len(mac) - 1):
            mac_format += ":"
    return mac_format

# fonction qui permet de formater une force de signal (RSSI) reçue pour l'envoyer a Google API

def twos_complement(hexstr,bits):
	value = int(hexstr,16)
	if value & (1 << (bits-1)):
		value -= 1 << bits
	return value


def signalStrength_formatage(strength):
    strength_format = twos_complement(strength,8) #version hex de 0xstrentgh
    return str(strength_format)

###########   test ###########
def get_location(trame1, trame2):
    macAdress = []
    signalStrength = []


    #trames de test
    #trame1 = "5897BDCD9260CD00FE030000"
    #trame2 = "7062B8512620CD00FE030000"

    macAdress.append(trame1[0:12])
    macAdress.append(trame2[0:12])

    signalStrength.append(trame1[12:14])
    signalStrength.append(trame2[12:14])

    for i in range (2):
        print("Mac Adress : ", macAdress[i])
        print("version formatée:", macAdress_formatage(macAdress[i]))
        print("signal Strength:", signalStrength[i])
        print("version formatée:", signalStrength_formatage(signalStrength[i]))
        print("--")
    # formation du message json
    file = open("mac_adresses.json","w")
    json_file = '''{ \n"considerIp": "false",\n'''
    json_file +=''' "wifiAccessPoints": [\n'''
    json_file +='''     {\n'''
    json_file +='''         "macAddress":"''' + macAdress_formatage(macAdress[0]) + '''",\n'''
    json_file +='''         "signalStrength":''' + signalStrength_formatage(signalStrength[0]) + ''',\n'''
    json_file +='''         "signalToNoiseRatio": 0\n'''
    json_file +='''     },\n'''
    json_file +='''     {\n'''
    json_file +='''         "macAddress":"''' + macAdress_formatage(macAdress[1]) + '''",\n'''
    json_file +='''         "signalStrength":''' + signalStrength_formatage(signalStrength[1]) + ''',\n'''
    json_file +='''         "signalToNoiseRatio": 0\n'''
    json_file +='''     }\n'''
    json_file +=''' ]\n'''
    json_file +='''}\n'''

    file.write(json_file)
    file.close()

    #fichier pour lire la clé:
    kapi = open("key2.txt", "r")
    ligne = kapi.read()
    print('ligne',ligne)
    ligne = ligne.rstrip('\n')


    print("classe = ", type(ligne).__name__)


    #key a remplacer par celle de Melissa dans le trello
    url = 'https://www.googleapis.com/geolocation/v1/geolocate?key='+ str(ligne)
    donnees = json.loads(json_file)
    print("donnees = ",donnees)
    print("")
    print(donnees["wifiAccessPoints"][0])
    donnees_json =  json.dumps(donnees)
    print("Donnees JSON", donnees_json)
    r = requests.post(url, data=donnees_json)
    print("reponse : ", r)
    if (str(r)=="<Response [200]>"):
        reponse = r.json()
        print('response',reponse)
        print("lat = ",reponse['location']['lat'])
        print("long= ",reponse['location']['lng'])
        location = [reponse['location']['lat'], reponse['location']['lng']]

        return location
    else :
        print('error')
        return [0,0]
