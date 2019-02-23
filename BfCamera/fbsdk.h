#define dx9includes

#ifdef dx9includes
#include <d3dx9math.h>
#define Matrix4x4 D3DXMATRIXA16
#define Vec2 D3DXVECTOR2
#define Vec3 D3DXVECTOR3
#define Vec4 D3DXVECTOR4
#endif

class RenderView
{
public:
	Matrix4x4 transform1; //0x0000
	Matrix4x4 transform2; //0x0040
	char pad_0080[672]; //0x0080
	Vec3 cameraPosition; //0x0320
	char pad_032C[260]; //0x032C
	Vec3 viewProjection; //0x0430
	char pad_0470[1120]; //0x0470
}; //Size: 0x08D0

class GameRenderer
{
public:
	char pad_0000[1336]; //0x0000
	class RenderView* renderView; //0x0538
	char pad_0540[2824]; //0x0540
}; //Size: 0x1048
