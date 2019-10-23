/*
 * extdrv/peripheral/ada/tlv320.h for Linux .
 *
 * History: 
 *      10-April-2006 create this file
 */ 

#ifndef _INC_TLV320AIC31
#define  _INC_TLV320AIC31	

#define     IN2LR_2_LEFT_ADC_CTRL 0x0
#define     IN2LR_2_RIGTH_ADC_CTRL 0x1
#define     IN1L_2_LEFT_ADC_CTRL 0x2
#define     IN1R_2_RIGHT_ADC_CTRL 0x3
#define     PGAL_2_HPLOUT_VOL_CTRL 0x4
#define     PGAR_2_HPLOUT_VOL_CTRL 0x5
#define     DACL1_2_HPLOUT_VOL_CTRL 0x6
#define     DACR1_2_HPLOUT_VOL_CTRL 0x7
#define     HPLOUT_OUTPUT_LEVEL_CTRL 0x8 
#define     PGAL_2_HPLCOM_VOL_CTRL 0x9
#define     PGAR_2_HPLCOM_VOL_CTRL 0xa
#define     DACL1_2_HPLCOM_VOL_CTRL 0xb
#define     DACR1_2_HPLCOM_VOL_CTRL 0xc
#define     HPLCOM_OUTPUT_LEVEL_CTRL 0xd
#define     PGAR_2_HPROUT_VOL_CTRL 0xf
#define     DACR1_2_HPROUT_VOL_CTRL 0x10
#define     HPROUT_OUTPUT_LEVEL_CTRL 0x11
#define     PGAR_2_HPRCOM_VOL_CTRL 0x12
#define     DACR1_2_HPRCOM_VOL_CTRL 0X13
#define     HPRCOM_OUTPUT_LEVEL_CTRL 0x14
#define     PGAL_2_LEFT_LOP_VOL_CTRL 0x15
#define     DACL1_2_LEFT_LOP_VOL_CTRL 0x16
#define     LEFT_LOP_OUTPUT_LEVEL_CTRL 0x17
#define     PGAR_2_RIGHT_LOP_VOL_CTRL 0x18
#define     DACR1_2_RIGHT_LOP_VOL_CTRL 0x19
#define     RIGHT_LOP_OUTPUT_LEVEL_CTRL 0x20
#define     SET_ADC_SAMPLE 0x21
#define     SET_DAC_SAMPLE 0x22
#define     SET_DATA_LENGTH 0x23
#define     SET_CTRL_MODE 0x24
#define     LEFT_DAC_VOL_CTRL 0x25
#define     RIGHT_DAC_VOL_CTRL 0x26
#define     LEFT_DAC_POWER_SETUP 0x27
#define     RIGHT_DAC_POWER_SETUP 0x28
#define     DAC_OUT_SWITCH_CTRL 0x29
#define     LEFT_ADC_PGA_CTRL 0x30
#define     RIGHT_ADC_PGA_CTRL 0x31
#define     TLV320AIC31_REG_DUMP 0x32
#define     SOFT_RESET 0x33
#define     SET_TRANSFER_MODE 0x34
#define     SET_SERIAL_DATA_OFFSET 0X35

/*
0: ADC Fs = Fsref/1     48		44
1: ADC Fs = Fsref/1.5	32	
2: ADC Fs = Fsref/2		24		22
3: ADC Fs = Fsref/2.5	20
4: ADC Fs = Fsref/3		16
5: ADC Fs = Fsref/3.5	13.7
6: ADC Fs = Fsref/4		12		11
7: ADC Fs = Fsref/4.5	10.6
8: ADC Fs = Fsref/5		9.6
9: ADC Fs = Fsref/5.5
a: ADC Fs = Fsref / 6	8
*/
#define     AC31_SET_8K_SAMPLERATE	                0xa
#define     AC31_SET_12K_SAMPLERATE	                0x6
#define     AC31_SET_16K_SAMPLERATE	                0x4
#define     AC31_SET_24K_SAMPLERATE	                0x2
#define     AC31_SET_32K_SAMPLERATE	                0x1
#define     AC31_SET_48K_SAMPLERATE	                0x0

#define     AC31_SET_11_025K_SAMPLERATE	                0x6
#define     AC31_SET_22_05K_SAMPLERATE	                0x2
#define     AC31_SET_44_1K_SAMPLERATE	                0x0

#define     AC31_SET_SLAVE_MODE                      0
#define     AC31_SET_MASTER_MODE                     1

#define     AC31_SET_16BIT_WIDTH                      0
#define     AC31_SET_20BIT_WIDTH                      1
#define     AC31_SET_24BIT_WIDTH                      2
#define     AC31_SET_32BIT_WIDTH                      3		




typedef enum Audio_In_
{
    IN1L =0,
    IN1R =1,
    IN2L =2,
    IN2R =3,
}Audio_In;
typedef enum Audio_Out_
{
    LINE_OUT_R=0,
    LINE_OUT_L,
    HPL,
    HPR,
}Audio_Out;
typedef struct 
{
    unsigned int chip_num:3;
    unsigned int audio_in_out:2;
    unsigned int if_mute_route:1;
    unsigned int if_powerup:1;
    unsigned int input_level:7;
    unsigned int sample:4;
    unsigned int if_44100hz_series:1;
    unsigned int data_length:2;
    unsigned int ctrl_mode:1;
    unsigned int dac_path:2;
    unsigned int trans_mode:2;
    unsigned int reserved :6;
    unsigned int data_offset:8;
    unsigned int sampleRate;
}Audio_Ctrl;
#endif
