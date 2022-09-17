#!/bin/bash
set -e
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.."

find /usr/local/musl -mindepth 2 -type f -executable \( -name 'musl-gcc' -or -name 'musl-clang' \) -printf '%P\0' | \
while IFS= read -r -d '' filename; do
	make -B CC="/usr/local/musl/${filename}" OUTNAME="crc64-$(grep -Po '^[^/\\]+' <<< "${filename}")"
done

printf "\033[1;32m\nBuild completed successfully.\033[0m\n\n"
