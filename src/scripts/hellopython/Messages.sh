#! /bin/sh

XGETTEXT_FLAGS_PYTHON="\
--copyright-holder=This_file_is_part_of_KDE \
--msgid-bugs-address=https://bugs.kde.org \
--from-code=UTF-8 \
-L Python \
-ki18n:1 -ki18np:1,2 \
"

$XGETTEXT_PROGRAM $XGETTEXT_FLAGS_PYTHON `find . -name '*.py'` -o $podir/falkon_hellopython.pot
