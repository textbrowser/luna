#!/usr/bin/env bash
# Alexis Megas.

if [ ! -x /usr/bin/dpkg-deb ]
then
    echo "Please install dpkg-deb."
    exit 1
fi

if [ ! -x /usr/bin/fakeroot ]
then
    echo "Please install fakeroot."
    exit 1
fi

if [ ! -e luna.pro ]
then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

make distclean 2>/dev/null
mkdir -p ./opt/luna

if [ -x /usr/bin/qmake6 ]
then
    qmake6 && make -j $(nproc)
else
    qmake && make -j $(nproc)
fi

cp -p ./Icons/luna.png ./opt/luna/.
cp -p ./Luna ./opt/luna/.
cp -p ./luna.sh ./opt/luna/.
mkdir -p luna-debian.d/opt
mkdir -p luna-debian.d/usr/share/applications
cp -p Distributions/luna.desktop luna-debian.d/usr/share/applications/.

architecture="$(dpkg --print-architecture)"

if [ $architecture = "arm64" ]
then
    cp -pr Distributions/PiOS64 luna-debian.d/DEBIAN
elif [ $architecture = "armhf" ]
then
    cp -pr Distributions/PiOS32 luna-debian.d/DEBIAN
else
    cp -pr Distributions/KUBUNTU-NATIVE luna-debian.d/DEBIAN
fi

cp -r ./opt/luna luna-debian.d/opt/.

if [ $architecture = "arm64" ]
then
    fakeroot dpkg-deb --build luna-debian.d Luna-2026.08.15_PiOS_arm64.deb
elif [ $architecture = "armhf" ]
then
    fakeroot dpkg-deb --build luna-debian.d Luna-2026.08.15_PiOS_arm32.deb
else
    fakeroot dpkg-deb --build luna-debian.d Luna-2026.08.15_Kubuntu_amd64.deb
fi

make distclean
rm -fr ./opt
rm -fr luna-debian.d
