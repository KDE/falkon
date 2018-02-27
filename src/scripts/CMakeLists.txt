function(install_python_script name)
    if (ENABLE_PYTHON_PLUGINS)
        install(DIRECTORY ${name} DESTINATION "${FALKON_INSTALL_PLUGINDIR}/python")
        install(FILES i18n.py DESTINATION "${FALKON_INSTALL_PLUGINDIR}/python/${name}")
    endif()
endfunction()

install_python_script(hellopython)
