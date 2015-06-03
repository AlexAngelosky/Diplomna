#ifndef MODEL_H
#define MODEL_H
#include <d3dx9.h>
#include "ObjLoader.h"

//class-a opisva obekt v 3D prostranstvoto kato izpolzva directx vertex i index bufferitek, koito se pulnqt s informaciq
//s pomoshtta na prochetenoto ot obj_loader-a ot nqkoy fail, opisvasht 3d obekt
class Model
{
    private:
        IDirect3DDevice9* gD3dDevice;//directx device ukazatel, she pomaga za realiziraneto na funkciqta draw
        ObjLoader* loader;//ukazatel kum obj_loader
        LPDIRECT3DVERTEXBUFFER9 v_buffer;//buffer ot vertexi
        LPDIRECT3DINDEXBUFFER9 i_buffer;//buffer ot indexi
        int vertices;//kolko vertexa imame
        int triangles;//kolko triugulnika si pravim ot tiq vertexi
    public:
        Model(IDirect3DDevice9*, ObjLoader*);//konstruktor
        LPDIRECT3DVERTEXBUFFER9& get_v_buffer();//vrushta referenciq kum vertex buffera
        LPDIRECT3DINDEXBUFFER9& get_i_buffer();//vrushta referenciq kum index buffera

        void load_model(char*);//zarejda model po dadeno ime na file (chete faila s loadera i pulni vertex i index bufferite)
        void draw(float, float, float, float, float, float, float, float, float);//risuva modela v 3d prostranstvoto s rotaciq, translaciq i scale
        void draw(float, float, float, float);//risuva modela v 3d prostranstvoto s translaciq i ednakuv scale po x y i z
};
#endif
