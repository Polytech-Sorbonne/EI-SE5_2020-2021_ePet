# path = device puis infos = device, time et data
#time = integer, time in seconds
#device : string, up to 8 char
#data : string hexadecimal

curl -X POST http://localhost:8888/device\?device\=\"8459a1\"\&time\=010320\&data\=8a9f6fe10e74cdfffe620880
