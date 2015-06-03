#include "GameWorld.h"
#include "GameObject.h"
#include "Camera.h"
void GameWorld::set_depth(float depth)
{
    this->depth = depth;
}
void GameWorld::set_width(float width)
{
    this->width = width;
}
void GameWorld::set_height(float height)
{
    this->height = height;
}
float GameWorld::get_depth()
{
    return this->depth;
}
float GameWorld::get_width()
{
    return this->width;
}
float GameWorld::get_height()
{
    return this->height;
}
void GameWorld::add_object(GameObject* object)
{
    if(this->objects != NULL)
    {
        if(this->objects[objects_counter] != NULL)
            delete[] this->objects[objects_counter];
        this->objects[objects_counter++] = object;
        if(objects_counter == capacity)
        {
            GameObject** p = this->objects;
            this->objects = new GameObject*[capacity+50];
            for(int i = 0; i<objects_counter; i++)
                this->objects[i] = p[i];
            for(int i = objects_counter; i<capacity+50; i++)
                this->objects[i] = NULL;
            delete[] p;
            capacity += 50;
        }
    }
    else
    {
        capacity = 50;
        objects_counter = 0;
        this->objects = new GameObject*[capacity];
        for(int i = 0; i<capacity; i++)
            this->objects[i] = NULL;
        this->objects[objects_counter++] = object;
    }
}

void GameWorld::set_camera(Camera*& camera)
{
    this->camera = camera;
}
Camera*& GameWorld::get_camera()
{
    return this->camera;
}

GameWorld::GameWorld()
{
    this->camera = NULL;
    this->objects = NULL;
    this->width = 200;
    this->height = 200;
    this->depth = 200;
}

GameWorld::GameWorld(float width, float height, float depth, Camera*& camera)
{
    set_width(width);
    set_height(height);
    set_depth(depth);
    set_camera(camera);
    this->capacity = 50;
    this->objects_counter = 0;
    this->objects = new GameObject*[capacity];
}

GameWorld::~GameWorld()
{
    for(int i = 0; i<capacity; i++)
        delete this->objects[i];
    delete[] this->objects;
    //not sure for camera!
}

void GameWorld::draw()
{
    for(int i = 0; i<objects_counter; i++)
        if(objects[i] != NULL)
            objects[i]->draw();
}
