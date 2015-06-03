#ifndef OBJ_LOADER
#define OBJ_LOADER
#include<vector>
#include<string>
#include <d3d9.h>
#include <d3dx9.h>
using namespace std;
//struktura koqto loadera she polzva za zapazvane na vertexite
struct v{
    double x,y,z,w;
    v()
    {
        w = 1;
    }
};
//struktura koqto loadera she polzva za zapazvane na texture vectorite
struct vt{
    double u,v,w;
    vt()
    {
        w = 0;
    }
};
//struktura koqto loadera she polzva za zapazvane na vruzkite mejdu vertexite, texturite i normalnite vectori
//face-a ne pazi samiq vertex ili normalen vector a pazi poredniq mu nomer vuv faila (nqkakuv index)
struct face{
    int* vertices;//ukazatel kum masiv ot vertexi za daden face(vruzkata mejdu trite neshta)
    int number_of_vertices;//kolko sa vertexite
    int* textures;//ukazatel kum masiv ot texturi za daden face
    int number_of_textures;//kolko sa texturite
    int* normals;//ukazatel kum masiv ot normalni vektori za daden face
    int number_of_normals;//kolko sa normalnite vektori
    face()//konstruktor
    {
        number_of_vertices = 3;//poneje napravih nabliudenieto che nay chesto edin face se definira ot tri vertexa tova e default stoinostta
        vertices = new int[number_of_vertices];//zadelqme pamet za masiva
        //sushtoto kato gore
        number_of_textures = 3;
        textures = new int[number_of_textures];
        number_of_normals = 3;
        normals = new int[number_of_normals];
    }
    ~face()//destruktor
    {
        delete[] vertices;//osvobojdavane pametta zadelena za zapazvane na vertexite
        delete[] textures;
        delete[] normals;
    }
};

class ObjLoader{//class za loadvane na obekti ot .obj fail
private:
    vector<v*> vv;//vector ot ukazateli kum vertexi
    vector<vt*> vvt;//vector ot ukazateli kum texture vectori
    vector<face*> faces;//vector ot ukazateli kum face-ove
    vector<D3DVECTOR*> normals;//vector ot normalni vectori
    void load_vv(char**, int);//loadva po daden dvumeren masiv na vseki red na koito ima po edin red ot textoviq fail vertexite
    void load_vvt(char**, int);//loadva po daden dvumeren masiv na vseki red na koito ima po edin red ot textoviq fail texture vectorite
    void load_faces(char**, int);//loadva po daden dvumeren masiv na vseki red na koito ima po edin red ot textoviq fail face-ovete
    void load_normals(char**, int);//loadva po daden dvumeren masiv na vseki red na koito ima po edin red ot textoviq fail normalnite vectori
    char* trim_whitespace(char*);//premahva whitespaceovete i tabulaciite v nachaloto i v kraq na daden simvolen niz
    int rows;//pazi broq na prochetenite ot .obj fail redove
    char** result;//pazi prochetenoto sudurjanie ot .obj faila
public:
    ObjLoader();//konstruktor
    void read_file(char*, char**&, int& );//prochita faila i zapazva vsicko procheteno v dvumeren masiv vseki red na koito e red ot faila
    void split(char*, char**&, int&, char);//razdelq daden simvolen niz na mnojestvo ot simvolni nizove po daden simvol
    void load_model(char*);//loadva model ot fail
    vector<v*> get_vv();//vrushta vectora ot vertexi
    vector<vt*> get_vvt();//vrushta vectora ot texture vectori
    vector<face*> get_faces();//vrushta vectora ot faceove
    vector<D3DVECTOR*> get_normals();//vrushta vectora ot normalni vectori
};

#endif // OBJ_LOADER
