This document briefly describes how to port and use alsa-lib.

1.download alsa-lib source code
	Note: The alsa-lib source package is not released by default, only dynamic library files are released. The alsa-lib source package needs to be downloaded from the open source community.
	Download the source code of alsa-lib v1.1.7 from the alsa-project open source community:
	1) Go to the website: www.alsa-project.org
	2) Select the http://www.alsa-project.org/main/index.php/Download option of the HTTP protocol resource to enter the subpage
	3) Download alsa-lib-1.1.7.tar.bz2

2.compile alsa-lib
	1) Store the downloaded alsa-lib-1.1.7.tar.bz2 in the mpp/sample/uvc_app/alsa/opensource directory.
	2) In the linux server, enter the mpp/sample/uvc_app/alsa/opensource directory and execute the following command:
	tar -xjvf alsa-lib-1.1.7.tar.bz2
	cd ./alsa-lib-1.1.7/
	mkdir -p /home/install/alsa-lib-1.1.7/
	./configure --host=arm-himix100-linux --prefix=/home/install/alsa-lib-1.1.7/ --enable-static=no --enable-shared=yes --with-configdir=/home/audio/alsa/ --disable-python
	make
	make install
	cp -r /home/install/alsa-lib-1.1.7/lib/ ../../
	cp -r /home/install/alsa-lib-1.1.7/include/ ../../

	Note: The above command is used to generate the dynamic link library of alsa-lib. --host is used to specify the cross compiler. --prefix is used to specify the installation path of the compiled file. --enable-static is used to specify static library support. --enable-shared is used to specify dynamic library support. --with-configdir is used to specify the installation path of the config file (also the path of the config file on the board).

3.install to the embedded platform
	On the target board, the following files need to be copied to the corresponding location:
	1) Copy the libasound.so.2 file in the /home/install/alsa-lib-1.1.7/lib/ directory on the server to the /lib/ directory on the board.
	2) Copy all the files in the /home/audio/alsa/ directory on the server to the /home/audio/alsa/ directory of the board (create it if it does not exist).
