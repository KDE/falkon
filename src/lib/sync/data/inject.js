new QWebChannel(qt.webChannelTransport, function(channel) {
    communicator = channel.objects.communicator;
    window.comm = communicator;
});

window.addEventListener('WebChannelMessageToChrome', function(event) {
    let e = {type: event.type, detail: event.detail};
    window.comm.receiveJSON(e);
});

function sendMessage(response) {
    let e = new window.CustomEvent('WebChannelMessageToContent',
                {detail: response}
            );
    window.dispatchEvent(e);
}
