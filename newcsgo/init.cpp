#include "init.h"
float radarsize = 150;  //代表雷达->每个象限的大小
float flRadarPos_x = 120; //这个默认就好，如果想调，就调 radarsize 这个参数
float flRadarPos_y = 120;//这个默认就好，如果想调，就调 radarsize 这个参数
float radarrange = 60; 
extern Dwm_Dram* pDwm;

namespace G
{
	HWND g_hwnd;
	ULONG_PTR g_matrix_address;
	ULONG_PTR g_angle_address;
	ULONG_PTR g_players_address;
	Matrix4x4 g_matrix;
}

namespace Menum
{
	BOOL show;
	BOOL rect;
	BOOL ambot;
	BOOL point;

}


struct playerInfoList
{



	inline int GetHealth()
	{
		return *(int*)((uintptr_t)this + 血量);
	}

	inline Vector GetOrigin()
	{
		return *(Vector*)((uintptr_t)this + 坐标X偏移);
	}

	inline short IsDormant()
	{
		return *(short*)((uintptr_t)this + 人物坐标休眠标志);
	}
	inline int GetTeamNum()
	{
		return *(int*)((uintptr_t)this + 阵型偏移);
	}

	inline PVOID GetBoneBase()
	{
		ULONG_PTR offset25 = 0;
		__try {
			offset25 = (ULONG_PTR)this;
			offset25 = *(ULONG_PTR*)(offset25 + 0x14);
			offset25 = *(ULONG_PTR*)(offset25 + 0x44);
			offset25 = *(ULONG_PTR*)(offset25 + 0xc0);
			offset25 += 0xc;
		}
		__except (1)
		{
			offset25 = 0;
		}
		return (PVOID)offset25;
	}


	inline Vector GetBonePos(int boneIndex)
	{
		ULONG_PTR offset25 = (ULONG_PTR)GetBoneBase();

		Vector pTemp;
		if (offset25)
		{
			offset25 += boneIndex * 32;

			pTemp.x = *(float*)offset25;
			pTemp.y = *(float*)(offset25 + 16);
			pTemp.z = *(float*)(offset25 + 32);
		}

		return pTemp;
	}





};




struct EntityList
{
	playerInfoList* GetClientPlayerEntity(int entnum)
	{
		__try
		{
			return (playerInfoList*)(*(ULONG_PTR*)(G::g_players_address + entnum * 0x10));
		}
		__except (1)
		{
			return NULL;
		}
	}

	inline Matrix4x4 GetMatrix4x4Data()
	{

		return  *(Matrix4x4*)((uintptr_t)(G::g_matrix_address));
	}

	inline Vector GetViewAngles()
	{
		return *(Vector*)((uintptr_t)(G::g_angle_address));
	}

};


struct EngineClient
{


	bool GetPlayerInfo(int iIndex, playerinfo* pInfo)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID, int, playerinfo*);
		return getvfunc<OriginalFn>(this, 8)(this, iIndex + 1, pInfo);
	}
};

EntityList* pEntList;
EngineClient* pEngine;
void DrawPlayer(playerInfoList* entity, WndInfo* pWndInfo);
void draw_byDwm(playerInfoList* local,WndInfo* p);

PVOID GetInterface(LPCWSTR szModule, PCCH szObject)
{
	if (const auto Module = GetModuleHandleW(szModule))
	{
		const auto dwCreateInterface = reinterpret_cast<DWORD>(GetProcAddress(Module, "CreateInterface"));
		const auto dwShortJmp = dwCreateInterface + 0x5;
		const auto dwJmp = (dwShortJmp + *reinterpret_cast<DWORD*>(dwShortJmp)) + 0x4; //获取jmp 后的地址 

		auto pList = **reinterpret_cast<InterfaceNode***>(dwJmp + 0x6);  //获取[xxx] mov esi,[engine.dll+66EF64]--> 保存一个链表

		while (pList)
		{
			if ((strstr(pList->szName, szObject) && (strlen(pList->szName) - strlen(szObject)) < 5))
				return pList->fnGet();

			pList = pList->pNext;
		}
	}

	return nullptr;
}
void InitialiseInterfaces()
{
	pEngine = (EngineClient*)(GetInterface(L"engine.dll", "VEngineClient"));
}


