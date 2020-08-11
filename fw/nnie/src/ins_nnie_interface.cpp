#include "stdio.h"
#include "ins_nnie_interface.h"
#include "nnie_core.h"
#include "Tensor.h"
#include "util.h"

NNIE::NNIE()
{
}
NNIE::~NNIE()
{
    NNIE_Param_Deinit(&s_stNnieParam_, &s_stModel_);
}
void NNIE::init(const char *model_path, const int image_height = 416, const int image_width = 416)
{
    model_path_ = model_path;
    image_height_ = image_height;
    image_width_ = image_width;
    load_model(model_path, &s_stModel_);
    nnie_param_init(&s_stModel_, &stNnieCfg_, &s_stNnieParam_);
}

void NNIE::run(const char *file_path)
{

    int file_length = 0;
    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL)
    {
        printf("open %s failed\n", file_path);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    file_length = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) * file_length);

    fread(data, file_length, 1, fp);

    fclose(fp);

    NNIE_Forward_From_Data(data, &s_stModel_, &s_stNnieParam_, output_tensors_);

    free(data);
}

void NNIE::run(const unsigned char *data)
{
    NNIE_Forward_From_Data(data, &s_stModel_, &s_stNnieParam_, output_tensors_);
}

void NNIE::finish()
{
    NNIE_Param_Deinit(&s_stNnieParam_, &s_stModel_);
}

int NNIE::getWidth()
{
    return image_width_;
}

int NNIE::getHeight()
{
    return image_height_;
}

Tensor NNIE::getOutputTensor(int index)
{

    return output_tensors_[index];
}
