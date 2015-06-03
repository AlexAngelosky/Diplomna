#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "Model.h"
#include "Point.h"
class GameObject
{
private:
    Model* object_model;
    Point object_position;
    Point object_scale;
public:
    GameObject(Model*&, Point);
    GameObject(Model*&, Point, Point);
    GameObject(Model*&, Point, float);
    ~GameObject();
    void draw(float, float, float);
    void draw();
};
#endif // GAME_OBJECT
