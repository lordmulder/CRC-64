#!/bin/zsh
# xcode-select -install
set -e
cd -- "$(dirname -- "$0")/../.."

make -B MARCH= MTUNE= STATIC=0 STRIP=0 XCFLAGS="-target x86_64-apple-darwin" OUTNAME="crc64-x86_64"
make -B MARCH= MTUNE= STATIC=0 STRIP=0 XCFLAGS="-target arm64-apple-darwin"  OUTNAME="crc64-arm64"

strip crc64-*

echo "Build completed successfully."
