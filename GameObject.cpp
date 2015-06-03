#include "GameObject.h"
#include <d3dx9.h>
GameObject::GameObject(Model*& model, Point position)
{
    this->object_model = model;
    this->object_position = position;
    this->object_scale.x = 1;
    this->object_scale.y = 1;
    this->object_scale.z = 1;
}

GameObject::GameObject(Model*& model, Point position, Point scale)
{
    this->object_model = model;
    this->object_position = position;
    this->object_scale = scale;
}

GameObject::GameObject(Model*& model, Point position, float scale)
{
    this->object_model = model;
    this->object_position = position;
    this->object_scale.x = scale;
    this->object_scale.y = scale;
    this->object_scale.z = scale;
}

GameObject::~GameObject()
{
    delete this->object_model;
}

void GameObject::draw(float rx, float ry, float rz)
{
    if(object_model != NULL)
    {
        object_model->draw(
object_position.x, object_position.y, object_position.z,
rx, ry, rz,
object_scale.x, object_scale.y, object_scale.z);
    }
}

void GameObject::draw()
{
    draw(0, 0, 0);
}
