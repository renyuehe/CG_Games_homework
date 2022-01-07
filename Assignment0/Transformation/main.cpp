#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>
#include <algorithm>

int main(){

    // Basic Example of cpp
    std::cout << "Example of cpp \n";
    float a = 1.0, b = 2.0;
    std::cout << a << std::endl;
    std::cout << a/b << std::endl;
    std::cout << std::sqrt(b) << std::endl; // std::sqrt 
    std::cout << "acos(-1) = " << std::acos(-1) << std::endl; // std::acos
    std::cout << std::sin(30.0/180.0*acos(-1)) << std::endl; // std::sin 

    // Example of vector
    std::cout << "Example of vector \n";
    // vector definition
    Eigen::Vector3f v(1.0f,2.0f,3.0f);
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
    Eigen::Matrix3f i,j;
    i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    j << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;
    // matrix output
    std::cout << "Example of output \n";
    std::cout << i << std::endl << std::endl;
    std::cout << j << std::endl << std::endl;
    // matrix add i + j
    // matrix scalar multiply i * 2.0
    // matrix multiply i * j 
    // matrix multiply vector i * v

    std::cout << "i + j = " << std::endl << i + j << std::endl << std::endl;
    std::cout << "i * 2.0 = " << std::endl << i * 2.0 << std::endl << std::endl;
    std::cout << "i * j = " << std::endl << i * j << std::endl << std::endl;
    std::cout << "i * v(vector) = " << std::endl << i * j << std::endl << std::endl;

    Eigen::Matrix4f model;
	model << 1, 2, 3, 4, 2, 2, 3, 4, 3, 2, 3, 4, 4, 2, 3, 4;
	std::cout << "model : \n"<< model << std::endl;

    /* 
    * PA 0
    */
    // TO DO: Define point P
    // TO DO: Define rotation matrix M
    // TO DO: M * P
	Eigen::Vector2f p;
	p << 1.0, 0.0;
	Eigen::Matrix2f r;
	r << cos(90.0/ (180 / 3.1415926)), sin(90.0/ (180 / 3.1415926)), -sin(90.0/ (180 / 3.1415926)), cos(90.0/ (180 / 3.1415926));
//	std::cout << p<< std::endl;
	std::cout << r<< std::endl;
	std::cout << r * p << std::endl;
    return 0;
}
