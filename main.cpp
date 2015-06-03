#define VC_EXTRALEAN
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <string>
#include <sstream>
#include <iostream>
using namespace std;

#include "Camera.h"
#include "ObjLoader.h"
#include "custom_vertex.h"
#include "Model.h"
#include "Point.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "HelpFunctions.h"
#include "Rectangle.h"

// Global Direct3D objects
IDirect3D9* gD3dObject=NULL;
IDirect3DDevice9* gD3dDevice=NULL;
ID3DXMesh * gTeapotMesh=NULL;
ID3DXFont * gFont=NULL;

// Direct3D materials
D3DMATERIAL9 gRedMaterial;
D3DMATERIAL9 gGreenMaterial;
D3DMATERIAL9 gBlueMaterial;
D3DMATERIAL9 gYellowMaterial;

// Our camera class
Camera* camera=NULL;

// Some program constants
const float cameraMovementSpeed=0.1f;
const float cameraRotationSpeed=0.01f;
const int fontSize=14;

// Forward declarations of functions in this source file
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool SetupDirect3D(HWND hWnd);
void UpdateGame();

//camera movement and rotation
bool rotate_left = false,
     rotate_right = false,
     rotate_up = false,
     rotate_down = false,
     go_forward = false,
     go_backward = false,
     go_right = false,
     go_left = false;

//camera rotation by mouse
int oldX, oldY;

//screen size variables
int WIDTH = 0;
int HEIGHT = 0;

ObjLoader loader;

Model* chair = NULL;
Model* hands_makarov = NULL;
Model* ak = NULL;
Model* bottle = NULL;
Model* cube = NULL;

GameWorld world;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GetDesktopResolution(WIDTH, HEIGHT);
    ShowCursor(false);
    oldX = WIDTH/2;
    oldY = HEIGHT/2;
    SetCursorPos(oldX, oldY);
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc= (WNDPROC)WndProc;
    wcex.cbClsExtra= 0;
    wcex.cbWndExtra= 0;
    wcex.hInstance= hInstance;
    wcex.hIcon= 0;
    wcex.hCursor= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName= 0;
    wcex.lpszClassName= "Rise of O2";
    wcex.hIconSm= 0;

    RegisterClassEx(&wcex);

    HWND hWnd = CreateWindow("Rise of O2", "Rise of O2", WS_EX_TOPMOST|WS_POPUP,
                             0, 0, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);

    if (!SetupDirect3D(hWnd))
        return 0;

    // Create our camera and set its initial position
    D3DXVECTOR3 camera_vector(0,0,-12.0f);
    camera=new Camera(camera_vector);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );

    // When there are no messages for our window update or demo
    while( msg.message!=WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {

            if(rotate_left) camera->Yaw(-cameraRotationSpeed);
            if(rotate_right) camera->Yaw(cameraRotationSpeed);
            if(rotate_up) camera->Pitch(-cameraRotationSpeed);
            if(rotate_down) camera->Pitch(cameraRotationSpeed);

            if(go_backward && !go_left && !go_right) camera->MoveForward(-cameraMovementSpeed);
            if(go_backward && (go_left || go_right)) camera->MoveForward(-cameraMovementSpeed/2);

            if(go_forward && !go_left && !go_right) camera->MoveForward(cameraMovementSpeed);
            if(go_forward && (go_left || go_right)) camera->MoveForward(cameraMovementSpeed/2);

            if(go_left && !go_backward && !go_forward) camera->MoveRight(-cameraMovementSpeed);
            if(go_left && (go_backward || go_forward)) camera->MoveRight(-cameraMovementSpeed/2);

            if(go_right && !go_backward && !go_forward) camera->MoveRight(cameraMovementSpeed);
            if(go_right && (go_backward || go_forward)) camera->MoveRight(cameraMovementSpeed/2);

            //puts the camera on the ground
            camera->GetPosition().y = 0;

            UpdateGame();
            // Dont be mean - let other apps have a wee bit of time:
            Sleep(0);
        }

    }

    // Clean up and exit
    delete camera;
    gFont->Release();
    gTeapotMesh->Release();
    gD3dDevice->Release();
    gD3dObject->Release();

    return (int)msg.wParam;
}

