//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H
#include "Vector.hpp"
struct Ray{
    //Destination = origin + t*direction
    //起点
    Vector3f origin;
    //单位方向向量(_inv为倒数，为了用乘法简化除法)
    Vector3f direction, direction_inv;

    double t;//transportation time,
    double t_min, t_max; // min >= 0, max <= numeric_limits<double>

    Ray(const Vector3f& ori, const Vector3f& dir, const double _t = 0.0): origin(ori), direction(dir),t(_t) {
        direction_inv = Vector3f(1./direction.x, 1./direction.y, 1./direction.z);
        t_min = 0.0;
        t_max = std::numeric_limits<double>::max();
    }

    //计算出光线走了多远
    Vector3f operator()(double t) const{return origin+direction*t;}

    //友元运算符重载，把内容流入 ostream 以及其派生类中
    friend std::ostream &operator<<(std::ostream& os, const Ray& r)
    {
        os<<"[origin:="<<r.origin<<", direction="<<r.direction<<", time="<< r.t<<"]\n";

        return os;
    }
};
#endif //RAYTRACING_RAY_H
