const socket = io();

const stateMap = {
    "up": "silent",
    "down": "rain",
    "right": "forest",
    "left": "fire",
    "front": "zen",
    "back": "jazz",
}

const orientationColorMap = {
    "up": ["hsl(206, 10%, 50%)", '#f0f0f0'],
    "down": ["hsl(206, 100%, 50%)", '#f0f0f0'],
    "right": ["green", '#f0f0f0'],
    "left": ["hsl(15, 100%, 50%)", '#f0f0f0'],
    "front": ["hsl(265, 50%, 50%)", '#f0f0f0'],
    "back": ["#c59800", '#f0f0f0'],
};

const stateColorMap = {
    "dorment": ["#dedede", '#444'],
    "sit_on": ["#b4bdca", 'black'],
    "moving": ["#3c3c3c", 'white'],
    "error": ["#ff0000", 'white'],
};

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
                orientation: 'right',
                mode: 'forest',
                lastUpdate: new Date().valueOf(),
                forced: false,
                pickedUp: false,
            },
            'test-device-2': {
                id: 'x',
                nearby: null,
                closest: null,
                state: 'sit_on',
                orientation: 'down',
                mode: 'rain',
                lastUpdate: new Date().valueOf(),
                forced: false,
                pickedUp: false,
            },
        },
        moment,
        now: moment().valueOf(),
    },

    mounted() {
        setInterval(() => {
            this.now = moment().valueOf();
        }, 200);
    },

    methods: {
        updateState(device, newState) {
            socket.emit('setState', device.id, newState);
        },
        deviceOrientColor(device) {
            return orientationColorMap[device.orientation][0];
        },
        deviceOrientBackgroundcolor(device) {
            return orientationColorMap[device.orientation][1];
        },
        deviceStateColor(device) {
            return stateColorMap[device.state][1];
        },
        connected(device) {
            return this.now - device.lastUpdate < 5000;
        },
        deviceStateBackgroundcolor(device) {
            return stateColorMap[device.state][0];
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
        distanceState() {
            const dist = Number(this.distance);
            if (dist > 4500) {
                return "Alone";
            }
            if (dist > 1500) {
                return "Safe";
            }
            return "Danger";
        },
        safe() {
            return Number(this.distance) > 1500;
        },
        alone() {
            return Number(this.distance) > 4500;
        },
    },
});
