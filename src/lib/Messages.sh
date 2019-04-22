#! /bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT `find . -name '*.cpp' -o -name '*.h'` -o $podir/falkon.pot
