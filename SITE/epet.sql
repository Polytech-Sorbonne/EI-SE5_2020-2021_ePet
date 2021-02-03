-- sqlite3 epet.db
-- .read epet.sql

--Destruction de tables
DROP TABLE IF EXISTS User;
DROP TABLE IF EXISTS Animal;
DROP TABLE IF EXISTS Perimeter;

--Creation de tables
CREATE TABLE User(id INTEGER PRIMARY KEY AUTOINCREMENT,password TEXT NOT NULL,username TEXT NOT NULL,nbanimals INTEGER NOT NULL);
CREATE TABLE Animal(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT NOT NULL,type INTEGER NOT NULL,state INTEGER NOT NULL,temp FLOAT NOT NULL,secuperim INTEGER NOT NULL,owner INTEGER NOT NULL,FOREIGN KEY (secuperim) REFERENCES Perimeter(id),FOREIGN KEY (owner) REFERENCES User(id));
CREATE TABLE Perimeter(id INTEGER PRIMARY KEY AUTOINCREMENT,x0 INTEGER NOT NULL,y0 INTEGER NOT NULL,x1 INTEGER NOT NULL,y1 INTEGER NOT NULL,x2 INTEGER NOT NULL,y2 INTEGER NOT NULL,x3 INTEGER NOT NULL,y3 INTEGER NOT NULL);

--Insertion pour but d'essai

--Insertion pour les données des users
INSERT INTO User(password,username,nbanimals) VALUES ("pet","marie",2);
INSERT INTO User(password,username,nbanimals) VALUES ("puppy","jacob",1);
--Insertion pour les données des animaux
INSERT INTO Animal(name,type,state,temp,secuperim,owner) VALUES ("tokyo","chien",0,38.5,1,1);
INSERT INTO Animal(name,type,state,temp,secuperim,owner) VALUES ("chanel","chat",1,39.3,1,1);
INSERT INTO Animal(name,type,state,temp,secuperim,owner) VALUES ("franklin","chien",1,37.0,2,2);
--Insertion pour les données de localisation
INSERT INTO Perimeter(x0,y0,x1,y1,x2,y2,x3,y3) VALUES (1,2,5,3,3,3,7,5);
INSERT INTO Perimeter(x0,y0,x1,y1,x2,y2,x3,y3) VALUES (2,3,1,1,2,4,3,2);
