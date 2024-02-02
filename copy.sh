#!/bin/sh

#  copy.sh
#  
#
#  Created by Chris Coding on 2/2/24.
#

echo "[COPY BOOTSTRAP] making lib folder in .app.. (3/X)"

mkdir .theos/obj/debug/install_Bootstrap/Applications/Bootstrap.app/include
mkdir .theos/obj/debug/install_Bootstrap/Applications/Bootstrap.app/include/libs
mkdir .theos/obj/debug/install_Bootstrap/Applications/Bootstrap.app/include/libs/SBtools

echo "[COPY BOOTSTRAP] lib folder created, copying over dylibs (4/X)"

cp Bootstrap/include/libs/launchdhooker/launchdhooker.dylib .theos/obj/debug/install_Bootstrap/Applications/Bootstrap.app/include/libs/launchdhooker.dylib

cp Bootstrap/include/libs/SBtools/sbtool/SBTool .theos/obj/debug/install_Bootstrap/Applications/Bootstrap.app/include/libs/SBtools/SBTool

cp Bootstrap/include/libs/SBtools/sbhooker/SBHooker.dylib .theos/obj/debug/install_Bootstrap/Applications/Bootstrap.app/include/libs/SBHooker.dylib

# we can just check if one of them exists then we'll be good
if [ -e .theos/obj/debug/install_Bootstrap/Applications/Bootstrap.app/include/libs/launchdhooker.dylib ]
then
echo "[COPY BOOTSTRAP] dylibs have been copied over!"

else
echo "[COPY BOOTSTRAP] ERR: dylibs were unable to be copied over"
exit
fi
