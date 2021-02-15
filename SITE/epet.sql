-- sqlite3 epet.db
-- .read epet.sql

--Destruction de tables
DROP TABLE IF EXISTS User;
DROP TABLE IF EXISTS Animal;
DROP TABLE IF EXISTS Perimeter;
DROP TABLE IF EXISTS Temperature;

--Creation de tables
CREATE TABLE User(id INTEGER PRIMARY KEY AUTOINCREMENT,password TEXT NOT NULL,username TEXT NOT NULL,nbanimals INTEGER NOT NULL);
CREATE TABLE Animal(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT NOT NULL,type INTEGER NOT NULL,state INTEGER NOT NULL,y0 FLOAT NOT NULL,x0 FLOAT NOT NULL,secuperim INTEGER NOT NULL,owner INTEGER NOT NULL,iddevice TEXT NOT NULL,FOREIGN KEY (secuperim) REFERENCES Perimeter(id),FOREIGN KEY (owner) REFERENCES User(id));
CREATE TABLE Temperature(id INTEGER PRIMARY KEY AUTOINCREMENT,date_insert TIMESTAMP DEFAULT CURRENT_TIMESTAMP,temp FLOAT NOT NULL,animal INTEGER NOT NULL,owner INTEGER NOT NULL,FOREIGN KEY (animal) REFERENCES Animal(id),FOREIGN KEY (owner) REFERENCES User(id));
CREATE TABLE Perimeter(id INTEGER PRIMARY KEY AUTOINCREMENT,y1 FLOAT NOT NULL,x1 FLOAT NOT NULL,y2 FLOAT NOT NULL,x2 FLOAT NOT NULL,y3 FLOAT NOT NULL,x3 FLOAT NOT NULL);

--Insertion pour but d'essai

--Insertion de données des users
INSERT INTO User(password,username,nbanimals) VALUES ("pet","marie",2);
INSERT INTO User(password,username,nbanimals) VALUES ("puppy","jacob",1);
--Insertion de données des animaux
INSERT INTO Animal(name,type,state,y0,x0,secuperim,owner,iddevice) VALUES ("tokyo","chien",0,48.8566,2.3522,1,1,"C5051C");
INSERT INTO Animal(name,type,state,y0,x0,secuperim,owner,iddevice) VALUES ("chanel","chat",1,48.8765,2.3783,1,1,"C5061B");
INSERT INTO Animal(name,type,state,y0,x0,secuperim,owner,iddevice) VALUES ("franklin","chien",1,2,3,2,2,"C4181F");
--Insertion de données de localisation
INSERT INTO Perimeter(y1,x1,y2,x2,y3,x3) VALUES (48.8852,2.3856,48.8354,2.4055,48.9056,2.4044);
INSERT INTO Perimeter(y1,x1,y2,x2,y3,x3) VALUES (1,1,2,4,3,2);
--Insertion de données de température
INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (38.5,CURRENT_TIMESTAMP,1,1);
INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (39.5,CURRENT_TIMESTAMP,1,1);
INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (38.9,CURRENT_TIMESTAMP,1,1);
INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (38.8,CURRENT_TIMESTAMP,1,1);
INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (39.3,CURRENT_TIMESTAMP,1,1);

INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (38.3,CURRENT_TIMESTAMP,2,1);
INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (39.5,CURRENT_TIMESTAMP,2,1);
INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (38.9,CURRENT_TIMESTAMP,2,1);
INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (38.3,CURRENT_TIMESTAMP,2,1);

INSERT INTO Temperature(temp,date_insert,animal,owner) VALUES (38.7,CURRENT_TIMESTAMP,1,2);
