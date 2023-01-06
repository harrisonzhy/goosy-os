SYSTEM_HEADER_PROJECTS="libc kernel"
PROJECTS="libc kernel"

export MAKE=${MAKE:-make}
export HOST=${HOST:-$(./default-host.sh)}

export AR=${HOST}-ar
export AS=${HOST}-as
export CC=${HOST}-gcc

# TODO: change folders
export PREFIX=/usr
export EXEC_PREFIX=$PREFIX
export LIBDIR=$EXEC_PREFIX/lib
export INCLUDEDIR=$PREFIX/include

export CFLAGS='-02 -g'
export CPPFLAGS=''

# cross compiler should use system root
export SYSROOT=$(pwd)/sysroot
export CC="$CC --sysroot=$SYSROOT"

# workaround since elf gcc targets don't have an include dir
if echo "HOST" | grep -Eq -- '-elf($|-)'; then
    export CC="$CC -isystem=$INCLUDEDIR"
fi