void initialize_address()
{

	ULONG_PTR engine_module_address = (ULONG_PTR)GetModuleHandleW(L"engine.dll");
	ULONG_PTR client_module_address = (ULONG_PTR)GetModuleHandleW(L"client.dll");
	//ULONG_PTR server_module_address = (ULONG_PTR)GetModuleHandleW(L"server.dll");


	int matrix_address = engine_module_address + 0x596EF0;
	int angle_address = engine_module_address + 0x4622D0 - 4;
	int players_address = client_module_address + 0x04D3904;



	G::g_matrix_address = matrix_address;
	G::g_angle_address = angle_address;  //鼠标 xy
	G::g_players_address = players_address;


	G::g_hwnd = FindWindowW(L"Valve001", L"Counter-Strike Source");
}


BOOL CNot(BOOL a)
{
	if (a)
	{
		return FALSE;
	}

	return TRUE;
}

bool key_status(ULONG key)
{
	if (GetAsyncKeyState(key) & 1)
	{
		return true;
	}
	return false;
}
void monitor_key()
{

	if (key_status(VK_F1))
	{
		Menum::rect = CNot(Menum::rect);
	}

	if (key_status(VK_F2))
	{
		Menum::point = CNot(Menum::point);
	}
	if (key_status(VK_F3))
	{
		Menum::ambot = CNot(Menum::ambot);
	}
	if (key_status(VK_INSERT))
	{
		Menum::show = CNot(Menum::show);
	}

}


void dram_menum(RECT* pRect)
{
	if (!Menum::show)
	{
		pDwm->draw_text(50 + pRect->left, 200 - 30 + pRect->top, 绿色, "Ins / End ");
		pDwm->draw_text(50 +pRect->left, 200 + pRect->top, 绿色, (Menum::rect ? "F1 Rect On" : "F1 Rect Off"));
		pDwm->draw_text(50 + pRect->left, 200 + 30 +pRect->top, 绿色, (Menum::point ? "F2 point On" : "F2 point Off"));
		pDwm->draw_text(50 + pRect->left, 200 + 60 + pRect->top, 绿色, (Menum::ambot ? "F3 ambot On" : "F3 ambot Off"));
	}
}

