set( YoutubeDownload_SRCS
    yticon.cpp
    ytinterface.cpp
    ytsettings.cpp
)

set( YoutubeDownload_UIS
    ytsettings.ui
)
qt5_wrap_ui(UIS ${YoutubeDownload_UIS})

set( YoutubeDownload_RSCS
    yt.qrc
)
qt5_add_resources(RSCS ${YoutubeDownload_RSCS})

add_library(YoutubeDownload MODULE ${YoutubeDownload_SRCS} ${UIS} ${RSCS})
install(TARGETS YoutubeDownload DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(YoutubeDownload FalkonPrivate)
