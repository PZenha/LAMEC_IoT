const moongose = require('mongoose');

const Schema = moongose.Schema;

const eventSchema = new Schema({
    T_air: {
        type: Number
    },
    H_air: {
        type: Number
    },
    soil: {
        type: Number
    },
    Time: {
        type: Date,
        default: Date.now
    },
    LastTime: {
        type: Date,
    }
});

module.exports = moongose.model('Event', eventSchema);