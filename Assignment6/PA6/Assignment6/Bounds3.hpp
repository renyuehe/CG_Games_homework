//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_BOUNDS3_H
#define RAYTRACING_BOUNDS3_H
#include "Ray.hpp"
#include "Vector.hpp"
#include <limits>
#include <array>

class Bounds3
{
  public:
    //用两个三维空间的点（向量） 来指定 boudning box
    Vector3f pMin, pMax; // two points to specify the bounding box

    Bounds3()
    {
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        pMax = Vector3f(minNum, minNum, minNum);
        pMin = Vector3f(maxNum, maxNum, maxNum);
    }
    
    Bounds3(const Vector3f p) : pMin(p), pMax(p) {}
    Bounds3(const Vector3f p1, const Vector3f p2)
    {
        //输入任意一对角线上两个点，经过矫正后，一定是 xyz都是min 和 xyz都是max的两个点。
        pMin = Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
        pMax = Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
    }

    //从'左上角'-->'右下角',对角线向量
    Vector3f Diagonal() const { 
        return pMax - pMin; 
    }

    //求最长的轴，0：x， 1：y， 2：z
    int maxExtent() const
    {
        //对角线
        Vector3f d = Diagonal();


        if (d.x > d.y && d.x > d.z)//x最长
            return 0;
        else if (d.y > d.z)//因为已经确定了 x表示不是最大的，也就说明 y 和 z 有一个是最大的，y>z故y最长
            return 1;
        else
            return 2;
    }

    //表面积
    double SurfaceArea() const
    {
        Vector3f d = Diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    //求盒子的中心点
    Vector3f Centroid() { 
        return 0.5 * pMin + 0.5 * pMax; 
    }

    //求两个盒子相交的盒子，也是一个盒子
    Bounds3 Intersect(const Bounds3& b)
    {
        return Bounds3(Vector3f(fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y), fmax(pMin.z, b.pMin.z)),
                       Vector3f(fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y), fmin(pMax.z, b.pMax.z)));
    }


    Vector3f Offset(const Vector3f& p) const
    {
        Vector3f o = p - pMin;//

        if (pMax.x > pMin.x)
            o.x /= pMax.x - pMin.x;//o.x相对于bounds3.x的一个比例, 也就是相对于'左上角'的一个篇一量
        if (pMax.y > pMin.y)
            o.y /= pMax.y - pMin.y;//y同理
        if (pMax.z > pMin.z)
            o.z /= pMax.z - pMin.z;//z同理
        return o;
    }

    //是否重叠
    bool Overlaps(const Bounds3& b1, const Bounds3& b2)
    {
        bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);//x轴上有交集
        bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);//y轴上有交集
        bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);//z轴上有交集

        return (x && y && z);//x、y、z上都有交集，表示这两个 bounding box 确实是有交集的
    }

    //p 是否在 盒子b 内部
    bool Inside(const Vector3f& p, const Bounds3& b)
    {
        //判断这个 p 这个点 是否是在 b bounding box 的内部
        return (p.x >= b.pMin.x && p.x <= b.pMax.x && 
                p.y >= b.pMin.y && p.y <= b.pMax.y && 
                p.z >= b.pMin.z && p.z <= b.pMax.z);
    }
    
    //'左上角' 和 '右下角' 运算符 []
    inline const Vector3f& operator[](int i) const
    {
        //bounds3[0]返回 pMin, bounds3[1]返回 pMax
        return (i == 0) ? pMin : pMax;
    }

    inline bool IntersectP(const Ray& ray, 
                           const Vector3f& invDir,
                           const std::array<int, 3>& dirisNeg) const;
};


//AABBs 光线 和 盒子 是否相交
inline bool Bounds3::IntersectP(const Ray& ray, 
                                const Vector3f& invDir,
                                const std::array<int, 3>& dirIsNeg) const
{
    // invDir 是光线的倒数，目的是用乘法代替出发，这样更快
    // invDir: ray direction(x,y,z), invDir=(1.0/x,1.0/y,1.0/z), use this because Multiply is faster that Division
    // dirIsNeg：通过这个可以快速的判断光线的方向逻辑
    // dirIsNeg: ray direction(x,y,z), dirIsNeg=[int(x>0),int(y>0),int(z>0)], use this to simplify your logic
    // TODO test if ray bound intersects

    //invDir = 1 / D; t = (Px - Ox) / dx
    //分别表示3个面
    //每个面分别表示 2个轴上的 光到达该面的时间。
    float t_Min_x = (pMin.x - ray.origin.x)*invDir[0];
    float t_Max_x = (pMax.x - ray.origin.x)*invDir[0];

    float t_Min_z = (pMin.z - ray.origin.z)*invDir[2];
    float t_Max_z = (pMax.z - ray.origin.z)*invDir[2];
    
    float t_Min_y = (pMin.y - ray.origin.y)*invDir[1];
    float t_Max_y = (pMax.y - ray.origin.y)*invDir[1];
    

    //如果发现射线的方向是反的，调换t_min和t_max的位置。
    if(dirIsNeg[0])
    {
        float t = t_Min_x;
        t_Min_x = t_Max_x;
        t_Max_x = t;
    }
    if(dirIsNeg[1])
    {
        float t = t_Min_y;
        t_Min_y = t_Max_y;
        t_Max_y = t;
    }
    if(dirIsNeg[2])
    {
        float t = t_Min_z;
        t_Min_z = t_Max_z;
        t_Max_z = t;
    }
 
    //光线进入的时间，最晚进入的时间
    float t_enter = std::max(t_Min_x, std::max(t_Min_y, t_Min_z));
    //光线离开的时间，最早离开的时间
    float t_exit  = std::min(t_Max_x, std::min(t_Max_y, t_Max_z));

    //如果离开时间大于 进入时间 且 离开时间 >= 0，表示确实相交了
    //否则没有相交
    if(t_enter < t_exit && t_exit >= 0)
        return true;
    else
        return false;
}

//求并集（盒子 并 盒子）
inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2)
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b1.pMin, b2.pMin);
    ret.pMax = Vector3f::Max(b1.pMax, b2.pMax);
    return ret;
}

//求并集（盒子 并 点）
inline Bounds3 Union(const Bounds3& b, const Vector3f& p)
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b.pMin, p);
    ret.pMax = Vector3f::Max(b.pMax, p);
    return ret;
}

#endif // RAYTRACING_BOUNDS3_H
