// Make sure to include arbitrator_option.js before loading this file!

app = Vue.createApp({
    components: {
        'arbitrator-option': ArbitratorOptionComponent,
    },
    data() {
        return {
            arbitrationGraph: null,
            status: "Loading…",
            browser: detectBrowser(),
            isConnected: false,
            offlineMode: false,
            fileName: "arbitration_graph",
            svgFilters: null,
            headless: false,
            showTitle: true,
            showButtons: true,
            showStatus: true,
            hoverTimeout: null,
            hoverAnimationTimeout: null,
            animateHoverBar: false
        }
    },
    methods: {
        connectToWebSocket() {
            const wsUrl = 'ws://localhost:' + location.port + '/status';
            this.websocket = new WebSocket(wsUrl);

            this.websocket.onopen = () => {
                this.isConnected = true;
                this.status = "Connected to WebSocket";
            };

            this.websocket.onmessage = (event) => {
                const message = event.data;
                this.setArbitrationGraphFromYaml(message);
            };

            this.websocket.onclose = () => {
                this.isConnected = false;
                this.status = "WebSocket connection closed";

                this.connectToWebSocket();
            };

            this.websocket.onerror = (error) => {
                console.error("WebSocket error:", error);
                this.status = "WebSocket error";
            };
        },
        setArbitrationGraph(graph) {
            this.arbitrationGraph = graph;
        },
        scaleAndAlignSvg() {
            if (!document.querySelector('svg') || !document.getElementById('root-svg-group')) {
                return;
            }

            var svgElement = document.querySelector('svg');
            var svgBBox = svgElement.getBoundingClientRect();

            var rootSvgGroup = document.getElementById('root-svg-group');
            var groupBBox = rootSvgGroup.getBBox();

            var scale = Math.min(svgBBox.width / groupBBox.width, svgBBox.height / groupBBox.height);
            var newGroupBBoxWidth = groupBBox.width * scale;
            var newGroupBBoxHeight = groupBBox.height * scale;

            var translateX = -groupBBox.x * scale + (svgBBox.width - newGroupBBoxWidth) / 2;
            var translateY = -groupBBox.y * scale + (svgBBox.height - newGroupBBoxHeight) / 2;

            rootSvgGroup.setAttribute("transform", "translate(" + translateX + ", " + translateY + ") scale(" + scale + ")");

        },
        setArbitrationGraphFromYaml(yaml) {
            yamlObject = jsyaml.load(yaml);
            if ('arbitration' in yamlObject) {
                this.arbitrationGraph = yamlObject.arbitration;
            } else {
                this.arbitrationGraph = yamlObject;
            }
            Vue.nextTick().then(function () {
                app.scaleAndAlignSvg();
            })
        },
        showTitleButtonsAndStatus() {
            this.showTitle = true;
            this.showButtons = true;
            this.showStatus = true;
            this.animateHoverBar = false;
        },
        hideTitleButtonsAndStatus() {
            this.showTitle = false;
            this.showButtons = false;
            this.showStatus = false;
        },
        setupOrResetHoverAnimation() {
            if (this.hoverTimeout) {
                clearTimeout(this.hoverTimeout)
                clearTimeout(this.hoverAnimationTimeout)
            }
            this.hoverTimeout = setTimeout(function () {
                app.hideTitleButtonsAndStatus();
                app.hoverTimeout = null;
            }, 5200)
            this.hoverAnimationTimeout = setTimeout(function () {
                app.hoverAnimationTimeout = null;
                app.animateHoverBar = true;
            }, 200)
        },
        clearHoverAnimation() {
        },
        mouseMovedOnSvg() {
            this.showTitleButtonsAndStatus();

            if (this.headless) {
                this.setupOrResetHoverAnimation();
            }
        },
        clickOnSvg() {
            this.mouseMovedOnSvg();
        }
    },
    mounted() {
        this.connectToWebSocket();

        window.onresize = () => {
            this.scaleAndAlignSvg();
        };
    }
}).mount('#hello-vue')

downloadYaml = function () {
    var yamlData = jsyaml.dump(app.arbitrationGraph);
    var yamlBlob = new Blob([yamlData], { type: "text/yaml;charset=utf-8" });
    var yamlUrl = URL.createObjectURL(yamlBlob);
    var downloadLink = document.createElement("a");
    downloadLink.href = yamlUrl;
    downloadLink.download = app.fileName + ".yaml";
    downloadLink.click();
}

downloadSvg = function () {
    var svgElement = document.querySelector("#arbitration-frame svg");
    inlineStyle(svgElement);
    var svgData = svgElement.outerHTML;
    var svgBlob = new Blob([svgData], { type: "image/svg+xml;charset=utf-8" });
    var svgUrl = URL.createObjectURL(svgBlob);
    var downloadLink = document.createElement("a");
    downloadLink.href = svgUrl;
    downloadLink.download = app.fileName + ".svg";
    downloadLink.click();

    app.arbitrationGraph = null;
    app.status = "Downloading… Page will be reloaded…";

    setTimeout(function () {
        window.location.reload();
    }, 2000)
}

loadExample = function (fileName) {
    fetch(fileName)
        .then(response => response.text())
        .then(data => {
            app.offlineMode = true;
            app.status = fileName + " loaded";
            app.fileName = fileName.replace(/\.[^/.]+$/, "");
            app.setArbitrationGraphFromYaml(data);
        });
}

uploadFile = function () {
    let files = input.files;
    if (files.length == 0) return;
    const file = files[0];

    let reader = new FileReader();
    reader.onload = (e) => {
        const data = e.target.result;

        app.offlineMode = true;
        app.status = file.name + " loaded";
        app.fileName = file.name.replace(/\.[^/.]+$/, "");
        app.setArbitrationGraphFromYaml(data);
    };
    reader.onerror = (e) => alert(e.target.error.name);
    reader.readAsText(file);
}

loadSvgFilters = function () {
    fetch("svg_filters.xml")
        .then(response => response.text())
        .then(data => {
            app.svgFilters = data;
        });
}


loadSvgFilters()

let input = document.querySelector('#file-upload-form input');
input.addEventListener('change', uploadFile);

const urlParams = new URLSearchParams(window.location.search);
const topic = urlParams.get('topic');
const headless = urlParams.get('headless');

if (headless == 'true') {
    app.headless = true;
    app.hideTitleButtonsAndStatus();
}
