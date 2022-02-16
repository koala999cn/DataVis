#pragma once
#include "KcSampled1d.h"
#include <string>


class KcAudio : public KcSampled1d
{
public:
    using KcSampled1d::KcSampled1d; // 继承构造函数

    // 从文件加载音频，成功返回空字符串，失败返回错误信息
    std::string load(const std::string& path);


    // 保存音频到文件，成功返回空字符串，失败返回错误信息
    // quality代表音频编码品质，从0到4，品质逐渐升高
    std::string save(const std::string& path, int quality = 2);
};


