-- sqlite3 epet.db
-- .read epet.sql

--Destruction de tables
DROP TABLE IF EXISTS User;
DROP TABLE IF EXISTS Animal;
DROP TABLE IF EXISTS Perimeter;

--Creation de tables
CREATE TABLE User(id INTEGER PRIMARY KEY AUTOINCREMENT,password TEXT NOT NULL,username TEXT NOT NULL,nbanimals INTEGER NOT NULL);
CREATE TABLE Animal(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT NOT NULL,type INTEGER NOT NULL,state INTEGER NOT NULL,secuperim INTEGER NOT NULL,owner INTEGER NOT NULL,FOREIGN KEY (secuperim) REFERENCES Perimeter(id),FOREIGN KEY (owner) REFERENCES User(id));
CREATE TABLE Perimeter(id INTEGER PRIMARY KEY AUTOINCREMENT,x0 INTEGER NOT NULL,y0 INTEGER NOT NULL,x1 INTEGER NOT NULL,y1 INTEGER NOT NULL,x2 INTEGER NOT NULL,y2 INTEGER NOT NULL,x3 INTEGER NOT NULL,y3 INTEGER NOT NULL,idanimal INTEGER NOT NULL);
