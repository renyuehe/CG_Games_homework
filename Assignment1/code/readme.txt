问题：
//hormogenies coordinate
for (auto & vert : v)
{
    vert.x() = 0.5*width*vert.x() + 0.5*width;
    vert.y() = 0.5*height*vert.y() + 0.5*height;
    std::cout << " default z = "  << vert.z() << std::endl;
    std::cout << " f1, f2 = "  << f1 << "," << f2 << std::endl;

    vert.z() = vert.z() * f1 + f2; // 这里的 z, 为什么要 * f1 + f2 ,其中 f1 = 49.95 f2 = 50.05

    std::cout << " z = "  << vert.z() << std::endl << std::endl;
}