void draw_byDwm(playerInfoList* local,WndInfo* p)
{
	playerinfo info = { 0 };
	int iMyTeam = local->GetTeamNum();

	for (int ax = 1; ax < 60; ax++)
	{
		playerInfoList* entity = pEntList->GetClientPlayerEntity(ax); //获取人物链表

		if (!entity)  //为空 就 continue
			continue;

		if (entity == local)//如果人物链表是 自己的 就 continue
			continue;

		if (entity->IsDormant())  //Dormant 休眠 判断是否休眠
			continue;

		if (entity->GetHealth() <= 1) //是否存活
			continue;
		if (iMyTeam == entity->GetTeamNum())  //自己得队伍 
		{
			continue;
		}

		if (Menum::rect) {
			DrawPlayer(entity, p);
		}

		if (Menum::ambot)
		{
			if (!pEngine->GetPlayerInfo(ax, &info))
			{
				continue;
			}
			DrawRadarPoint(entity->GetOrigin(), local->GetOrigin(), pEntList->GetViewAngles(), info.m_name,p);
		}

	}

}
void DrawRadarPoint(Vector vecOriginx/*敌人*/, Vector vecOriginy/*自己*/, Vector vAngle, char* playerName, WndInfo* p)
{


	int size = radarsize;



	//int centerx = flRadarPos_x + (size * 0.5) + p->window_x;  //坐标原点x
	//int centery = flRadarPos_y + (size * 0.5) + p->window_y;  //坐标原点y

	int centerx =  p->window_w_half +p->window_x;  //坐标原点x
	int centery = p->window_h_half + p->window_y;  //坐标原点y
	pDwm->DrawCircleFilled(centerx + 1, centery, 5, 白色, 20);

	float dx = vecOriginx.x - vecOriginy.x;
	float dy = vecOriginx.y - vecOriginy.y;
	float flAngle = vAngle.y;
	float yaw = (flAngle) * (PI / 180.0); //角度与弧度的换算:1°=Π/180 =0.01745 rad 
	float mainViewAngles_CosYaw = cos(yaw);
	float mainViewAngles_SinYaw = sin(yaw);
	float x = dy * (-mainViewAngles_CosYaw) + dx * mainViewAngles_SinYaw;
	float y = dx * (-mainViewAngles_CosYaw) - dy * mainViewAngles_SinYaw;
	float range = radarrange * 30;   //雷达范围
	if (fabs(x) > range || fabs(y) > range)
	{
		if (y > x)
		{
			if (y > -x) {
				x = range * x / y;
				y = range;
			}
			else {
				y = -range * y / x;
				x = -range;
			}
		}
		else {
			if (y > -x) {
				y = range * y / x;
				x = range;
			}
			else {
				x = -range * x / y;
				y = -range;
			}
		}
	}
	int ScreenX = centerx + int(x / range * float(size));
	int ScreenY = centery + int(y / range * float(size));

	int radar_menu_x = centerx;
	int radar_menu_y = centery;

	if (ScreenX < radar_menu_x - size)
		ScreenX = radar_menu_x - size + 1;
	else if (ScreenX > radar_menu_x + size)
		ScreenX = radar_menu_x + size - 3;

	if (ScreenY < radar_menu_y - size)
		ScreenY = radar_menu_y - size + 2;
	else if (ScreenY > radar_menu_y + size)
		ScreenY = radar_menu_y + size - 4;

	pDwm->DrawCircleFilled(ScreenX + 1, ScreenY, 5, 绿色, 10);

	if (playerName) {
		char buf[128] = { 0 };
		ULONG distance = GetLength(vecOriginx, vecOriginy);

		sprintf_s(buf, "%s:%d", playerName, distance);
		pDwm->draw_text(ScreenX - 2, ScreenY - 10, 绿色,buf);
	}
}

void DrawPlayer(playerInfoList* entity, WndInfo* pWndInfo)
{
	Vector Origin; //人物坐标 脚底下那个屏幕坐标
	Vector Origin3D = entity->GetOrigin(); //人物世界三维坐标 默认脚底下那个
	if (Origin3D.IsZero())
		return;



	//获取角色脚下 屏幕坐标
	if (!worldToScreen(&pEntList->GetMatrix4x4Data(), &Origin3D, pWndInfo->window_w_half, pWndInfo->window_h_half, &Origin)) {
		return;
	}

	Vector Head3D = entity->GetBonePos(21);  //人物骨骼头部世界坐标 

	if (Head3D.IsZero())
		return;


	Head3D.z += 8.0f; //画出方框 移动 上一点

	Vector Head; //人物头部 屏幕坐标
	if (!worldToScreen(&pEntList->GetMatrix4x4Data(), &Head3D, pWndInfo->window_w_half, pWndInfo->window_h_half, &Head)) {
		return;
	}

	//判断 这个两个是否 都在 窗口范围内
	if (IsPointInWindowsRect({ pWndInfo->window_x ,pWndInfo->window_y }, pWndInfo->window_w_half * 2, pWndInfo->window_h_half * 2, { (LONG)Origin.x + pWndInfo->window_x ,(LONG)Origin.y + pWndInfo->window_y }))
	{
		if (IsPointInWindowsRect({ pWndInfo->window_x ,pWndInfo->window_y }, pWndInfo->window_w_half * 2, pWndInfo->window_h_half * 2, { (LONG)Head.x + pWndInfo->window_x ,(LONG)Head.y + pWndInfo->window_y }))
		{
			//都在 就画出方框
			float h = fabs(Head.y - Origin.y);
			float w = h / 1.60f;

			RECT rect =
			{
				static_cast<long>(Origin.x - w * 0.5f),
				static_cast<long>(Head.y),
				static_cast<long>(w),
				static_cast<long>(Origin.y)
			};

			rect.right += static_cast<long>(rect.left);
			pDwm->draw_rect(rect.left + pWndInfo->window_x, rect.top + pWndInfo->window_y, rect.right - rect.left, rect.bottom - rect.top, 绿色, 2);

		}
	}

}


