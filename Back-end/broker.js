var mosca = require('mosca');
var ip = require("ip");

var settings = {
		port:1883
		}

var server = new mosca.Server(settings);

server.on('ready', function(){
console.log("broker ready on: " + ip.address());
});