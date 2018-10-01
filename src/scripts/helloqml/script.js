function init() {
    external.extra.helloQmlObject.clicked.connect(function(count){
        alert('You clicked ' + count + ' times')
    })
}

if (window._falkon_external) {
    init();
} else {
    document.addEventListener("_falkon_external_created", init);
}
