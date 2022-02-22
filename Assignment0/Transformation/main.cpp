#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>
#include <algorithm>

int main()
{
    // Basic Example of cpp
    std::cout << "Example of cpp \n";
    float a = 1.0, b = 2.0;
    std::cout << a << std::endl;
    std::cout << a/b << std::endl;
    std::cout << std::sqrt(b) << std::endl; // std::sqrt 平方根
    std::cout << "acos(-1) = " << std::acos(-1) << std::endl; // std::acos
    std::cout << std::sin(30.0/180.0*acos(-1)) << std::endl; // std::sin  

    // Example of vector
    std::cout << "Example of vector \n";
    // vector definition
    Eigen::Vector3f v(1.0f,2.0f,3.0f); //一个三维向量
    Eigen::Vector3f w(1.0f,0.0f,0.0f);

    // vector output
    std::cout << "Example of output \n";
    std::cout << v << std::endl;
    // vector add
    std::cout << "Example of add \n";
    std::cout << v + w << std::endl; 
    // vector scalar multiply
    std::cout << "Example of scalar multiply \n";
    std::cout << v * 3.0f << std::endl; 
    std::cout << 2.0f * v << std::endl; 

    // Example of matrix
    std::cout << "Example of matrix \n";
    // matrix definition
    Eigen::Matrix2f X2; // 这里a是2x3的矩阵，分配了空间没有进行初始化；
    X2(0,0) = 1.0;
    X2(0,1) = 1.0;
    X2(1,0) = 1.0;
    X2(1,1) = 2.0;

    Eigen::MatrixXf XX(2,3); // 这里a是2x3的矩阵，分配了空间没有进行初始化；
    XX(0,0) = 1.0;
    XX(0,1) = 1.0;
    XX(0,2) = 1.0;
    XX(1,0) = 2.0;
    XX(1,1) = 2.0;
    XX(1,2) = 2.0;
    // XX << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0; //逗号初始化
    std::cout << XX << std::endl;
    Eigen::Matrix3f i,j;// i, j 是3x3的矩阵，分配了float[9]的空间但是没有进行初始化
    i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    j << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;
    // matrix output
    std::cout << "Example of output \n";
    std::cout << i << std::endl << std::endl;
    std::cout << j << std::endl << std::endl;
    
    
    
    
    // matrix add i + j
    std::cout << "i + j = " << std::endl << i + j << std::endl << std::endl; 
    // matrix scalar multiply i * 2.0
    std::cout << "i * 2.0 = " << std::endl << i * 2.0 << std::endl << std::endl; 
    // matrix multiply i * j 
    std::cout << "i * j = " << std::endl << i * j << std::endl << std::endl;
    // matrix multiply vector i * v
    std::cout << "i * v(vector) = " << std::endl << i * j << std::endl << std::endl;

    Eigen::Matrix4f model;
	model << 1, 2, 3, 4, 
               2, 2, 3, 4, 
               3, 2, 3, 4, 
               4, 2, 3, 4;
	std::cout << "model : \n"<< model << std::endl;


    /* 
    * PA 0
    */
    // TO DO: Define point P
    // TO DO: Define rotation matrix M
    // TO DO: M * P
    double angle = 90.0/ (180 / 3.1415926); //弧度   

	Eigen::Vector2f p;
	p << 1.0, 0.0;
	Eigen::Matrix2f r;
	r << cos(angle), sin(angle), 
        -sin(angle), cos(angle);
	std::cout << "PA 0   rotate" << std::endl;
	std::cout << r<< std::endl;
	std::cout << r * p << std::endl;



    std::cout << "--------- rotate 3d matrix:" << std::endl;
    Eigen::Matrix3f rx; 
    rx << 1, 0, 0,
          0, cos(angle), -sin(angle),
          0, sin(angle), cos(angle);
    Eigen::Matrix3f ry;
    ry << cos(angle), 0, -sin(angle),
          0, 1, 0,
          sin(angle), 0, cos(angle);
    Eigen::Matrix3f rz;
    rz << cos(angle), -sin(angle), 0,
          sin(angle), cos(angle), 0,
          0, 0, 1;


    Eigen::Vector3f p2;
    p2 << 1.0, 0.0, 0.0;
    std::cout << "p2 沿着x轴旋转90度: \n" << rx * p2 << std::endl; // 1, 0, 0 绕着 x 旋转 还是 x
    std::cout << "p2 沿着y轴旋转90度: \n" << ry * p2 << std::endl; // 0, 0, 1 x 绕着 y 旋转 90度 就到了 z
    std::cout << "p2 沿着z轴旋转90度: \n" << rz * p2 << std::endl; // 0, 1, 0
    

    std::cout << "------------- Homogenous Coordinates: -------------" << std::endl;
    Eigen::Matrix4f rx44; 
    rx44 << 1,          0,           0, 0,
          0, cos(angle), -sin(angle), 0,
          0, sin(angle),  cos(angle), 0,
          0,           0,          0, 1;
    Eigen::Matrix4f ry44;
    ry44 << cos(angle), 0, -sin(angle), 0,
          0,          1,           0, 0,
          sin(angle), 0,  cos(angle), 0,
          0,          0,           0, 1;
    Eigen::Matrix4f rz44;
    rz44 << cos(angle), -sin(angle), 0, 0,
          sin(angle),  cos(angle), 0, 0,
          0,                    0, 1 ,0,
          0,                    0, 0, 1;

    Eigen::Vector4f p4;
    p4 << 1.0, 0.0, 0.0, 1.0;
    std::cout << "p2 沿着x轴旋转90度: \n" << rx44 * p4 << std::endl; // 1, 0, 0 绕着 x 旋转 还是 x
    std::cout << "p2 沿着y轴旋转90度: \n" << ry44 * p4 << std::endl; // 0, 0, 1 x 绕着 y 旋转 90度 就到了 z
    std::cout << "p2 沿着z轴旋转90度: \n" << rz44 * p4 << std::endl; // 0, 1, 0


      // std::cout << "------------罗德里格斯旋转公式--------------" << std::endl;
      // Eigen::Vector3f pp(1, 0, 0);//被旋转的向量
      // Eigen::Vector3f rotation_axis(1,0,0);//旋转轴
      // float rotation_angle = 90;//旋转角度
      // float radian = rotation_angle/180.0 * 3.1415926;//弧度

      // Eigen::Matrix3f rotation_axis_mm;//旋转轴叉乘的矩阵形式
      // rotation_axis_mm << 0, -rotation_axis[2],  rotation_axis[1],
      //                   rotation_axis[2], 0,  -rotation_axis[0],
      //                   -rotation_axis[1], -rotation_axis[0], 0; 

      // Eigen::Matrix3f unit_mm;
      // unit_mm <<  1,0,0, 
      //             0,1,0, 
      //             0,0,1;

      // Eigen::Matrix3f rodrigues_r_matrix;//罗德里格斯旋转矩阵
      
      // //通过  旋转弧度、 旋转轴 计算旋转矩阵
      // rodrigues_r_matrix = cos(radian) * unit_mm
      //                   + (1-cos(radian))*rotation_axis * rotation_axis.transpose() 
      //                   + sin(radian) * rotation_axis_mm;

      // std::cout << "旋转前:\n" << pp << std::endl;
      // pp = rodrigues_r_matrix * pp;//旋转
      // std::cout << "旋转后:\n" << pp << std::endl;



      std::cout << "------------罗德里格斯旋转公式 Homogenous Coordinate 形式--------------" << std::endl;
      Eigen::Vector4f pp(0, 1, 0, 1);//被旋转的向量
      Eigen::Vector4f rotation_axis(1, 0, 0, 0);//旋转轴
      float rotation_angle = 90;//旋转角度
      float radian = rotation_angle/180.0 * 3.1415926;//弧度

      Eigen::Matrix4f rotation_axis_mm;//旋转轴叉乘的矩阵形式
      rotation_axis_mm << 0, -rotation_axis[2],  rotation_axis[1], 0,
                        rotation_axis[2], 0,  -rotation_axis[0],   0,
                        -rotation_axis[1], -rotation_axis[0], 0,   0,
                        0, 0, 0, 0; 

      Eigen::Matrix4f unit_mm;
      unit_mm <<  1,0,0, 0,
                  0,1,0, 0,
                  0,0,1, 0,
                  0,0,0, 0;

      Eigen::Matrix4f rodrigues_r_matrix;//罗德里格斯旋转矩阵
      

      std::cout << "cos(radian) * unit_mm:\n" << cos(radian) * unit_mm << std::endl;
      std::cout << "(1-cos(radian))*rotation_axis * rotation_axis.transpose() :\n" << (1-cos(radian))*rotation_axis * rotation_axis.transpose()  << std::endl;
      std::cout << "sin(radian) * rotation_axis_mm:\n" << sin(radian) * rotation_axis_mm << std::endl;
      //通过  旋转弧度、 旋转轴 计算旋转矩阵
      rodrigues_r_matrix = cos(radian) * unit_mm
                        + (1-cos(radian))*rotation_axis * rotation_axis.transpose() 
                        + sin(radian) * rotation_axis_mm;

      rodrigues_r_matrix(3,3) = 1.0;//homogenous特性,这里强制设置为1,不通用
      

      std::cout << "rodrigues_r_matrix:\n" << rodrigues_r_matrix << std::endl;
      std::cout << "homogenous coordinate 旋转前:\n" << pp << std::endl;
      pp = rodrigues_r_matrix * pp;//旋转
      std::cout << "homogenous coordinate 旋转后:\n" << pp << std::endl;

    return 0;
}
