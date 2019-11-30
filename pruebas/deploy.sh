cd

git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library/
sudo make install
cd

git clone https://github.com/sisoputnfrba/hilolay.git
cd hilolay/
make && sudo make install
cd

cd tp-2019-2c-Pasaron-cosas/biblioteca/Debug/
make clean
make
cd

cd tp-2019-2c-Pasaron-cosas/biblioteca/Debug/
sudo cp libbiblioteca.so /usr/lib/
cd

#cd tp-2019-2c-Pasaron-cosas/suse/Debug/
#make clean
#make
#cd

cd tp-2019-2c-Pasaron-cosas/muse/Debug/
make clean
make
cd

#cd workspace/tp-2019-2c-Pasaron-cosas/sac-server/Debug/
#make clean
#make
#cd