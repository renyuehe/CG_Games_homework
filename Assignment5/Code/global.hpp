#pragma once

#include <cmath>
#include <iostream>
#include <random>

#define M_PI 3.14159265358979323846

//获取 float 类型的最大值
constexpr float kInfinity = std::numeric_limits<float>::max();

//夹在lo和hi之间
inline float clamp(const float& lo, const float& hi, const float& v)
{
    //不能高于 hi 不能低于 lo
    return std::max(lo, std::min(hi, v));
}

//解 一元二次方程
inline bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
    /* 解一元二次方程，ax2 + bx + c */

    float discr = b * b - 4 * a * c;
    if (discr < 0)//无解
        return false;
    else if (discr == 0)//一个解
        x0 = x1 = -0.5 * b / a;
    else//两个解
    {
        float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1)//x0小，x1大
        std::swap(x0, x1);
    return true;
}

//材质
enum MaterialType
{
    DIFFUSE_AND_GLOSSY, //漫反射 和 有光泽的
    REFLECTION_AND_REFRACTION, //反射 和 折射
    REFLECTION //反射
};

//随机浮点数
inline float get_random_float()
{
    /* 获取随即浮点数 */
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]

    return dist(rng);
}

//进度条
inline void UpdateProgress(float progress)
{
    /* 进度条 */
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0);
    std::cout << " %\r";// \r 回到当前行的行首
    std::cout.flush();// 刷新行缓存（强制输出）
}
