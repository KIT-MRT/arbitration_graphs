lastMsgTime = 0;

connectToRos = function (statusCallback, connectedCallback) {
    statusCallback('No ROS connection yet - please reload');

    var ips = ['ws://172.22.148.191:9090',
        'ws://localhost:9090', 'ws://53.52.131.172:9090'
    ];

    ros = [];

    for (i = 0; i < ips.length; i++) {
        console.log('Try IP: ' + ips[i]);
        try {
            ros[i] = new ROSLIB.Ros({
                url: ips[i]
            });

            ros[i].on('connection', function () {
                for (j = 0; j < ips.length; j++) {
                    if (!ros[j].isConnected)
                        continue;

                    console.log('Connected to websocket server at ' + ros[j].socket.url + '.');
                    statusCallback('Connected');
                    connectedCallback(ros[j]);
                }
            });

            ros[i].on('error', function (error) {
                console.log('Error connecting to websocket server: ',
                    error.target.url);
            });
        } catch (e) {
            console.log(e + ' not successful. ' + e);
            continue;
        }
    }
}

setupWatchdog = function (rosConnection, statusCallback, connectionLostCallback) {
    setInterval(function reload(statusCallback, connectionLostCallback) {
        now = new Date().getTime();
        diff = now - lastMsgTime;
        if (!rosConnection.isConnected) {
            if (lastMsgTime != 0) {
                statusCallback('ROS connection lost! Please reload');
                connectionLostCallback();
            }
            return;
        }
        if (lastMsgTime == 0) {
            console.log("Have not received any messages so far")
            statusCallback('No messages received yet');
            return;
        }
        if (diff > 5000) { // 5s
            console.log("Have not received messages for >5s!")
            statusCallback("No messages received for >5s!");
            connectionLostCallback();
            return;
        }
    }, 1000, statusCallback, connectionLostCallback); //1s
}


subscribeToArbitrationYaml = function (rosConnection, topicName, statusCallback, callback) {
    yamlSubscriber = new ROSLIB.Topic({
        ros: rosConnection,
        name: topicName,
        messageType: 'std_msgs/String'
    });

    console.log("Subscribe to arbitration yaml on " + topicName);

    //Subscriber Callback
    yamlSubscriber.subscribe(
        function (message) {
            lastMsgTime = new Date().getTime();
            statusCallback('Connected');
            callback(message);
        }
    );
}