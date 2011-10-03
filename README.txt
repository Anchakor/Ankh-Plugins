= LV2 Ankh Plugins =

Audio plugins for use with LV2 supporting hosts.

http://mud.cz/lv2/plugins
http://lv2plug.in/

Currently consists of distortion, 3-band distortion, 4-band compressor plugins.


= Dependencies =

lv2core


= Install & Uninstall =

  make
  make INSTALL_DIR=/usr/local/lib/lv2 install

  make INSTALL_DIR=/usr/local/lib/lv2 uninstall

DESTDIR is also supported by make (un)install.

  
= License =

See LICENSE.txt.

Copyright 2011 Jiří Procházka <ojirio@gmail.com> & others (specified in each file).

