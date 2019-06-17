#!/bin/sh

#(normal mode)
ir_cut_enable()
{
	# pin_mux
	himm 0x120400A0 0x0		# GPIO5_6
	himm 0x1204009C 0x0		# GPIO5_5
	
	# dir
	himm 0x12145400	0x60
	# data, GPIO5_6: 0, GPIO5_5: 1  (normal mode)
	himm 0x12145180 0x20		# GPIO5_6 GPIO5_5
	
	# back to original 
	sleep 1;
	himm 0x12145180 0x0		# GPIO5_6 GPIO0_5
}

# (ir mode)
ir_cut_disable()
{
	# pin_mux
	himm 0x120400A0 0x0		# GPIO5_6
	himm 0x1204009C 0x0		# GPIO5_5
	
	# dir
	himm 0x12145400	0x60
	# data, GPIO5_6: 1, GPIO5_5: 0  (ir mode)
	himm 0x12145180 0x40		# GPIO5_6 GPIO5_5
	
	# back to original 
	sleep 1;
	himm 0x12145180 0x0		# GPIO5_6 GPIO5_5
}

isp_ai_pin_mux()
{
    # PWM4
	himm 0x12040098 0x3;
}


isp_ai_pwm_config()
{
    # 50MHz
	 himm 0x120100DC 0x6;
}

if [ $# -eq 0 ]; then
    echo "ir mode : ./demo_config.sh 1";
else
    if [ $1 -eq 0 ]; then
        echo "normal mode, ir_cut on"
        ir_cut_enable > /dev/null;
    fi

    if [ $1 -eq 1 ]; then
        echo "ir mode, ir_cut off"
        ir_cut_disable > /dev/null ;
    fi
fi

isp_ai_pin_mux;
isp_ai_pwm_config;