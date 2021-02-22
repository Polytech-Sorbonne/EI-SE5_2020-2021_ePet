# Code Arduino 
Les dossiers contenus ici contiennent le code arduino compatible à l'esp32. Les librairies de l'accéléromètre MMA8451, et du capteur de temperature SHTC3 doivent être ajoutées aux codes correspondant et pour le schema complet. 


## Schéma complet
Explication du code:
Toutes les NB_SECOND = 60 , une interruption a lieu.
Cette interruption est rattachée à la tâche task1: elle permet de déterminer quand on doit stocker les données associées à l'accéléromètre et celles associées au capteur de température via deux compteurs : minutes_acc et totalInterruptCounter.

Au bout de 7 minutes, une moyenne des temperatures relevées est faite et stockée dans temp1 et temp2.

Pendant 15 minutes, des données d'activité sont déterminées en comparant la différence d'accélération sur les axes X et Y à un seuil de valeur seuilX et seuilY: si la différence est minime, on considère que l'activité n'est pas considérable. 

Sinon on le stocke dans mouvX ou mouvY et on incrémente un compteur countX ou countY suivant l'axe. 

Finalement, au bout de 15 minutes dans la loop principale, on scanne les wifis environnantes, on vérifie que ce ne sont pas les mêmes que l'on a déjà transmises au serveur, on élimine celles qui sont signes d'un constructeur mobile (en les comparant les 3 premiers octets avec ceux du tableau mac) et on forme le message Sigfox à envoyer au serveur. 

Ce message se forme en testant si la température est en dehors d'une plage de valeur typique d'une température normale d'un animal de compagnie (37-38 °C +- sensibilité_capteur), et si l'animal à bougé de façon significative (countX + countY > Xaddseuil + Yaddseuil). 

Ainsi, selon le cas, la trame formée et envoyée sera différente (voir doc SITE pour le décodage)



