const mqtt = require('mqtt');
const { key, secret } = require('./secrets.js');

const express = require('express');
const app = express();
const http = require('http').createServer(app);
const io = require('socket.io')(http);

app.use(express.static('static'));

const client = mqtt.connect(`mqtt://${key}:${secret}@broker.shiftr.io`, {
	clientId: 'javascript'
});

client.on('connect', () => {
	console.log('client has connected!');

	client.subscribe('/log-device');
	client.subscribe('/amb-global-distance');
	// client.unsubscribe('/example');
});

client.on('message', (topic, message) => {
	io.emit(topic, message.toString());
});


io.on('connection', (socket) => {
	socket.on('setState', (id, newState) => {
		client.publish(`/amb-action-${id}`, `0\t${newState}`);
	});
	socket.on('setDistance', (newDistance) => {
		client.publish(`/amb-global-distance`, newDistance);
	});
});

http.listen(8010, '0.0.0.0', () => {
	console.log('listening on *:8010');
});
