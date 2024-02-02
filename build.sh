#! /bin/sh

SCRIPT_DIR=$( dirname -- "$( readlink -f -- "$0"; )"; )

echo "[MAKE BOOTSTRAP] making Bootstrap-G dylibs..(1/X)"
# Gonna build choma first
cd "$SCRIPT_DIR"
cd Bootstrap/include/choma
make clean && make
cd ../libs

cd launchdhooker
make clean && make

cd ../SBtools/sbtool
make clean && make

cd ../sbhooker
make clean && make

echo "[MAKE BOOTSTRAP] made dylibs, moving them now (2/X)"

cd ../../launchdhooker

mv .theos/obj/debug/launchdhooker.dylib launchdhooker.dylib
if [ -e launchdhooker.dylib ]
then
rm -rf .theos

cd ../SBtools/sbtool
mv .theos/obj/debug/SBTool SBTool
if [ -e SBTool ]
then
rm -rf .theos

cd ../sbhooker
mv .theos/obj/debug/SBHooker.dylib SBHooker.dylib
if [ -e SBHooker.dylib ]
then

rm -rf .theos
# continue from here
echo "[MAKE BOOTSTRAP] dylibs moved successfully, running [COPY BOOTSTRAP]"
cd "$SCRIPT_DIR"
chmod ++x copy.sh
./copy.sh

else
echo "[MAKE BOOTSTRAP] ERR: SBHooker.dylib wasn't moved correctly"
exit
fi

else
echo "[MAKE BOOTSTRAP] ERR: SBTool wasn't moved correctly"
exit
fi

else
echo "[MAKE BOOTSTRAP] ERR: launchdhooker.dylib wasn't moved correctly"
exit
fi
