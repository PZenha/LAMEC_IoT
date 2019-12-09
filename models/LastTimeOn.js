const moongose = require('mongoose');

const Schema = moongose.Schema;

const eventSchema = new Schema({
    LastTime: {
        type: Date,
        default: new Date()
    }
},
{
    collection: 'LastTime'
});

module.exports = moongose.model('LastTime', eventSchema);