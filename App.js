const express = require('express');
const mqtt = require('mqtt');
const bodyParser = require('body-parser');
const moongose = require('mongoose');
const Event = require('./models/event');
require('dotenv/config')

const app = express();

var mqttData;
app.use(bodyParser.json());

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

client.on('connect', () =>{
    setInterval(() =>{
        client.subscribe('esp/sensor');
        //client.publish('esp/test', 'Ola mqtt');
        //console.log("Message sent");
    }, 5000);
});

client.on('message', (topic,message)=>{   // T_air:21.60 H_air:54.00 soil:100;
    context = message.toString();
    var T_air_Index = context.indexOf("T_air");
    var H_air_Index = context.indexOf("H_air");
    var soil_Index = context.indexOf("soil:");
    let T_air = context.slice(T_air_Index + 6,H_air_Index - 1);
    let H_air = context.slice(H_air_Index + 6,soil_Index - 1);
    let soil = context.slice(soil_Index + 5, context.length);

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
    })
});

moongose.connect(process.env.DB_CONNECTION).then(() =>{
        console.log("connected to mongodb");
})

/*app.post('/data',(req,res) => {
    console.log(req.body);
    const event = new Event({
        T_air: req.body.T_air,
        H_air: req.body.H_air,
        soil: req.body.H_soil
    });

    event.save()
    .then(data => {
        console.log(data);
        res.json(data);
    })
    .catch(err => {
        console.log(err);
        res.json(err);
        throw err;
    })
}); */

app.get('/espdata',(req,res) => {
    var d = new Date();
    
    Event.find(/*{

        "Time": {
           $gte: d.setHours(d.getHours() - 1)
        } 
    }*/)
    .sort({_id:-1}).limit(10)
    .then(data => {
        res.json(data);
    })
    .catch(err => {
        res.json(err);
        throw err;
    })
});

app.get('/ola',(req,res)=>{
    let ola = "ola";
    res.json({data: ola});
});

app.listen(4000, ()=>{
    console.log("Listen on port 4000");
})