#ifndef __h26xbits__
#define __h26xbits__

int h26x_parse_nalu(unsigned char *in, int size
		, int (*nalucb)(void *u, char *data, int type, int off, int size)
		, void *u);

int h26x_parse_sps_wh(char *data, int type, int size
    , unsigned short *width, unsigned short *height);

#endif