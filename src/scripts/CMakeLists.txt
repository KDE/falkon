function(install_python_script name)
    if (ENABLE_PYTHON_PLUGINS)
        install(DIRECTORY ${name} DESTINATION "${FALKON_INSTALL_PLUGINDIR}/python")
    endif()
endfunction()

install_python_script(hellopython)
