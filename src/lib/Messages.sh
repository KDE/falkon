#! /bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$EXTRACT_TR_STRINGS `find . -name '*.cpp' -o -name '*.h'` -o $podir/falkon.pot
