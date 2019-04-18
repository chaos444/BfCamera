#pragma once
#define dx9includes
//#define dx11includes
#ifdef dx9includes
#include <d3dx9math.h>
#pragma comment (lib, "d3dx9.lib")
#define Matrix4x4 D3DXMATRIXA16
#define Vec2 D3DXVECTOR2
#define Vec3 D3DXVECTOR3
#define Vec4 D3DXVECTOR4
#endif

#ifdef dx11includes
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <utility>
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;
#define Matrix4x4 XMFLOAT4X4A
#define Vec3 XMFLOAT3
#define Vec2 XMFLOAT2
#define Vec4 XMFLOAT4
#endif

class UISettings {
public:
	char pad_000[0x44];
	bool drawEnable;
};

class GameRenderSettings
{
public:
	char pad_0000[40]; //0x0000
	float resolutionScale; //0x0028
	char pad_002C[32]; //0x002C
	float forceFOV; //0x004C
	char pad_0050[32]; //0x0050
	float forceBlurAmount; //0x0070
	char pad_0074[84]; //0x0074
	float brightnessScale; //0x00C8
	char pad_00CC[1916]; //0x00CC
}; //Size: 0x0848

class MeshSettings
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088

class WorldRenderSettings
{
public:
	char pad_0000[328]; //0x0000
}; //Size: 0x0148

class CompositionSettings
{
public:
	char pad_0000[184]; //0x0000
	class MeshSettings* meshSettings; //0x00B8
	char pad_00C0[88]; //0x00C0
	class WorldRenderSettings* worldRenderSettings; //0x0118
	char pad_0120[1832]; //0x0120
}; //Size: 0x0848


class RenderView
{
public:
	Matrix4x4 transform1; //0x0000
	Matrix4x4 transform2; //0x0040
	char pad_0080[672]; //0x0080
	Vec3 cameraPosition; //0x0320
	char pad_032C[260]; //0x032C
	Vec3 viewProjection; //0x0430
	char pad_0470[128]; //0x0470
}; //Size: 0x04F0

class GameRenderer
{
public:
	char pad_0000[1304]; //0x0000
	class GameRenderSettings* gameRenderSettings; //0x0518
	class CompositionSettings* compositionSettings; //0x0520
	char pad_0528[16]; //0x0528
	class RenderView* renderView; //0x0538
	char pad_0540[2824]; //0x0540
}; //Size: 0x1048

