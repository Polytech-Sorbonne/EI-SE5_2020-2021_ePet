# Serveur correspondant au site de visualisation des données

## Base de données
Le serveur est rattaché à une base de données epet.db, mise à jour à chaque réception de données.

## Récupération des données
Les données sont récupérées depuis le backend de sigfox via un POST sur /device.
Elles sont envoyées sous format JSON et contiennent :
- l'id du device Sigfox émetteur,
- la trame reçue
- le timestamp associé

Pour pouvoir recevoir les callbacks depuis le backend de Sigfox, il faut lancer le serveur en public. Dans le cadre de notre projet nous l'avons hébergé sur une Raspberry Pi.

Les fichiers contenus dans example_requests permettent de simuler un callback sigfox pour plusieurs trames.
	
## Localisation et requêtes Google API
La localisation est déterminée par la technique de sniffing de wifi, avec 2 adresses mac environnant le device et leur rssi, on peut interroger des bases telles que celle de Google, et recevoir une localisation. Pour se faire, il faut enregistrer un compte sur Google Api, créer un profil de paiement, et récupérer une clé API.
Notre clé API Google a été stockée dans le fichier key2.txt .

Le fichier Geoloc_example.py permet de récupérer les adresses mac et leur rssi depuis deux trames tests, puis d'interroger google API et récupérer le couple (lat,long) dans la réponse. 


