/******************************************************************************

  Copyright (C), 2020, ArashVision

 ******************************************************************************
  File Name     : ins_nnie_interface.h
  Version       : V1.0
  Author        : yangfei
  Created       : 2020
  Description   :
******************************************************************************/
#ifndef INS_NNIE_INTERFACE_H
#define INS_NNIE_INTERFACE_H

#include <stdio.h>
#include "Tensor.h"
#include "sample_comm_nnie.h"
#include "ins_nnie_c_interface.h"

#define MAX_OUTPUT_NUM 5

class NNIE
{
public:
    NNIE();
    ~NNIE();

public:
    void init(const char *model_path, const int image_height = 416, const int image_width = 416);
    void run(const char *file_path);
    void run(const unsigned char *data);
    void finish();
    Tensor getOutputTensor(int index);

    int getWidth();
    int getHeight();

    void setCallBack(CNNIE_RESULT_CB callback){
_result_callback = callback;
    }

CNNIE_RESULT_CB getCallBack(){
return _result_callback;
}
    

protected:
    const char *_file_path;
    const unsigned char *_data;
    const char *model_path_;
    int image_height_;
    int image_width_;
    Tensor output_tensors_[MAX_OUTPUT_NUM];

    SAMPLE_SVP_NNIE_MODEL_S s_stModel_ ;
    SAMPLE_SVP_NNIE_PARAM_S s_stNnieParam_ ;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg_ ;

    CNNIE_RESULT_CB _result_callback;
};

#endif