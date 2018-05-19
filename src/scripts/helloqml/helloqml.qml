import org.kde.falkon 1.0 as Falkon

Falkon.PluginInterface {
    init: function(state, settingsPath){
        console.log(__name__ + " Plugin loaded")
    }
    
    testPlugin: function() {
        return true
    }
    
    unload: function() {
        console.log("Bye!")
    }
}