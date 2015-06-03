#include "Model.h"
#include "ObjLoader.h"
#include "custom_vertex.h"
#include <iostream>
#include <d3dx9.h>
using namespace std;
Model::Model(IDirect3DDevice9* gD3dDevice, ObjLoader* loader)
{
    //inicializira model
    //toy ne moje da sushtestvuva bez obektite s chiqto pomosht shte se risuva i loadva ot file
    this->gD3dDevice = gD3dDevice;
    this->loader = loader;
    //NULL-vame bufferite
    v_buffer = NULL;
    i_buffer = NULL;
    //nulirame broq triugulnici i vertexi
    vertices = 0;
    triangles = 0;
}

void Model::load_model(char* filename)//loadva model po dadeno ime na file
{
    loader->load_model(filename);//loadera prochita file-a i zarejda modela otdelnite mu elementi vuv vektorite si
    //cout<<8<<endl;
    int n = 1000;//nachalen broy vertexi
    int i = 0;
    CUSTOMVERTEX* ver = new CUSTOMVERTEX[n];//suzdavame si masiv ot customvertexi, struktura koqto directx moje da risuva
    CUSTOMVERTEX init_vertex;
    for(int j = 0; j<loader->get_vv().size(); j++)
    {
        //za vseki edin procheten vertex si pravim customvertex i pulnim negovite stoynosti s tezi koito sme procheli
        CUSTOMVERTEX cv;
        //cout<<j<<endl;
        cv.X = loader->get_vv().at(j)->x;
        cv.Y = loader->get_vv().at(j)->y;
        cv.Z = loader->get_vv().at(j)->z;
        cv.NORMAL = {0.0f, 0.0f, -1.0f};//defaultskata stoynost na normalniq vector
        //nalaga se zashtoto ima .obj failove koito nqmat v sebe si opisanie na normalnite vectori
        //tuy kato tova ne e zaduljitelen element
        ver[i++] = cv;
        if(i == n)//ako broqt na slojenite veche vertexi e dostignal broq na razreshenite takiva razshirqvame masiva
        {
            CUSTOMVERTEX* p = ver;
            ver = new CUSTOMVERTEX[n+1000];
            for(int k = 0; k < n; k++)
            {
                ver[k] = p[k];
            }
            delete[] p;
            n+=1000;
        }
    }
    //cout<<9<<endl;
    vertices = i; //zapazvame kolko vertexa sme napravili

    int m = loader->get_faces().size()*6;
    int* indi = new int[m];//zadelqme pamet za intexite
    //cout<<10<<endl;
    i = 0;

    for(int j = 0; j<loader->get_faces().size();  j++)//za vseki face
    {
        //cout<<j<<endl;
        //cout<<loader->get_faces().at(j)->number_of_vertices<<endl;
        //cout<<loader->get_normals().size()<<endl;
        for(int f = 0; f<loader->get_faces().at(j)->number_of_vertices; f++)//za vseki zapazen vuv face-a vertex
        {
            int number_of_vertex = loader->get_faces().at(j)->vertices[f]-1;//nomer na vertexa
            indi[i++] = number_of_vertex;//number of vertex
            if(loader->get_normals().size()>0){
                //cout<<11<<endl;
                ver[number_of_vertex].NORMAL = *(loader->get_normals().at(loader->get_faces().at(j)->normals[f]-1));
                //cout<<12<<endl;
                //ako ima normalen vektor slaga i nego na mqstoto na defaultskiq
            }
        }
    }
    //cout<<11<<endl;
    // create a vertex buffer interface called v_buffer
    gD3dDevice->CreateVertexBuffer(vertices*sizeof(CUSTOMVERTEX),
                               0,
                               CUSTOMFVF,
                               D3DPOOL_MANAGED,
                               &v_buffer,
                               NULL);

    VOID* pVoid;    // a void pointer

    // lock v_buffer and load the vertices into it
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, ver, vertices*sizeof(CUSTOMVERTEX));
    v_buffer->Unlock();


    // create an index buffer interface called i_buffer
    gD3dDevice->CreateIndexBuffer(i*sizeof(int),
                              0,
                              D3DFMT_INDEX32,
                              D3DPOOL_MANAGED,
                              &i_buffer,
                              NULL);

    // lock i_buffer and load the indices into it
    i_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, indi, i*sizeof(int));
    i_buffer->Unlock();
    triangles = i/3;
}
LPDIRECT3DVERTEXBUFFER9& Model::get_v_buffer()
{
    return v_buffer;
}
LPDIRECT3DINDEXBUFFER9& Model::get_i_buffer()
{
    return i_buffer;
}
//risuva obekta v prostranstvoto na dadeni koordinati x y z rotiran po x y i z osta i scale-nat po x y z
void Model::draw(float x, float y, float z,
                 float rx, float ry, float rz,
                 float sx, float sy, float sz)
{
    D3DXMATRIX scaleMatrix;//pravim matrica za mashtabirane
    D3DXMatrixScaling(&scaleMatrix, sx, sy, sz);//zapulvame q s danni

    D3DXMATRIX rotYMatrix;//pravim matrica za rotaciq po y
    //D3DXVECTOR3 v = D3DXVECTOR3(rx,ry,rz);
    D3DXMatrixRotationY(&rotYMatrix, ry);//zapulvame q s danni

    D3DXMATRIX rotXMatrix;//pravim matrica za rotaciq po x
    //D3DXVECTOR3 v = D3DXVECTOR3(rx,ry,rz);
    D3DXMatrixRotationX(&rotXMatrix, rx);//zapulvame q s danni

    D3DXMATRIX translateMatrix;//pravim matrica za translaciq
    D3DXMatrixTranslation(&translateMatrix, x, y, z);//zapulvame q s danni

    //proizvedenieto na tezi matrici e matrica opisvashta vsichki transformacii na koito e bil podlojen obekta predi da
    //bude narisuvan
    D3DXMATRIX worldMatrix = scaleMatrix*rotXMatrix*rotYMatrix*translateMatrix;

    //kazvame koq she e transformaciqta

    gD3dDevice->SetTransform( D3DTS_WORLD, &worldMatrix);

    // select the vertex and index buffers to use
    //kazvame koi she sa buferite
    gD3dDevice->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
    gD3dDevice->SetIndices(i_buffer);

    //kazvame da narisuva index primitivata
    gD3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertices, 0, triangles);
}
void Model::draw(float x, float y, float z, float s)
{
    draw(x,y,z,0.0f,0.0f,0.0f,s,s,s);//vika gorniq draw method
}
