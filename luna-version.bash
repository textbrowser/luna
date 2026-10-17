#!/usr/bin/env bash

# Alexis Megas.

echo "The command sed may fail on MacOS."

VERSION=$1

if [ -z "$VERSION" ]
then
    echo "Please specify the version: $0 <VERSION>."
    exit 1
fi

for file in "Distributions/*/control"
do
    sed -i "s/Version: .*/Version: $VERSION/" $file
done

for file in "Distributions/build*"
do
    sed -i \
	"s/Luna-[[:digit:]]\+\.[[:digit:]]\+\.[[:digit:]]\+/Luna-$VERSION/" \
	$file
done

FILE="Android/AndroidManifest.xml"

if [ -r "$FILE" ]
then
    sed -i \
    's/\(android:versionCode="\)[0-9]\+\([0-9]\+\)*"/\1'"${VERSION//./}"'"/' \
    $FILE
    sed -i \
    's/\(android:versionName="\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE
fi

FILE="Source/luna-calendar.h"

sed -i \
    's/\(LUNA_VERSION_STRING "\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE
echo "Please modify ReleaseNotes.html."
