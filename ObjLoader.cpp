#include "ObjLoader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
using namespace std;

ObjLoader::ObjLoader()
{
}

vector<v*> ObjLoader::get_vv() {return vv;}
vector<vt*> ObjLoader::get_vvt() {return vvt;}
vector<face*> ObjLoader::get_faces() {return faces;}
vector<D3DVECTOR*> ObjLoader::get_normals() {return normals;}


void ObjLoader::load_model(char* filename)
{
    //izchistvat se vsichki vectori, za da moje ako predi tova sus sushtiq loader sme zarejdali drug obekt da se premahne informaciqta za nego
    vv.clear();
    vvt.clear();
    faces.clear();
    normals.clear();
    //broy redove vuv faila - 100 po podrazbirane
    rows = 100;
    result = new char*[rows];//zadelqme pamet da pazim prochetenoto
    read_file(filename, result, rows);//chetem ot fail
    load_vv(result, rows);//zapazvame prochetenite ot faila vertexi vuv vector ot ukazateli kum tqh
    load_vvt(result, rows);//zapazvame prochetenite ot faila texture vectori vuv vector ot ukazateli kum tqh
    load_normals(result, rows);//zapazvame prochetenite ot faila normalni vectori vuv vector ot ukazateli kum tqh
    load_faces(result, rows);//zapazvame prochetenite ot faila faceove vuv vector ot ukazateli kum tqh
}

void ObjLoader::read_file(char* filename, char**& result, int& rows)
{
    int current_row = 0;//koy red chetem
    ifstream reader;//obekt koyto pozvolqva chetene ot fail
    reader.open(filename, ios::in);//otvarqme fail s ime filename
    if(!reader.is_open()) return;//ako faila ne se otvori spirame da chetem
    char* rl = new char[200];//zadelqme pamet za edin red... she ima problem ako reda e nad 200 simvola
    while(reader.getline(rl,200))//dokato ima kakvo da chetem, chetem edin red i go zapazvame v r1
    {
        char* r = new char[200];//zadelqme nova pamet
        strcpy(r, rl);//kopirame ot r1 v r
        result[current_row++] = r;//slagame r v masiva result
        if(current_row == rows)//ako nqma svobodni redove
        {
            //razshirqvame masiva
            char** p = result;
            result = new char*[rows+100];
            for(int i=0; i<rows; i++)
            {
                int len = strlen(p[i]);
                result[i] = new char[len+1];
                strcpy(result[i], p[i]);
                result[i][len] = '\0';
            }
            for(int i = 0; i<rows; i++)
            {
                delete[] p[i];
            }
            delete[] p;
            rows += 100;
        }
    }
    rows = current_row;//kazvame che redovete v dvumerniq masiv sa tolkova kolkoto sme slojili
}

//loadvame vertexite kato obikalqme vsichko procheteno i tursim redove chiqto purva duma e "v"
void ObjLoader::load_vv(char** loaded_from_file, int rows)
{
    for(int i = 0; i<rows; i++){
        char* line = loaded_from_file[i];//red ot faila
        int n = 10;
        char** words = new char*[n];//masiv koito she pazi dumite ot tozi red
        split(line, words, n, ' ');//cepvame reda na dumi
        if(strcmp(words[0], "v") == 0)//proverqvame dali purvata ot tqh e "v"
        {
            v* _v = new v();//pravime si nov vertex
            _v->x = atof(words[1]);//davame mu koordinati za x
            _v->y = atof(words[2]);//davame mu koordinati za y
            _v->z = atof(words[3]);//davame mu koordinati za z
            vv.push_back(_v);//slagame go vuv vectora ot vertexi
        }
        for(int j = 0; j<n; j++)
            delete[] words[j];
        delete[] words;
    }
}
//loadva normalnite vectori kato obikalq vsichko procheteno ot faila i tursi redove, zapochvashti s "vn"
void ObjLoader::load_normals(char** loaded_from_file, int rows)
{
    for(int i = 0; i<rows; i++){
        char* line = loaded_from_file[i];//vzima poredniq red
        int n = 10;
        char** words = new char*[n];//zadelq masiv za dumite v nego
        split(line, words, n, ' ');//razdeklq reda na dumi po podaden simvol
        if(strcmp(words[0], "vn") == 0)//gleda dali purvata duma e "vn"
        {
            D3DVECTOR* _v = new D3DVECTOR();//suzdava nov vector
            _v->x = atof(words[1]);//davame mu koordinati za x
            _v->y = atof(words[2]);//davame mu koordinati za y
            _v->z = atof(words[3]);//davame mu koordinati za z
            normals.push_back(_v);//slagame go vuv vectora ot normalni vectori
        }
        for(int j = 0; j<n; j++)
            delete[] words[j];
        delete[] words;
    }
}
//loadva texturnite vectori kato obikalq vsichko procheteno ot faila i tursi redove, zapochvashti s "vt"
void ObjLoader::load_vvt(char** loaded_from_file, int rows)
{
    for(int i = 0; i<rows; i++){
        char* line = loaded_from_file[i];//vzima poredniq red
        int n = 10;
        char** words = new char*[n];//zadelq masiv za dumite v nego
        split(line, words, n, ' ');//razdeklq reda na dumi po podaden simvol
        if(strcmp(words[0], "vt") == 0)//gleda dali purvata duma e "vt"
        {
            vt* _v = new vt();//suzdava nov vt obekt
            _v->u = atof(words[1]);//davame mu koordinati za u
            _v->v = atof(words[2]);//davame mu koordinati za v
            if(n == 4)
                _v->w = atof(words[3]);//moje da ima i w ako ima slagame i neq
            vvt.push_back(_v);//slagame go vuv vectora ot normalni vectori
        }
        for(int j = 0; j<n; j++)
            delete[] words[j];
        delete[] words;
    }
}

