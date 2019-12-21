cd

#rm sac-disk.bin
dd if=/dev/zero of=sac-disk.bin bs=4096 count=262144

cd tp-2019-2c-Pasaron-cosas/sac-formatter/Debug/
make clean
make
./sac-formatter -f /home/utnso/sac-disk.bin

cd