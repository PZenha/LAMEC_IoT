const express = require('express');
const mqtt = require('mqtt');
const bodyParser = require('body-parser');
const moongose = require('mongoose');
const Event = require('./models/event');
const LastTime = require('./models/LastTimeOn');
require('dotenv/config')

const app = express();

app.use(bodyParser.json());

var TimeOn = "";

//CORS POLICY
//Setting the right headers to allow cross origin requests
app.use((req,res, next) => {
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'POST,GET,OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
    if (req.method === 'OPTIONS') {
      return res.sendStatus(200);
    }
    next();
  });

var client = mqtt.connect('mqtt://localhost:1883');

function SaveTimeToDB(){
    const lastTime = new LastTime({
        LastTime: new Date()
    });
    lastTime.save()
    .then(data => {
        TimeOn = data.LastTime;
    })
    .catch(err => {
        console.log(err);
        throw err;
    });
}

function GetLastTimeDB(){
    LastTime.find()
    .sort({_id: -1}).limit(1)
    .then(data => {
        TimeOn = data[0].LastTime;
    })
    .catch(err => {
        console.log(err);
        throw err;
    });
}

client.on('connect', () =>{
    setInterval(() =>{
        client.subscribe('esp/sensor');
        //client.publish('esp/test', 'Ola mqtt');
        //console.log("Message sent");
    }, 5000);
});

client.on('message', (topic,message)=>{   // T_air:21.60 H_air:54.00 soil:100;
    context = message.toString();
    console.log(context);
    if(context.includes("T_air")){
        var T_air_Index = context.indexOf("T_air");
        var H_air_Index = context.indexOf("H_air");
        var soil_Index = context.indexOf("soil:");
        var T_air = context.slice(T_air_Index + 6,H_air_Index - 1);
        var H_air = context.slice(H_air_Index + 6,soil_Index - 1);
        var soil = context.slice(soil_Index + 5, context.length);

        if(soil < 5){
            client.publish("esp/actuator","ON");
            console.log("Telling to turn water on...");
            SaveTimeToDB();
        }

        const event = new Event({
            T_air: T_air,
            H_air: H_air,
            soil:  soil 
        });

        event.save()
        .then(data => {
            console.log(data);
        })
        .catch(err => {
            console.log(err);
            throw err;
        }); 
    }
    
//If the ground is dry, than sends a mqtt message to turn the relay on
    

});

moongose.connect(process.env.DB_CONNECTION).then(() =>{
        console.log("connected to mongodb");
})

app.post('/actuator',(req,res) => {
    console.log(req.body);
    if(req.body.Relay === "ON"){
        client.publish("esp/actuator", "ON");
        SaveTimeToDB();
    }
    if(req.body.Relay === "OFF"){
        client.publish("esp/actuator", "OFF");
    }
    res.json("OK");
}); 

app.get('/espdata',(req,res) => {
    GetLastTimeDB();
    Event.find()
    .sort({_id:-1}).limit(10)
    .then(dados => {
       res.json({dados,
                  LastTime: new Date(TimeOn).toLocaleString()
                });
    })
    .catch(err => {
        res.json(err);
        throw err;
    })
});

app.listen(4000, ()=>{
    console.log("Listen on port 4000");
})