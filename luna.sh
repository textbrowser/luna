#!/usr/bin/env sh

# Alexis Megas.

export AA_ENABLEHIGHDPISCALING=1
export AA_USEHIGHDPIPIXMAPS=1
export QT_AUTO_SCREEN_SCALE_FACTOR=1
export QT_QPA_PLATFORMTHEME=qt6ct
export QT_X11_NO_MITSHM=1

if [ -f ./Luna ] && [ -r ./Luna ] && [ -x ./Luna ]
then
    echo "Launching a local Luna."
    ./Luna "$@"
    exit $?
fi

if [ -f /opt/luna/Luna ] && [ -r /opt/luna/Luna ] && [ -x /opt/luna/Luna ]
then
    echo "Launching an official Luna (/opt/luna)."
    /opt/luna/Luna "$@"
    exit $?
fi

echo "Cannot locate Luna. Why?"
exit 1
