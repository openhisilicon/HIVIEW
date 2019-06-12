#!/bin/sh


# (normal mode)
ir_cut_enable()
{
	# pin_mux
	himm 0x200f01a0 0x0		# GPIO15_0
	himm 0x200f01a4 0x0		# GPIO15_1
	
	# dir
	himm 0x20260400	0x3
	# data, GPIO15_0: 0, GPIO15_1: 1  (normal mode)
	himm 0x2026000c 0x2
	
	# back to original 
	sleep 1;
	himm 0x2026000c 0x0
}

# (ir mode)
ir_cut_disable()
{
	# pin_mux
	himm 0x200f01a0 0x0		# GPIO15_0
	himm 0x200f01a4 0x0		# GPIO15_1
	
	# dir
	himm 0x20260400	0x3
	# data, GPIO15_0: 1, GPIO15_1: 0  (ir mode)
	himm 0x2026000c 0x1
	
	# back to original 
	sleep 1
    himm 0x2026000c 0x0
}


ir_light_on()
{
    # pin_mux
    himm 0x200f00e0 0x0     # GPIO0_3

    # dir 
    himm 0x20140400 0xa     

    # data
    himm 0x20140020 0x0
}

ir_light_off()
{
    # pin_mux
    himm 0x200f00e0 0x0     # GPIO0_3

    # dir 
    himm 0x20140400 0xa     

    # data
    himm 0x20140020 0x8
}

isp_ai_pin_mux()
{
    # PWM4
    himm 0x200F00A4 0x2;
}


isp_ai_pwm_config()
{
    # 50MHz
    himm 0x20030038 0x6;
}

if [ $# -eq 0 ]; then
    echo "normal mode: ./demo_config 0";
    echo "ir mode    : ./demo_config 1";
else
    if [ $1 -eq 0 ]; then
        echo "normal mode, ir light off"
        ir_cut_enable > /dev/null;
        ir_light_off  > /dev/null;
    fi

    if [ $1 -eq 1 ]; then
        echo "ir mode, ir light on"
        ir_cut_disable > /dev/null ;
        ir_light_on    > /dev/null;
    fi
fi

isp_ai_pin_mux;
isp_ai_pwm_config;
