Liunx:

1. Compile:
	only compile uvc default, if need uac, please change the value of "UAC_COMPILE" into "y" in Makefile.linux.param file.
	$ make clean
	$ make



LiteOS:

1. Adaptation and Modification
	1.1 Modifying the Sample
		1.1.1 Set the UVC transmission capability set
			modify the mpp/sample/liteos/app_init.c file, Call the function "fuvc_frame_descriptors_set" before "usb_init":
				extern void fuvc_frame_descriptors_set(void);
				fuvc_frame_descriptors_set();
				usb_init(HOST, 0);

		1.1.2 Changing the USB Device Type
		    1) use UVC:
			In the mpp/sample/liteos directory, modify the app_init.c file as follows:
				usb_init(DEVICE, DEV_UVC); (or usb_init(DEVICE, DEV_CAMERA);)
			In the mpp/sample/uvc_app directory, modify the application.c file as follows:
				unsigned int g_uvc = 1;
				unsigned int g_uac = 0;
			2) use UAC:
			In the mpp/sample/liteos directory, modify the app_init.c file as follows:
				usb_init(DEVICE, DEV_UAC);
			In the mpp/sample/uvc_app directory, modify the application.c file as follows:
				unsigned int g_uvc = 0;
				unsigned int g_uac = 1;
			3) use UVC and UAC:
			In the mpp/sample/liteos directory, modify the app_init.c file as follows:
				usb_init(DEVICE, DEV_CAMERA);
			In the mpp/sample/uvc_app directory, modify the application.c file as follows:
				unsigned int g_uvc = 1;
				unsigned int g_uac = 1;

		1.1.3 allocate the USB memory
			In the mpp/sample/liteos directory, modify the app_init.c file as follows:
				g_usb_mem_size = 0x100000;

	1.2 Starting the Sample
		In the mpp/sample/liteos directory, modify the app_init.c file as follows:
			Replace sample_command() with app_main(1, NULL).

2. Compilation
	2.1 Compile U-Boot.
		For details about how to compile U-Boot, see osdrv/readme_en.txt.
	2.2 Compile LiteOS.
		In the directory osdrv/platform/liteos/liteos:
			cp tools/build/config/hi3516ev200_defconfig .config
			make clean && make
	2.3 Place the sample in the mpp/sample directory and go to the uvc_app directory, such as mpp/sample/uvc_app.
		make clean;make
	After the compilation is complete, the uvc_app.bin file is generated in the directory.

3. Image Burning
	The following uses an 8 MB SPI NOR flash memory as an example. The flash address space is described as follows:
        |     1 MB    |             4 MB	    |
        |-------------|-----------------------------|
        |     U-Boot  |       uvc_app.bin	    |

        The following operations are performed based on the following allocated address space. You can change the address space as required.


	Set the U-Boot booting commands.
		setenv bootcmd 'sf probe 0;sf read 0x40000000 0x100000 0x500000;go 0x40000000';saveenv
		You may adjust the commands as required.


