set -e
. ./config.sh

# create sysroot directory
mkdir -p "$SYSROOT"

for PROJECT in $SYSTEM_HEADER_PROJECTS; do
	(cd $PROJECT && DESTDIR="SYSROOT" $MAKE install-headers)
done