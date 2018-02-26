function(install_python_script name)
    install(DIRECTORY ${name} DESTINATION "${FALKON_INSTALL_PLUGINDIR}/python")
endfunction()

install_python_script(hellopython)
