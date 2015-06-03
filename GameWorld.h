#ifndef GAMEWORLD_H
#define GAMEWORLD_H
#include "GameObject.h"
#include "Camera.h"
class GameWorld{
private:
    float width;
    float height;
    float depth;
    GameObject** objects;
    Camera* camera;
    int capacity;
    int objects_counter;
public:
    GameWorld(float, float, float, Camera*&);
    GameWorld();
    ~GameWorld();
    void set_width(float);
    void set_height(float);
    void set_depth(float);
    float get_width();
    float get_height();
    float get_depth();
    void add_object(GameObject*);
    void set_camera(Camera*&);
    Camera*& get_camera();
    void draw();
};
#endif // GAMEWORLD_H
