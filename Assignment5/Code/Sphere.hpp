#pragma once

#include "Object.hpp"
#include "Vector.hpp"

class Sphere : public Object
{
public:
    Sphere(const Vector3f& c, const float& r)
        : center(c)
        , radius(r)
        , radius2(r * r)
    {}

    bool intersect(const Vector3f& orig, const Vector3f& dir, float& tnear, uint32_t&, Vector2f&) const override
    {
        // analytic solution
        // orig: 光源位置
        // dir: 光线传播方向向量（normalize）
        // tnear: 光线与球面相交的时间t，接受计算结果并返回

        // 球方程: (p - c)^2 - R^2 = 0
        // 其中 p是光线和球面的交点, c是球心, R是半径
        // 光线与球面交点方程: (o + td - c)^2 - R^2 ,
        // 这是一个关于 t 的一元二次方程, 可以写成 a*t^2 + b*t + c = 0 的形式
        // a: d*d
        // b:2(o-c)d
        // c:2(o-c)(o-c) - R^2
        Vector3f L = orig - center;//p
        float a = dotProduct(dir, dir);//d*d
        float b = 2 * dotProduct(dir, L);//2*(o-c)*d
        float c = dotProduct(L, L) - radius2;//R^2
        
        float t0, t1;
        if (!solveQuadratic(a, b, c, t0, t1))//通过abc解一元二次方程
            return false;
        if (t0 < 0)
            t0 = t1;
        if (t0 < 0)
            return false;
        tnear = t0;//t时间取小

        return true;
    }

    void getSurfaceProperties(const Vector3f& P, const Vector3f&, const uint32_t&, const Vector2f&,
                              Vector3f& N, Vector2f&) const override
    {
        /**
         * @brief P：光线击中球体表面的点
         */
        
        //法线
        N = normalize(P - center);
    }

    Vector3f center; //中心点
    float radius, radius2;//半径 和 半径的平方
};
