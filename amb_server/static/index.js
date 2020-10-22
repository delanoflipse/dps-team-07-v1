const socket = io();

const stateMap = {
    "up": "silent",
    "down": "rain",
    "right": "forest",
    "left": "fire",
    "front": "zen",
    "back": "jazz",
}

socket.on('/log-device', (data) => {
    const [ id, nearby, closest, state, orientation, forced, pickedUp ] = data.split(/\t/g);
    const lastUpdate = moment().valueOf();
    const mode = stateMap[orientation];
    app.$set(app.devices, id, {
        id,
        nearby,
        closest,
        state,
        mode,
        orientation,
        lastUpdate,
        forced: Boolean(Number(forced)),
        pickedUp,
    });
});

const app = new Vue({
    el: '#app',
    data: {
        devices: {},
        moment,
    },
    methods: {
        updateState(device, newState) {
            socket.emit('setState', device.id, newState);
        },
        asTimeSince(device) {
            return moment(device.lastUpdate).format("HH:mm:ss");
        }
    },
});
