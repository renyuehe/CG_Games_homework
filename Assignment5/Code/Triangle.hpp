#pragma once

#include "Object.hpp"

#include <cstring>

//MT算法:光线和三角形求交
bool rayTriangleIntersect(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& orig,
                          const Vector3f& dir, float& tnear, float& u, float& v)
{
    
    //MT算法（Moller Trumbore Algorithm）:光线和三角形求交
    // TODO: Implement this function that tests whether the triangle
    // that's specified bt v0, v1 and v2 intersects with the ray (whose
    // origin is *orig* and direction is *dir*)
    // Also don't forget to update tnear, u and v.

    // 射线三角形相交算法: O + t*D = (1 - u - v)*P0 + u*P1 + v*P2
    // 其中 O,D,P0,P1,P2 是已知量. t,u,v 是未知量
    // 根据克莱姆法则可是知道, 已知量足够解出未知量
    // 下面是
    Vector3f E1 = v1 - v0;
    Vector3f E2 = v2 - v0;
    Vector3f S = orig - v0;
    Vector3f S1 = crossProduct(dir,E2);
    Vector3f S2 = crossProduct(S,E1);

    float temp = dotProduct(E1,S1);
    if(temp == 0 || temp <0)
    return false;

    u = dotProduct(S1,S)/temp;

    v = dotProduct(dir,S2)/temp;

    tnear = dotProduct(S2,E2)/temp;

    // u,v,(1-u-v) 都 > 0 表示在三角形内
    // tnear > 0 表示光线打到表面的时间是正的
    if(u >= 0 && v >= 0 && (1 - u -v) >= -__FLT_EPSILON__ && tnear >= 0)
    {
        return true;
    }
    else
    {
        //if(u >= 0 && v >= 0 && tnear >= 0 &&(1 - u -v) < 0)
        //std::cout << u << v << tnear << std::endl;
        return false;
    }
}

class MeshTriangle : public Object
{
public:
    MeshTriangle(const Vector3f* verts, const uint32_t* vertsIndex, const uint32_t& numTris, const Vector2f* st)
    {
        /**
         * @brief verts: 网格顶点
         * @brief vertsIndex: 顶点索引,每三对表示一个三角形
         * @brief numTris: 几个三角形
         * @brief st: 网格顶点 对应的 纹理顶点
         */
        uint32_t maxIndex = 0;
        for (uint32_t i = 0; i < numTris * 3; ++i)
            if (vertsIndex[i] > maxIndex)
                maxIndex = vertsIndex[i];
        maxIndex += 1;
        vertices = std::unique_ptr<Vector3f[]>(new Vector3f[maxIndex]);
        memcpy(vertices.get(), verts, sizeof(Vector3f) * maxIndex);
        vertexIndex = std::unique_ptr<uint32_t[]>(new uint32_t[numTris * 3]);
        memcpy(vertexIndex.get(), vertsIndex, sizeof(uint32_t) * numTris * 3);
        numTriangles = numTris;
        stCoordinates = std::unique_ptr<Vector2f[]>(new Vector2f[maxIndex]);
        memcpy(stCoordinates.get(), st, sizeof(Vector2f) * maxIndex);
    }

    
    bool intersect(const Vector3f& orig, const Vector3f& dir, float& tnear, uint32_t& index,
                   Vector2f& uv) const override
    {
        /**
         * @brief MeshTriangle 和光线求交
         * @brief orig:光源位置
         * @brief dir:光的方向
         * @brief tnear:光线传播时间，需求解
         */
        bool intersect = false;
        //遍历所有的三角形的
        for (uint32_t k = 0; k < numTriangles; ++k)
        {
            //一个三角形的三个顶点
            const Vector3f& v0 = vertices[vertexIndex[k * 3]];
            const Vector3f& v1 = vertices[vertexIndex[k * 3 + 1]];
            const Vector3f& v2 = vertices[vertexIndex[k * 3 + 2]];

            float t, u, v;
            if (rayTriangleIntersect(v0, v1, v2, orig, dir, t, u, v) && t < tnear)//在三角形内部
            {
                tnear = t;
                uv.x = u;
                uv.y = v;
                index = k;
                intersect |= true;
            }
        }
        return intersect;
    }

    //获取插值：法线 和 纹理值
    void getSurfaceProperties(const Vector3f&, const Vector3f&, const uint32_t& index, const Vector2f& uv, Vector3f& N,
                              Vector2f& st) const override
    {
        /**
         * @brief 
         * index：第几个三角形索引
         * uv：重心坐标的三个系数
         * 
         */
        //求 法线
        const Vector3f& v0 = vertices[vertexIndex[index * 3]];
        const Vector3f& v1 = vertices[vertexIndex[index * 3 + 1]];
        const Vector3f& v2 = vertices[vertexIndex[index * 3 + 2]];
        Vector3f e0 = normalize(v1 - v0);//边
        Vector3f e1 = normalize(v2 - v1);//边
        N = normalize(crossProduct(e0, e1));//叉乘 求 法线

        //求 纹理插值
        const Vector2f& st0 = stCoordinates[vertexIndex[index * 3]];
        const Vector2f& st1 = stCoordinates[vertexIndex[index * 3 + 1]];
        const Vector2f& st2 = stCoordinates[vertexIndex[index * 3 + 2]];
        st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
    }

    //验证 漫反射颜色
    Vector3f evalDiffuseColor(const Vector2f& st) const override
    {
        float scale = 5;
        float pattern = (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
        return lerp(Vector3f(0.815, 0.235, 0.031), Vector3f(0.937, 0.937, 0.231), pattern);
    }

    std::unique_ptr<Vector3f[]> vertices;
    uint32_t numTriangles;
    std::unique_ptr<uint32_t[]> vertexIndex;
    std::unique_ptr<Vector2f[]> stCoordinates;
};
