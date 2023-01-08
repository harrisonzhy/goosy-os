set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/goosyos.kernel isodir/boot/goosyos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "goosyos" {
	multiboot /boot/goosyos.kernel
}
EOF
grub-mkrescue -o goosyos.iso isodir