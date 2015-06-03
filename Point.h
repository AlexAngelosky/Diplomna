#ifndef POINT_H
#define POINT_H
struct Point
{
    float x, y, z;
    Point(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Point()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
};
#endif // POINT_H
