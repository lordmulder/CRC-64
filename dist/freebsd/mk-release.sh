#!/bin/sh
# pkg install gmake gcc12
set -e
cd -- "$(dirname -- "$0")/../.."

gmake -B CC=gcc12 XCFLAGS="-m32" MARCH=i586 MTUNE=pentium2 OUTNAME="crc64-i686"
gmake -B CC=gcc12 XCFLAGS="-m64" MARCH=x86-64 MTUNE=znver3 OUTNAME="crc64-x86_64"

echo "Build completed successfully."
