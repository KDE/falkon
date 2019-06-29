window.addEventListener('WebChannelMessageToChrome', function(event) {
    let e = {type: event.type, detail: event.detail};
    external.extra.communicator.receiveJSON(e);
});

function sendMessage(response) {
    let e = new window.CustomEvent('WebChannelMessageToContent',
                {detail: response}
            );
    window.dispatchEvent(e);
}
