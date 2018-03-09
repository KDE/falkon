#! /bin/sh
$EXTRACT_TR_STRINGS `find . -name '*.cpp' -o -name '*.h' -o -name '*.ui' | grep -v -e '/plugins/' -e '/scripts/'` -o $podir/falkon_qt.pot
