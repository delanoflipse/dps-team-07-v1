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

socket.on('/amb-global-distance', (data) => {
    app.distance = Number(data);
});

const app = new Vue({
    el: '#app',
    data: {
        distance: 6000,
        devices: {
            'test-device': {
                id: 'x',
                nearby: null,
                closest: null,
                state: 'dorment',
                orientation: 'left',
                mode: 'forest',
                lastUpdate: new Date().valueOf(),
                forced: false,
                pickedUp: false,
            },
        },
        moment,
    },

    methods: {
        updateState(device, newState) {
            socket.emit('setState', device.id, newState);
        },
        asTimeSince(device) {
            return moment(device.lastUpdate).format("HH:mm:ss");
        },
        distanceChange() {
            socket.emit('setDistance', this.distance);
        },
    },

    computed: {
        dist() {
            const meters = this.distance / 1000;
            return `${meters.toFixed(2)} m`;
        },
        safe() {
            return Number(this.distance) > 1500;
        },
    },
});
