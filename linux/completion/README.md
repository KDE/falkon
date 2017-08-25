**Shell completion files**

* *falkon* - bash completion file
* *_falkon* - zsh completion file

Bash completion file will be automatically installed with make install
into `/usr/share/bash-completion/completions`

To install zsh completion file, either manually copy it to proper directory
(one directory from $fpath), or run this command:

    cp _falkon "`echo $fpath | cut -d' ' -f1`/_falkon"
