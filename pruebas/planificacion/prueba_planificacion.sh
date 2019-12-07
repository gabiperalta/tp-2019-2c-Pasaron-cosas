cd

cd tp-2019-2c-Pasaron-cosas/biblioteca/Debug/
make clean
make
sudo cp libbiblioteca.so /usr/lib/
cd

cd tp-2019-2c-Pasaron-cosas/LibMuse/Debug/
make clean
make
cd

cd tp-2019-2c-Pasaron-cosas/LibMuse/Debug/
sudo cp liblibMuse.so /usr/lib/
cd

cd tp-2019-2c-Pasaron-cosas/suse/Debug/
make clean
make
cd

cp tp-2019-2c-Pasaron-cosas/pruebas/planificacion/suse.config tp-2019-2c-Pasaron-cosas/suse/src/

cd linuse-tests-programs/
make && make entrega
cd