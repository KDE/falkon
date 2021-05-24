# Falkon

Falkon is a KDE web browser. It uses QtWebEngine rendering engine.

![image](https://www.falkon.org/images/screenshot.png)

## Downloads

Falkon downloads are available from [homepage](https://www.falkon.org/download/).

## Building

```sh
mkdir build && cd build
cmake ..
make && make install
```

### Install to custom prefix

When installing Falkon to custom prefix, you may need to adjust `XDG_DATA_DIRS` environment variable.

```sh
# Build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/falkon

# Run
export XDG_DATA_DIRS="$HOME/falkon/share:$XDG_DATA_DIRS"
$HOME/falkon/bin/falkon
```

## Contributing

Want to contribute? Great!

[KDE Community Wiki](https://community.kde.org/Get_Involved)

Code review is done on [Phabricator](https://community.kde.org/Infrastructure/Phabricator). When sending patches, add Falkon project (`#Falkon`) as a reviewer.

#### Reporting bugs

You can report any bugs or feature request in KDE [bugzilla](https://bugs.kde.org/enter_bug.cgi?product=Falkon). Before reporting, please make sure your issue isn’t already reported ([open issues](https://bugs.kde.org/buglist.cgi?bug_status=UNCONFIRMED&bug_status=CONFIRMED&bug_status=ASSIGNED&bug_status=REOPENED&component=extensions&component=general&list_id=1597725&product=Falkon)).

#### Contact

You can get in contact with developers using [mailing list](https://mail.kde.org/mailman/listinfo/falkon) or __IRC__ `#falkon` at `irc.libera.chat`.
