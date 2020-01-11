const mqtt = require('mqtt');

var client = mqtt.connect('mqtt://localhost:1883');

client.on('connect', () =>{
    setInterval(() =>{
        client.subscribe('esp/sensor');
        client.publish('esp/actuator', 'on');
        console.log("Message sent");
    }, 10000);
});

client.on('message',(topic,message) => {
    context = message.toString();
    console.log(context);
});