BOOL IsPointInWindowsRect(POINT wnd_left_pos, ULONG 窗口宽, ULONG 窗口高, POINT compare_pos)
{
	if ((compare_pos.x > wnd_left_pos.x) && (compare_pos.y > wnd_left_pos.y) && (compare_pos.x < (wnd_left_pos.x + 窗口宽)) &&
		(compare_pos.y < (wnd_left_pos.y + 窗口高)))
	{
		return TRUE;
	}

	return FALSE;
}

void get_window_size(HWND target, LONG& x, LONG& y, LONG& width, LONG& height)
{
	//返回窗口左顶点 x,y
	RECT rect = { 0 };

	if (GetWindowRect(target, &rect)) 
	{

		x = rect.left;
		y = rect.top;


		//返回窗口的大小
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		if (GetWindowLongW(target, GWL_STYLE) & WS_CAPTION)
		{
			x += 8;
			width -= 8;
			y += 30;
			height -= 30;
		}
	}
}

bool worldToScreen(IN Matrix4x4* matrix, IN Vector* world_pos, int window_width_half, int window_heigt_half, OUT Vector* ScreenPos)
{
	//相机Z

	float to_target = matrix->m[2][0] * world_pos->x
		+ matrix->m[2][1] * world_pos->y
		+ matrix->m[2][2] * world_pos->z
		+ matrix->m[2][3];
	if (to_target < 0.01f)
	{
		ScreenPos->x = 0;
		ScreenPos->y = 0;
		return false;
	}

	to_target = 1.0f / to_target;

	//相机x
	ScreenPos->x = window_width_half + (matrix->m[0][0] * world_pos->x
		+ matrix->m[0][1] * world_pos->y
		+ matrix->m[0][2] * world_pos->z
		+ matrix->m[0][3]) * to_target * window_width_half;


	ScreenPos->y = window_heigt_half - (matrix->m[1][0] * world_pos->x
		+ matrix->m[1][1] * world_pos->y
		+ matrix->m[1][2] * world_pos->z
		+ matrix->m[1][3]) * to_target * window_heigt_half;



	return true;
}

ULONG GetLength(Vector a, Vector b)
{
	ULONG temp_x = abs(a.x - b.x);
	ULONG temp_y = abs(a.y - b.y);
	return sqrt((temp_x * temp_x) + (temp_y * temp_y)) - 32;
}

void render()
{

	static BOOL bOnce = FALSE;
	if (!bOnce)
	{
		InitialiseInterfaces();
		initialize_address();
		bOnce = TRUE;
	}

	LONG window_x = 0; //窗口左顶点x
	LONG window_y = 0;//窗口左顶点y
	LONG window_w = 0;//窗口的宽
	LONG window_h = 0;//窗口的高
	get_window_size(G::g_hwnd, window_x, window_y, window_w, window_h);

	WndInfo dwInfo = { window_x,window_y,window_w / 2,window_h / 2 };

	monitor_key();

	if (pDwm->Dwm_StartDraw())
	{

		dram_menum((RECT*)&dwInfo);

		//自己的人物信息
		playerInfoList* local = pEntList->GetClientPlayerEntity(0);
		draw_byDwm(local, &dwInfo);
	}
	pDwm->Dwm_EndDraw();
	Sleep(1);
}