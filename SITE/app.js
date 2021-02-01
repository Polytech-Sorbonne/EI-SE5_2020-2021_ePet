const path = require('path');

const express = require('express');
const bodyParser = require('body-parser');

let app = express();
const server = require('http').Server(app);

const request = require('request');

const sqlite3 = require('sqlite3').verbose();
let db = new sqlite3.Database(__dirname + '/db.sqlite');


let wifis = {};

const GOOGLE_API_TOKEN = "MON_API_KEY";


app.use(express.static('public'));
app.use(bodyParser.json());

// Get map view
app.get('/', function(req, res) {
	res.sendFile(path.join(__dirname, "/views/index.html"));
});

// Get device JSON infos (For search ajax request)
app.get('/device', function(req, res) {
	if(req.query["device"])
	{
		console.log(req.query["device"]);
		db.get("SELECT * FROM devices WHERE id=?", [req.query["device"]], (err, row) => {
			if(err) console.error(err);

			if(!row)
			{
				res.status(404).json({ message : "No device found !"});
			}
			else
			{
				res.json(row);
			}
		});
	}
});

// Add/Update device position (Sigfox callback)
app.post('/device', function(req, res) {
	let device = req.body.device;

	console.log(device);

	let mac = "";
	let rssi;
	
	// Decode Sigfox frame (MAC (6B) + RSSI (5B))
	for(let i = 0; i < 6; i++)
	{
		mac += req.body.data[i * 2 + 0] + req.body.data[i * 2 + 1] + ":";
	}
	mac = mac.substring(0, mac.length - 1);

	rssi = req.body.data.substring(12, req.body.data.length - 4).match(/../g).reverse().join('');
	rssi = hexToSignedInt(rssi);

	console.log(mac + " , rssi : " + rssi);

	// Add wifi informations to divice's wifis dictionary
	if(!wifis[device])
		wifis[device] = [];
	
	wifis[device].push({
		macAddress : mac,
		signalStrength : rssi
	});

	// If two frame are received, we could call google geolocation on them
	if(wifis[device].length >= 2)
	{
		request
			.post("https://www.googleapis.com/geolocation/v1/geolocate?key=" + GOOGLE_API_TOKEN, {
				json : {
					wifiAccessPoints : [
						wifis[device][wifis[device].length - 2], 
						wifis[device][wifis[device].length - 1]
					]
				}
			}, (err, res, body) => {
				if(err) console.error(err);

				db.get("SELECT * FROM devices WHERE id=?", [device], (err, row) => {
					if(err) console.error(err);

					if(!row)	// Add new device
					{
						let req = db.prepare("INSERT INTO devices VALUES(?, ?, ?)");
						req.run([device, body.location.lng, body.location.lat]);
					}
					else		// Update device
					{
						let req = db.prepare("UPDATE devices SET lng = ?, lat = ? WHERE id = ?");
						req.run([body.location.lng, body.location.lat, device]);
					}
				});
			});
	}

	return res.end();
});

server.listen(3000, function() {
	console.log("Listening on port 3000");
});

// Function to transform hexadecimal string in integer
function hexToSignedInt(hex) {
    if (hex.length % 2 != 0) {
        hex = "0" + hex;
    }
    let num = parseInt(hex, 16);
    let maxVal = Math.pow(2, hex.length / 2 * 8);
    if (num > maxVal / 2 - 1) {
        num = num - maxVal
    }
    return num;
}
