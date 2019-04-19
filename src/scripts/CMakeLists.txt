function(install_python_script name)
    if (ENABLE_PYTHON_PLUGINS)
        install(
            DIRECTORY ${name}
            DESTINATION ${FALKON_INSTALL_PLUGINDIR}
            FILES_MATCHING PATTERN "*" PATTERN "Messages.sh" EXCLUDE
        )
        install(FILES i18n.py DESTINATION "${FALKON_INSTALL_PLUGINDIR}/${name}")
    endif()
endfunction()

function(install_qml_script name)
    install(
        DIRECTORY ${name}
        DESTINATION ${FALKON_INSTALL_PLUGINDIR}
        FILES_MATCHING PATTERN "*" PATTERN "Messages.sh" EXCLUDE
    )
endfunction()

# install_python_script(hellopython)
install_python_script(runaction)
install_python_script(middleclickloader)

# install_qml_script(helloqml)
