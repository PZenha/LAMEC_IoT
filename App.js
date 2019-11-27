const express = require('express');
const mqtt = require('mqtt');
const bodyParser = require('body-parser');
const moongose = require('mongoose');
const Event = require('./models/event');
require('dotenv/config')

const app = express();

app.use(bodyParser.json());

var client = mqtt.connect('mqtt://localhost:1883');

client.on('connect', () =>{
    setInterval(() =>{
        client.subscribe('esp/test');
        //client.publish('esp/test', 'Ola mqtt');
        console.log("Message sent");
    }, 5000);
});

client.on('message', (topic,message)=>{
    context = message.toString();
    console.log(context);
});

moongose.connect(process.env.DB_CONNECTION).then(() =>{
        console.log("connected to mongodb");
})

app.post('/data',(req,res) => {
    console.log(req.body);
    const event = new Event({
        T_air: req.body.T_air,
        H_air: req.body.H_air,
        H_soil: req.body.H_soil
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
});

app.get('/ola',(req,res)=>{
    let ola = "ola";
    res.json({data: ola});
});

app.listen(4000, ()=>{
    console.log("Listen on port 4000");
})