// Callback function for our window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYUP:
        switch(wParam)
        {
        // Translations
        case 'W':
            go_forward = false;
            break;
        case 'S':
            go_backward = false;
            break;
        case 'D':
            go_right = false;
            break;
        case 'A':
            go_left = false;
            break;

        // Rotations
        case VK_LEFT:
            rotate_left = false;
            break;
        case VK_RIGHT:
            rotate_right = false;
            break;
        case VK_UP:
            rotate_up = false;
            break;
        case VK_DOWN:
            rotate_down = false;
            break;
        default:
            break;
        }
        break;
    case WM_KEYDOWN:
        switch(wParam)
        {
        // Translations
        case 'W':
            go_forward = true;
            break;
        case 'S':
            go_backward = true;
            break;
        case 'D':
            go_right = true;
            break;
        case 'A':
            go_left = true;
            break;
        // Rotations
        case VK_LEFT:
            rotate_left = true;
            break;
        case VK_RIGHT:
            rotate_right = true;
            break;
        case VK_UP:
            rotate_up = true;
            break;
        case VK_DOWN:
            rotate_down = true;
            break;
        default:
            break;
        }
        break;
    case WM_MOUSEMOVE:
    {
// Retrieve mouse screen position
        int x=(short)LOWORD(lParam);
        int y=(short)HIWORD(lParam);

        float alpha = acos(x);
        float betha = M_PI/2 - alpha;

        // SetCursorPos(WIDTH/2, HEIGHT/2);
        //trqbva da se nameri kak sled vsqko murdane kursora da ostava v sredata na ekrana

        POINT pt;
        // you should set pt as a position in your Application
        GetCursorPos(&pt);
        POINT screen_pt = pt;
        //ScreenToClient(hWnd, &pt);
        ScreenToClient(hWnd, &pt);
        POINT client_pt = pt;

        POINT zero;
        zero.x = screen_pt.x - client_pt.x;
        zero.y = screen_pt.y - client_pt.y;

        //kod pozvolqvasht na kamerata da se vurti i chrez mishkata
        //proverqva se ako tekushtata poziciq na kursora na miskata se e promenila sprqmo starata stoynost da se zavurta kamerata
        //spored tova na kude e otishla mishkata
        if(x > oldX+1)
        {
            rotate_right = true;
            //camera->Yaw(cameraRotationSpeed*1.2);
            //SetCursorPos( zero.x + WIDTH/2, zero.y+HEIGHT/2);
        }
        else rotate_right = false;

        if(x < oldX-1)
        {
            rotate_left = true;
            //camera->Yaw(-cameraRotationSpeed*1.2);
            //SetCursorPos( zero.x + WIDTH/2, zero.y+HEIGHT/2);
        }
        else rotate_left = false;

        if(y > oldY+1)
        {
            rotate_down = true;
            //camera->Pitch(cameraRotationSpeed*1.2);
            //SetCursorPos( zero.x + WIDTH/2, zero.y+HEIGHT/2);
        }
        else rotate_down = false;
        if(y < oldY-1)
        {
            rotate_up = true;
            //camera->Pitch(-cameraRotationSpeed*1.2);
            //SetCursorPos( zero.x + WIDTH/2, zero.y+HEIGHT/2);
        }
        else rotate_up = false;

        if (x == oldX)
        {
            rotate_left = false;
            rotate_right = false;
        }
        oldX = x;
        oldY = y;

        float d = 0.99f;
        if(x >= WIDTH/2+WIDTH/4)
        {
            x = WIDTH/2+WIDTH/4 - 1;
            oldX = x-d;
        }
        if(x<= WIDTH/4)
        {
            x = WIDTH/4+1;
            oldX = x+d;
        }

        if(y >= HEIGHT/2+HEIGHT/4)
        {
            y = HEIGHT/2+HEIGHT/4 - 1;
            oldY = y-d;

            if(y<= HEIGHT/4)
            {
                y = HEIGHT/4+1;
                oldY = y+d;
            }



            // Check to see if the left button is held down:
            bool leftButtonDown=wParam & MK_LBUTTON;

            // Check if right button down:
            bool rightButtonDown=wParam & MK_RBUTTON;

        }
    }
    default:
        // We do not want to handle this message so pass back to Windows to handle it in a default way
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Create our Direct3D objects etc.
bool SetupDirect3D(HWND hWnd)
{
    //zarejdane na 3d modelite

    gD3dObject=Direct3DCreate9(D3D_SDK_VERSION);
    if (!gD3dObject)
        return 0;

    D3DPRESENT_PARAMETERS presParams;
    ZeroMemory(&presParams,sizeof(presParams));

    presParams.Windowed=TRUE;
    presParams.SwapEffect=D3DSWAPEFFECT_DISCARD;
    presParams.BackBufferFormat=D3DFMT_UNKNOWN;
    presParams.PresentationInterval=D3DPRESENT_INTERVAL_ONE;
    presParams.EnableAutoDepthStencil = TRUE;
    presParams.AutoDepthStencilFormat = D3DFMT_D16;

    HRESULT hr=gD3dObject->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
                                        D3DCREATE_HARDWARE_VERTEXPROCESSING, &presParams, &gD3dDevice);
    if (FAILED(hr))
        return false;

    // Turn on the zbuffer
    // Create a teapot to view
    hr=D3DXCreateTeapot(gD3dDevice,&gTeapotMesh,NULL);
    if (FAILED(hr))
        return false;

    // The teapot has position and normal per vertex so we will need to turn on the lights
    gD3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(20,20,20));
    gD3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE);


    // Load models
    chair = new Model(gD3dDevice, &loader);
    chair->load_model("sink.obj");
    hands_makarov = new Model(gD3dDevice, &loader);
    hands_makarov->load_model("hands_makarov.obj");
    bottle = new Model(gD3dDevice, &loader);
    bottle->load_model("bottle.obj");
    ak = new Model(gD3dDevice, &loader);
    ak->load_model("sink.obj");
    cube = new Model(gD3dDevice, &loader);
    cube->load_model("cube.obj");

    //Add objects to world
    world.add_object(new GameObject(cube, Point(30, 0, 30), 20));
    world.add_object(new GameObject(cube, Point(-30, 0, 30), 5));

    // Fill in a light structure defining our light
    D3DLIGHT9 light;//svetlina
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type    = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 1.0f;
    light.Diffuse.a = 1.0f;
    light.Range   = 1000.0f;

    // Create a direction for our light - it must be normalized
    D3DXVECTOR3 vecDir;
    vecDir = D3DXVECTOR3(0.0f,-0.3f,0.7f);//posoka na svetlinata
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );

    // Tell the device about the light and turn it on
    gD3dDevice->SetLight( 0, &light );
    gD3dDevice->LightEnable( 0, TRUE );

    // Create three materials so we can draw the teapot three times
    // in red, green and blue

    //suzdavane na materiali
    ZeroMemory( &gRedMaterial, sizeof(D3DMATERIAL9) );
    gRedMaterial.Diffuse.r = gRedMaterial.Ambient.r = 1.0f;
    gRedMaterial.Diffuse.g = gRedMaterial.Ambient.g = 0;
    gRedMaterial.Diffuse.b = gRedMaterial.Ambient.b = 0;
    gRedMaterial.Diffuse.a = gRedMaterial.Ambient.a = 1.0f;

    ZeroMemory( &gGreenMaterial, sizeof(D3DMATERIAL9) );
    gGreenMaterial.Diffuse.r = gGreenMaterial.Ambient.r = 0;
    gGreenMaterial.Diffuse.g = gGreenMaterial.Ambient.g = 1.0f;
    gGreenMaterial.Diffuse.b = gGreenMaterial.Ambient.b = 0;
    gGreenMaterial.Diffuse.a = gGreenMaterial.Ambient.a = 1.0f;

    ZeroMemory( &gBlueMaterial, sizeof(D3DMATERIAL9) );
    gBlueMaterial.Diffuse.r = gBlueMaterial.Ambient.r = 0;
    gBlueMaterial.Diffuse.g = gBlueMaterial.Ambient.g = 0;
    gBlueMaterial.Diffuse.b = gBlueMaterial.Ambient.b = 1.0f;
    gBlueMaterial.Diffuse.a = gBlueMaterial.Ambient.a = 1.0f;

    ZeroMemory( &gYellowMaterial, sizeof(D3DMATERIAL9) );
    gYellowMaterial.Diffuse.r = gYellowMaterial.Ambient.r = 1.0f;
    gYellowMaterial.Diffuse.g = gYellowMaterial.Ambient.g = 1.0f;
    gYellowMaterial.Diffuse.b = gYellowMaterial.Ambient.b = 0.0f;
    gYellowMaterial.Diffuse.a = gYellowMaterial.Ambient.a = 1.0f;

    // Set up matrix
    RECT rct;
    GetClientRect(hWnd,&rct);
    D3DXMATRIX matProj;
    float aspect = (rct.right-rct.left) / (float)(rct.bottom-rct.top);
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, aspect, 1.0f, 100.0f );
    gD3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Create a D3DX font object
    hr=D3DXCreateFont( gD3dDevice, fontSize, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                       DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &gFont );
    if (FAILED(hr))
        return false;

    return true;
}