void ObjLoader::load_faces(char** loaded_from_file, int rows)
{
    for(int i = 0; i<rows; i++){
        char* line = loaded_from_file[i];//vzima poredniq red
        char* trimed_line = trim_whitespace(line);//premahva whitespace i tab
        delete[] line;
        line = trimed_line;
        int n = 10;
        char** words = new char*[n];//zadelq masiv za dumite v nego
        split(line, words, n, ' ');//razdeklq reda na dumi po podaden simvol
        if(strcmp(words[0], "f") == 0)//gleda dali purvata duma e "f"
        {
            int c = 0;
            face* _f = new face();//suzdava nov f obekt

            int number_of_vertices = (n-3)*3; //smqta kolko vertexa she trqbva da ima daden face ako go triangulirame

            int* vertices = new int[number_of_vertices];//zadelqme pamet za nomerata na vertexite
            int* normals = new int[number_of_vertices];//zadelqme pamet za nomerata na normalnite vectori
            int* textures = new int[number_of_vertices];//zadelqme pamet za nomerata na texturite

            //vseki edin polygon se deli na n-2 triugulnika kudeto n e broq na vurhovete mu

            for(int j = 1; j<n; j++)//za vsqka ot dumite na reda koito zapochva s "f"
            {
                int m = 3;//ako imame i trite elementa v vt vn, ako nqmame sha polzvame chast ot pametta
                char** numbers = new char*[m];//zadelqme pamet za tqh
                split(words[j], numbers, m, '/');//razdelqme chislata
                //cout<<m<<endl;
                //system("pause");
                //cout<<numbers[0]<<" "<<numbers[1]<<endl;
                //system("pause");
                if(m == 2)//ako sa 2
                {
                    vertices[c] = atoi(numbers[0]);//imame vertexi i texturi
                    textures[c++] = atoi(numbers[1]);
                }
                if(m == 3)//ako sa 3
                {
                    vertices[c] = atoi(numbers[0]);//imame vertexi
                    if(strlen(numbers[1])>0)//imame texturi
                        textures[c] = atoi(numbers[1]);//slagame texturite
                    normals[c++] = atoi(numbers[2]);//slagame normalnite vectori
                }

                for(int k = 0; k<m; k++)
                    delete[] numbers[k];
                delete[] numbers;
            }
            int counter = 0;

            delete[] _f->vertices;
            delete[] _f->normals;

            _f->normals = new int[number_of_vertices];
            _f->number_of_normals = number_of_vertices;
            _f->vertices = new int[number_of_vertices];
            _f->number_of_vertices = number_of_vertices;
            //procheli sme veche vsichki nomera na vertexi texturi i normalni vectori i sega gi razdelqme na triugulnici
            //i vsichki vertexi i tn na triugulnicite gi zapazvame v mmasivite na face-a
            for(int j = 1; j<n-2; j++)
            {
                _f->normals[counter] = normals[0];
                _f->vertices[counter++] = vertices[0];
                _f->normals[counter] = normals[j];
                _f->vertices[counter++] = vertices[j];
                _f->normals[counter] = normals[j+1];
                _f->vertices[counter++] = vertices[j+1];
            }
            faces.push_back(_f);//slagame face-a vuv vector
        }
    }
}
//razdelq daden simvolen niz na mnojestvo ot nizove po daden simvol
void ObjLoader::split(char* to_split, char**& result, int& words, char delimiter)
{
    int start = -1, end = -1;
    int len = strlen(to_split);
    int current_word = 0;
    for(int i = 0; i<len+1; i++)
    {
        if(to_split[i] == delimiter || to_split[i] == '\0')
        {
            end = i;
            int n = end-start-1;
            result[current_word] = new char[n+1];
            int c = 0;
            for(int j = start+1; j<end; j++)
            {
                result[current_word][c++] = to_split[j];
            }
            result[current_word][c] = '\0';
            start = end;
            current_word++;
        }

    }
    words = current_word;
}
//premahva whitespaceovete v nachaloto i v kraq na simvolniq niz
char* ObjLoader::trim_whitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;
  int len = strlen(str);
  char* to_return = new char[len+1];
  strcpy(to_return, str);
  to_return[len] = '\0';
  return to_return;
}
