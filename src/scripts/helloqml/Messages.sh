#! /bin/sh

$XGETTEXT `find . -name '*.js' -o -name '*.qml'` -o $podir/falkon_helloqml.pot