const float kGapX=3.0f;
const float kGapY=5.0f;
const int kNumTeapotsPerAxis=16;
float ry = 0.00+M_PI;
float rx = 0.0f;

// Called each update we clear the scene, draw all our teapots, put up some text and present it
void UpdateGame()
{
    gD3dDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0),1.0f,0);
    gD3dDevice->BeginScene();

    // Get and set the view matrix
    D3DXMATRIX viewMatrix;
    camera->CalculateViewMatrix(&viewMatrix);
    gD3dDevice->SetTransform( D3DTS_VIEW, &viewMatrix );

    gD3dDevice->SetMaterial( &gRedMaterial );

    world.draw();

    //todo: make a model to be rotatable
    D3DXVECTOR3 pos = camera->GetPosition();//vzimame poziciqta na kamerata za da moje da zakachim za neq rucete

    //moving the hands with the camera
    //mouse not working yet
    if(rotate_left)
    {

        ry -= 0.01;
    }
    if(rotate_right)
    {
        ry += 0.01;
    }
    if(rotate_up)
    {
        rx += 0.01;
    }
    if(rotate_down)
    {
        rx-=0.01;
    }
    if(rx >= M_PI/2)
        rx = M_PI/2;
    if(rx <= -M_PI/2)
        rx = -M_PI/2;

    hands_makarov->draw(pos.x, pos.y, pos.z, rx, ry,
                        camera->GetLook().z, 2, 2, 2);

    bottle->draw(-10.0f, 0.0f, 10.0f, 1);
     bottle->draw(10.0f, 0.0f, 10.0f, 5);
      bottle->draw(12.0f, 0.0f, 5.0f, 3);

    for (int i=0; i<kNumTeapotsPerAxis; i++)
    {
        D3DXMATRIX worldMatrix;
        D3DXMatrixTranslation(&worldMatrix,kGapX+kGapX*i,0.0f,0.0f);
        gD3dDevice->SetTransform( D3DTS_WORLD, &worldMatrix );
        gTeapotMesh->DrawSubset(0);
    }

    // Represent the z axis with green teapots
    gD3dDevice->SetMaterial( &gGreenMaterial );
    for (int i=0; i<kNumTeapotsPerAxis; i++)
    {
        D3DXMATRIX worldMatrix;
        D3DXMatrixTranslation(&worldMatrix,0.0f,0.0f,kGapX+kGapX*i);
        gD3dDevice->SetTransform( D3DTS_WORLD, &worldMatrix );
        gTeapotMesh->DrawSubset(0);
    }

    // Finally put a yellow teapot in the middle of the axis and rotate it
    static float rotation=0;
    rotation+=0.1f;
    D3DXMATRIX worldMatrix;
    D3DXMatrixRotationY(&worldMatrix,rotation);
    gD3dDevice->SetTransform( D3DTS_WORLD, &worldMatrix );
    gD3dDevice->SetMaterial( &gYellowMaterial );
    gTeapotMesh->DrawSubset(0);

    // Draw some text showing the controls and current angles and position
    D3DCOLOR fontColor = D3DCOLOR_XRGB(255,255,255);
    RECT rct;
    rct.left=fontSize;
    rct.right=800;
    rct.top=fontSize;
    rct.bottom=rct.top+fontSize;



    rct.bottom=rct.top+fontSize;
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;

    gFont->DrawText(NULL, "Diplomna rabota Alexander Angelosky:", -1, &rct, 0, fontColor );
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;
    gFont->DrawText(NULL, "Controls:", -1, &rct, 0, fontColor );
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;
    gFont->DrawText(NULL, "Left and right arrows: turn camera left and right", -1, &rct, 0, fontColor );
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;
    gFont->DrawText(NULL, "Up and down arrows: point camera up and down", -1, &rct, 0, fontColor );
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;
    gFont->DrawText(NULL, "Pointing the camera works with the mouse now", -1, &rct, 0, fontColor );
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;
    gFont->DrawText(NULL, "W,A,S,D for movement", -1, &rct, 0, fontColor );
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;
    rct.bottom=rct.top+fontSize;
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;


    // Current values
    gFont->DrawText(NULL, "Current Values", -1, &rct, 0, fontColor );
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;

    std::string posStr="Position x:"+ToString(camera->GetPosition().x) +
                       " y:" +ToString(camera->GetPosition().y)+
                       " z:" +ToString(camera->GetPosition().z);
    gFont->DrawText(NULL, posStr.c_str(), -1, &rct, 0, fontColor );

    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;

    std::string rotYawStr="Yaw - rotation around the y axis: "+ToString(camera->GetYaw())+
                          " Radians "+ToString(ToDegrees(camera->GetYaw()))+" Degrees";
    std::string rotPitchStr="Pitch - rotation around the x axis: " +ToString(camera->GetPitch())+
                            " Radians "+ToString(ToDegrees(camera->GetPitch()))+" Degrees";
    std::string rotRollStr="Roll - rotation around the z axis: " +ToString(camera->GetRoll())+
                           " Radians "+ToString(ToDegrees(camera->GetRoll()))+" Degrees";

    gFont->DrawText(NULL, rotYawStr.c_str(), -1, &rct, 0, fontColor );
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;
    gFont->DrawText(NULL, rotPitchStr.c_str(), -1, &rct, 0, fontColor );
    rct.top+=fontSize;
    rct.bottom=rct.top+fontSize;
    gFont->DrawText(NULL, rotRollStr.c_str(), -1, &rct, 0, fontColor );

    gD3dDevice->EndScene();
    gD3dDevice->Present( NULL, NULL, NULL, NULL );
}
