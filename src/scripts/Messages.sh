#! /bin/sh

XGETTEXT_FLAGS_PYTHON="\
--copyright-holder=This_file_is_part_of_KDE \
--msgid-bugs-address=http://bugs.kde.org \
--from-code=UTF-8 \
-L Python \
-ki18n:1 -ki18np:1,2 \
"

python_scripts="
hellopython
runaction
"

for script in $python_scripts; do
    $XGETTEXT_PROGRAM $XGETTEXT_FLAGS_PYTHON `find $script -name '*.py'` -o $podir/falkon_$script.pot
done
