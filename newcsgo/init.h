#pragma once
#include "DWM/Dwm_Dram.h"
#include "Vector.h"
#define  Ѫ�� 0x94
#define  ����ƫ��   0x9c
#define ����Xƫ��   0x260
#define  �����������߱�־  0x17e

#define  ��ɫ {255, 255, 255, 255}
#define  ��ɫ { 0, 255, 0, 255 }
#define  ��ɫ -65536

#define PI 3.14159265358979323846
struct Matrix4x4 {
	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;

		};
		struct {
			float _11, _21, _31, _41;
			float _12, _22, _32, _42;
			float _13, _23, _33, _43;
			float _14, _24, _34, _44;

		};
		float m[4][4];
	};
};
template <typename t>
t getvfunc(void* class_pointer, size_t index)
{
	return (*(t**)class_pointer)[index];
}

struct playerinfo
{
	char m_name[128];
	int userid;
	int m_nUserID;
	char guid[32 + 1];
	unsigned int friendsid;
	char friendsname[32 + 96];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
};
struct WndInfo
{
	LONG window_x = 0; //�����󶥵�x
	LONG window_y = 0;//�����󶥵�y
	LONG window_w_half = 0;//���ڵĿ��һ��
	LONG window_h_half = 0;//���ڵĸߵ�һ��
};
struct InterfaceNode
{
	void* (*fnGet)();
	const char* szName;
	InterfaceNode* pNext;
};
PVOID GetInterface(LPCWSTR szModule, PCCH szObject);
void render();
void InitialiseInterfaces();
void initialize_address();
void monitor_key();

bool key_status(ULONG key);
BOOL CNot(BOOL a);
void dram_menum(RECT* pRect);


BOOL IsPointInWindowsRect(POINT wnd_left_pos, ULONG ���ڿ�, ULONG ���ڸ�, POINT compare_pos);

void get_window_size(HWND target, LONG& x, LONG& y, LONG& width, LONG& height);
ULONG GetLength(Vector a, Vector b);
//������:���ڿ��һ��
//������:���ڸ߶�һ��
bool worldToScreen(IN Matrix4x4* matrix, IN Vector* world_pos, int window_width_half, int window_heigt_half, OUT Vector* ScreenPos);
void DrawRadarPoint(Vector vecOriginx, Vector vecOriginy, Vector vAngle, char* playerName,WndInfo* p);