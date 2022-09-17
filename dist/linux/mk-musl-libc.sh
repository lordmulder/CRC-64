#!/bin/bash
# sudo apt install curl crossbuild-essential-{i386,armel,armhf,arm64,mipsel,mips64el}
set -e
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

function mk_musl() {
	printf "\033[1;36m\nBuilding musl-libc for: ${1}\033[0m\n\n"
	local outdir="/usr/local/musl/${1}"
	local build="musl-build-${1}"
	rm -rf "${build}" && mkdir -p "${build}"
	tar -xvf "musl-latest.tar.gz" --strip-components=1 -C "${build}"
	pushd "${build}"
	local optdirs="$(find './src' -mindepth 1 -maxdepth 1 -type d -printf '%f,' | sed 's/,$//g')"
	CFLAGS="${3}" ./configure --enable-optimize="${optdirs}" --disable-shared --prefix="${outdir}" ${2:+--host=$2}
	make
	sudo rm -rf "${outdir}"
	sudo make install
	popd
}

if [ "$(gcc -dumpmachine)" != "x86_64-linux-gnu" ]; then
	echo "This script is supposed to run on the native \"x86_64-linux-gnu\" platform !!!"
	exit 1
fi

curl -vkf -o "musl-latest.tar.gz" "https://musl.libc.org/releases/musl-latest.tar.gz"

mk_musl x86_64 "" "-march=x86-64 -mtune=znver3"
mk_musl i686 i686-linux-gnu "-march=i586 -mtune=pentium2"
mk_musl armel arm-linux-gnueabi
mk_musl armhf arm-linux-gnueabihf
mk_musl arm64 aarch64-linux-gnu
mk_musl mipsel mipsel-linux-gnu
mk_musl mips64el mips64el-linux-gnuabi64

printf "\033[1;32m\nBuild completed successfully.\033[0m\n\n"
