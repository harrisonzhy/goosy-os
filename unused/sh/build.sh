set -e
. ./headers.sh
for PROJECTS in $PROJECTS; do
	(cd $PROJECT && DESTDIR="SYSROOT") $MAKE install)
done