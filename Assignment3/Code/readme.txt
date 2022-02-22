1、纹理的重心坐标？如何计算 rasterizer.cpp 中


2、Z 是啥？

    float Z = 1.0 / (alpha / v[0].w() 
                        + beta / v[1].w() 
                        + gamma / v[2].w());    //根据质心坐标计算实际的z值
    float zp =  alpha * v[0].z() / v[0].w() 
                + beta * v[1].z() / v[1].w() 
                + gamma * v[2].z() / v[2].w();
    zp *= Z;
