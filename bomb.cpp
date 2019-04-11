// bomb.cpp ////////////////////////////////////////////////
//************************************************//
//       作品名：炸弹超人                         //
//       作者: 王宁                               //
//       联系方式:qq 30298344                     //
//       代码完成时间:2006年月日                  //
//       编译环境:win XP & VC++ 6.0 & DX SDK8.1   //
//       版权说明:本demo为开源代码,你可任意       //
//                使用,但请保留此段               //
//                                                //
//************************************************//

// INCLUDES ///////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  // just say no to MFC

#define INITGUID

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream.h> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>

#include <ddraw.h> // include directdraw

// DEFINES ////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "WINCLASS1"

// default screen size
#define SCREEN_WIDTH    640  // size of screen
#define SCREEN_HEIGHT   480
#define SCREEN_BPP      8    // bits per pixel

#define BITMAP_ID            0x4D42 // universal id for a bitmap
#define MAX_COLORS_PALETTE   256

// TYPES //////////////////////////////////////////////////////

// basic unsigned types
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;

// container structure for bitmaps .BMP file
typedef struct BITMAP_FILE_TAG
	{
		BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
        BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
        PALETTEENTRY     palette[256];      // we will store the palette here
        UCHAR            *buffer;           // this is a pointer to the data
	} BITMAP_FILE, *BITMAP_FILE_PTR;

struct TEXTURE
{
	int x,y;			//屏幕坐标
	LPDIRECTDRAWSURFACE7  tile[12];
};

struct BOMBMAN
{
	int world_x,world_y,	//世界坐标
		x,y;				//屏幕坐标
	int maxbomb_num;		//最多炸弹数
	int bomb_num;			//手里有的雷数
	bool throughwall;		//是否可穿墙
	bool bombcontrol;		//是否可控制炸弹, 这是能力应该属于人的结构，
	int firelevel;			//火力大小，这是所有炸弹的能力，各雷之间没区别
	int state;
	int dead_counter;
	LPDIRECTDRAWSURFACE7  frame[2];
};

struct GHOST
{
	int world_x,world_y,	//世界坐标
		x,y;				//屏幕坐标
	int direction;
	bool cannotmove;
	int state;
	int dead_counter;
	LPDIRECTDRAWSURFACE7  frame[2];
};


struct BOMB
{
	int world_x,world_y;	//世界坐标
	int x,y;				//屏幕坐标
	int counter;
	int counter_explode;
	int state;

	LPDIRECTDRAWSURFACE7  frame;
};

int world[19][23];

int scene[19][23]={ 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
					3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
					3,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,
					3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
					3,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,
					3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
					3,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,
					3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
					3,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,
					3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
					3,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,
					3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
					3,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,
					3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
					3,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,
					3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
					3,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,3,
					3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
					3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
					};
int item[19][23];


// MACROS /////////////////////////////////////////////////

// tests if a key is up or down
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// initializes a direct draw struct
#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct));ddstruct.dwSize=sizeof(ddstruct); }

#define ALIVE 1
#define DEAD  0

#define TAKE    0
#define SET     1
#define EXPLODE 2
// 全局变量 ////////////////////////////////////////////////

HWND      main_window_handle = NULL; // globally track main window
HINSTANCE hinstance_app      = NULL; // globally track hinstance
int       window_closed      = 0;    // tracks if window is closed

int item_num=0;
int i,j,k;
long score = 0;
bool win;
int start_map_x,
	start_map_y, 
	end_map_x,
	end_map_y,
	world_x,
	world_y;
int offset_x,
	offset_y;
int tile_x,
	tile_y;

// directdraw stuff
LPDIRECTDRAW7         lpdd         = NULL;   // dd4 object
LPDIRECTDRAWSURFACE7  lpddsprimary = NULL;   // dd primary surface
LPDIRECTDRAWSURFACE7  lpddsback    = NULL;   // dd back surface
LPDIRECTDRAWSURFACE7  lpddsbackground = NULL;// this will hold the background image
LPDIRECTDRAWSURFACE7  lpddsbackground2 = NULL;// this will hold the background image
LPDIRECTDRAWPALETTE   lpddpal      = NULL;   // a pointer to the created dd palette
LPDIRECTDRAWCLIPPER   lpddclipper  = NULL;   // dd clipper
LPDIRECTDRAWCLIPPER	  lpddclipper_background = NULL;
LPDIRECTDRAWCLIPPER   lpddclipper_background2 = NULL;
PALETTEENTRY          palette[256];          // color palette
PALETTEENTRY          save_palette[256];     // used to save palettes
DDSURFACEDESC2        ddsd;                  // a direct draw surface description struct
DDBLTFX               ddbltfx;               // used to fill
DDSCAPS2              ddscaps;               // a direct draw surface capabilities struct
HRESULT               ddrval;                // result back from dd calls
DWORD                 start_clock_count = 0; // used for timing

BITMAP_FILE           bitmap;                // holds the bitmap
TEXTURE				  texture;
BOMBMAN				  bombman;
GHOST				  ghost[4];				//相个敌人
BOMB				  bomb[277];

// 函数声明  //////////////////////////////////////////////

int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height);
int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename);
int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap);
int Scan_Image_Bitmap(BITMAP_FILE_PTR bitmap, LPDIRECTDRAWSURFACE7 lpdds, int cx,int cy);
int BitmaptoSurface(BITMAP_FILE_PTR bitmap, LPDIRECTDRAWSURFACE7 lpdds );
int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds,int color);
int Color_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds,int x, int y, int width,  int height,  int color);
int DDraw_Draw_Surface(LPDIRECTDRAWSURFACE7 source, int x, int y, int width, int height, LPDIRECTDRAWSURFACE7 dest, int transparent=1);   
int SurfacetoSurface(LPDIRECTDRAWSURFACE7 source, int sx, int sy, int swidth, int sheight, int dx, int dy, int dwidth,  int dheight, LPDIRECTDRAWSURFACE7 dest, int transparent = 1) ;
int Draw_Text_GDI(char *text, int x,int y,COLORREF color, LPDIRECTDRAWSURFACE7 lpdds);
LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,int num_rects,LPRECT clip_list);
LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height,int mem_flags, int color_key);

int Game_Init(void *parms = NULL, int num_parms = 0);
int Game_Main(void *parms = NULL, int num_parms = 0);
int Game_Shutdown(void *parms = NULL, int num_parms = 0);


//WindowProc//////////////WindowProc////////////////WindowProc

LRESULT CALLBACK WindowProc(HWND hwnd, 
						    UINT msg, 
                            WPARAM wparam, 
                            LPARAM lparam)
{
	PAINTSTRUCT		ps;		// used in WM_PAINT
	HDC				hdc;	// handle to a device context
	switch(msg)
		{	
		case WM_CREATE: 
			{
			return(0);
			} break; 
		case WM_PAINT: 
			{
   			hdc = BeginPaint(hwnd,&ps);	 
			EndPaint(hwnd,&ps);
			return(0);
   			} break;
		case WM_DESTROY: 
			{
			PostQuitMessage(0);
			return(0);
			} break;
		default:break;
		} // end switch
	return (DefWindowProc(hwnd, msg, wparam, lparam));
} // end WinProc

////////////////////////////////////////////////////////////

int Game_Init(void *parms , int num_parms )
{
	// 建立DirectDraw 7.0 对像
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)))
       return(0);
	// 设置协作级别为：全屏
	if (FAILED(lpdd->SetCooperativeLevel(main_window_handle, 
                                      DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | 
                                      DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT)))
		return(0);
	//  设置显示模式为： 640x480x8
	if (FAILED(lpdd->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,0,0)))
		return(0);

	// we need a complex surface system with a primary and backbuffer

	// clear ddsd and set size
	DDRAW_INIT_STRUCT(ddsd); 
	// enable valid fields
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	// request a complex, flippable
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
	// set the backbuffer count field to 1, use 2 for triple buffering
	ddsd.dwBackBufferCount = 1;
	// 建立主表面
	if (FAILED(lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL)))
	   return(0);

	// 建立后备表面
	ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
	if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
	  return(0);

	// 用随机色建立调色表（1到254）
	for (int color=1; color < 255; color++)
		{
		// fill with random RGB values
		palette[color].peRed   = rand()%256;
		palette[color].peGreen = rand()%256;
		palette[color].peBlue  = rand()%256;

		// set flags field to PC_NOCOLLAPSE
		palette[color].peFlags = PC_NOCOLLAPSE;
		} // end for color
	// 0为黑，255为白
	palette[0].peRed     = 0;
	palette[0].peGreen   = 0;
	palette[0].peBlue    = 0;
	palette[0].peFlags   = PC_NOCOLLAPSE;

	palette[255].peRed   = 255;
	palette[255].peGreen = 255;
	palette[255].peBlue  = 255;
	palette[255].peFlags = PC_NOCOLLAPSE;

	if (FAILED(lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 | 
									DDPCAPS_INITIALIZE, 
									palette,&lpddpal, NULL)))
	return(0);

	// 关联调色板  到  主表面
	if (FAILED(lpddsprimary->SetPalette(lpddpal)))
	   return(0);
	//建立背景表面
	lpddsbackground = DDraw_Create_Surface(SCREEN_WIDTH,SCREEN_HEIGHT,0,0); //0表示透明色为黑色
	// 建立用来放背景的离屏表面
	lpddsbackground2 = DDraw_Create_Surface(SCREEN_WIDTH,SCREEN_HEIGHT,0,0);//-1为不带透明色

	// 在后备表面上设置剪切器
	RECT screen_rect= {0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1};
	lpddclipper = DDraw_Attach_Clipper(lpddsback,1,&screen_rect);
	lpddclipper_background2=DDraw_Attach_Clipper(lpddsbackground2,1,&screen_rect);

	RECT background_rect= {0,0,543,479};
	lpddclipper_background = DDraw_Attach_Clipper(lpddsbackground,1,&screen_rect);

	// 清理主表面和后备表面
	DDraw_Fill_Surface(lpddsprimary,0);
	DDraw_Fill_Surface(lpddsback,0);
	DDraw_Fill_Surface(lpddsbackground,0);
	DDraw_Fill_Surface(lpddsbackground2,0);


	// 读入背景位图到BITMAP
	if (!Load_Bitmap_File(&bitmap,"bk.bmp"))
		return(0);
//		  Scan_Image_Bitmap(&bitmap,                 // bitmap file to scan image data "from"
//						  lpddsbackground2,	   // surface "to" hold data
//						  0, 0);      
	BitmaptoSurface(&bitmap,lpddsbackground2);
	Unload_Bitmap_File(&bitmap);	//卸载位图

	// 读入tile位图到BITMAP结构********************************************
	if (!Load_Bitmap_File(&bitmap,"tiles.bmp"))
	   return(0);
	// 用tile位图的调色板重设主表面的调色板  lpddpal已和主表面关联
	if (FAILED(lpddpal->SetEntries(0,0,MAX_COLORS_PALETTE,bitmap.palette)))
	   return(0);

	//载入场景贴图
	for (int index = 0; index < 12; index++)
	  {
		// 建立表面存放各帧图型
		  texture.tile[index] = DDraw_Create_Surface(32,32,0,0);  //int width, int height, int mem_flags, int color_key=0
		  // 把位图装进相应表面	//这里没有用 memcpy
		  Scan_Image_Bitmap(&bitmap,                 // bitmap file to scan image data "from"
						  texture.tile[index],	   // surface "to" hold data
						  index, 0);               // cell(X,Y) to scan image from 
	  } // end for index

	//载入主角图片
	for (i = 0; i < 2; i++)
	{
	  // 建立表面存放各帧图型
	  bombman.frame[i] = DDraw_Create_Surface(32,32,0,0);  //int width, int height, int mem_flags, int color_key=0
	  // 把位图装进相应表面	
	  Scan_Image_Bitmap(&bitmap,                 // bitmap file to scan image data "from"
					  bombman.frame[i],	   // surface "to" hold data
					  i, 1);               // cell(X,Y) to scan image from 
	} // end for index

	//载入敌人图片
	for( j=0; j<4; j++)			//4个敌人
		for (i = 0; i < 2; i++)		//每个敌人两张图
		{
		  ghost[j].frame[i] = DDraw_Create_Surface(32,32,0,0); 
		  Scan_Image_Bitmap(&bitmap,                 // bitmap file to scan image data "from"
						  ghost[j].frame[i],	   // surface "to" hold data
						  i, j+2);               // cell(X,Y) to scan image from 
		} // end for index

	Unload_Bitmap_File(&bitmap);	//卸载位图


	// seed random number generator 初始随机数产生器，这样每次才真正随机
	srand(GetTickCount());

	for(i=0;i<19;i++)
	  for(j=0; j<23; j++)
	  {
		  world[i][j] = 0;	// world清0
	  }

	for(i=0;i<19;i++)
	  for(j=0; j<23; j++)
	  {
		  item[i][j] = 0;	// item清0
	  }

	//随机数砖墙
	for(i=0;i<19;i++)
	  for(j=0; j<23; j++)
	 {
		if(rand()%5 == 1)	//随机数越大砖墙越少,可以根据难度调整随机数
		  world[i][j] = 1;	
	 }

	//随机生成道具位置数据
	item_num = rand()%2+1;	//每关最多2个道具，最少1个，
	for(i=0;i<item_num;i++)
	{
		int index_y,index_x;
		do{
			index_y=rand()%8*2+3;
			index_x=rand()%10*2+3;
		}while( (index_x%2==0 && index_y%2 ==0) || world[index_y][index_x]!=1);
		item[index_y][index_x] = rand()%4+8;
	}

	//随机生成门的位置
	do{
		i=rand()%8*2+1;
		j=rand()%10*2+1;
	}while((i%2==0 && j%2 ==0) || item[i][j]!=0 || world[i][j]!=1);
	item[i][j] = 12;

	//把固定场景载入
	for(i=0;i<19;i++)
	  for(j=0; j<23; j++)
	  {
		if(scene[i][j]==2 || scene[i][j]==3)	
		  world[i][j] = scene[i][j];
	  }
	world[1][1]=0;			//清理主角出场时周围的空间
	world[1][2]=0;
	world[2][1]=0;

	world_x=0;
	world_y=0;
	win=0;
	score=0;

	bombman.world_x=32;//256;		//初始化主角位置
	bombman.world_y=32;//224;
	bombman.x=bombman.world_x-world_x;//256;
	bombman.y=bombman.world_y-world_y;//224;
	bombman.maxbomb_num = 1;
	bombman.bomb_num = bombman.maxbomb_num;
	bombman.state=ALIVE;
	bombman.throughwall=false;
	bombman.bombcontrol=false;
	bombman.firelevel=1;
	bombman.dead_counter=0;
	
	for(i=0; i<4; i++)
	{
		do{
		ghost[i].world_x=rand()%9*64+160;//初始化敌人位置
		ghost[i].world_y=rand()%8*64+160;
		}while(world[ghost[i].world_y/32][ghost[i].world_x/32]!=0);	//确保不让敌入生在石头里
		
		ghost[i].x=ghost[i].world_x-world_x;
		ghost[i].y=ghost[i].world_y-world_y;
		do
		{
		  ghost[i].direction = rand()%5-2;//初始化方向
		}while(ghost[i].direction==0);
		ghost[i].cannotmove = false;
		ghost[i].state=ALIVE;
		ghost[i].dead_counter=0;
	}
	
	for(i=0; i<bombman.maxbomb_num; i++)
	{
	bomb[i].counter = 0;
	bomb[i].counter_explode=0;
	bomb[i].state = TAKE;	//可用
	}
	return(1);
} // end Game_Init**************************************************************************************

//游戏逻辑*********************************************************************************
int Game_Main(void *parms , int num_parms )
{// this is the main loop of the game, do all your processing here
	int index; 

	if (window_closed)
	   return(0);
	if (KEYDOWN(VK_ESCAPE) )
	   {
		//int result = MessageBox(main_window_handle,
		//			" are you sure 确定退出游戏吗？",
		//			" exit退出",
		//			MB_YESNO | MB_ICONQUESTION);
		//if(result== IDYES)
		//{				
			PostMessage(main_window_handle,WM_CLOSE,0,0);
			window_closed = 1;
		//}
	   } // end if

	if(bombman.state==DEAD || win==1)		//主角死了就重新初始化
	{	
		Sleep(1000);
		//PostMessage(main_window_handle,WM_CLOSE,0,0);
				for(i=0;i<19;i++)
				  for(j=0; j<23; j++)
				  {
					  world[i][j] = 0;	// world清0
				  }

				for(i=0;i<19;i++)
				  for(j=0; j<23; j++)
				  {
					  item[i][j] = 0;	// item清0
				  }

				//随机数砖墙
				for(i=0;i<19;i++)
				  for(j=0; j<23; j++)
				 {
					if(rand()%5 == 1)	//随机数越大砖墙越少,可以根据难度调整随机数
					  world[i][j] = 1;	
				 }

				//随机生成道具位置数据
				item_num = rand()%2+1;	//每关最多2个道具，最少1个，
				for(i=0;i<item_num;i++)
				{
					int index_y,index_x;
					do{
						index_y=rand()%8*2+3;
						index_x=rand()%10*2+3;
					}while( (index_x%2==0 && index_y%2 ==0) || world[index_y][index_x]!=1);
					item[index_y][index_x] = rand()%4+8;
				}

				//随机生成门的位置
				do{
					i=rand()%8*2+1;
					j=rand()%10*2+1;
				}while((i%2==0 && j%2 ==0) || item[i][j]!=0 || world[i][j]!=1);
				item[i][j] = 12;

				//把固定场景载入
				for(i=0;i<19;i++)
				  for(j=0; j<23; j++)
				  {
					if(scene[i][j]==2 || scene[i][j]==3)	
					  world[i][j] = scene[i][j];
				  }

				world[1][1]=0;			//清理主角出场时周围的空间
				world[1][2]=0;
				world[2][1]=0;

				world_x=0,
				world_y=0;
				win=false;
				score=0;

				bombman.world_x=32;//256;		//初始化主角位置
				bombman.world_y=32;//224;
				bombman.x=bombman.world_x-world_x;//256;
				bombman.y=bombman.world_y-world_y;//224;
				bombman.state=ALIVE;
				bombman.dead_counter=0;
				if(win==false)
				{
					bombman.maxbomb_num = 1;
					bombman.bomb_num = bombman.maxbomb_num;
					bombman.throughwall=false;
					bombman.bombcontrol=false;
					bombman.firelevel=2;
				}
				
				for(i=0; i<4; i++)
				{
					do{
					ghost[i].world_x=rand()%9*64+160;//初始化敌人位置
					ghost[i].world_y=rand()%8*64+160;
					}while(world[ghost[i].world_y/32][ghost[i].world_x/32]!=0);	//确保不让敌入生在石头里
					
					ghost[i].x=ghost[i].world_x-world_x;
					ghost[i].y=ghost[i].world_y-world_y;
					do
					{
					  ghost[i].direction = rand()%5-2;//初始化方向
					}while(ghost[i].direction==0);
					ghost[i].cannotmove = false;
					ghost[i].state=ALIVE;
					ghost[i].dead_counter=0;
				}
				
				for(i=0; i<bombman.maxbomb_num; i++)
				{
				bomb[i].counter = 0;
				bomb[i].counter_explode=0;
				bomb[i].state = TAKE;	//可用
				}
	}

	DDraw_Fill_Surface(lpddsback, 0);	//背景置黑	
	
	DDraw_Fill_Surface(lpddsbackground, 154);	//背景置黑

	//移动主角------------------------------------------------------------------
	if (!KEYDOWN(VK_SPACE))	//如果按空格时就不能移动
	{
	  if (KEYDOWN(VK_RIGHT))
	  {
			if( bombman.world_y%32 <=8)
			{
				if(   world[(int)bombman.world_y/32] [(int)(bombman.world_x/32)+1]<=1
					||world[(int)bombman.world_y/32] [(int)(bombman.world_x/32)+1]>=4)
				{
					//如果主角将移动的位置上有雷
					if(world[(int)bombman.world_y/32][(int)(bombman.world_x/32)+1]==4)
					{
						if( bombman.world_x%32!=0 )
						{
							if(bombman.x>=256)			//更新窗口的世界坐标（绝对坐标）
							  if ((world_x+=4) > 192)
								world_x = 192;
							bombman.world_x+=4;		//更新主角的世界坐标
							bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
							if(bombman.world_y%32!=0)
							{
								if(bombman.y<=224)
								  if ((world_y-=bombman.world_y%32) < 0)
									world_y = 0;
								bombman.world_y-=bombman.world_y%32;
								bombman.y = bombman.world_y - world_y;
							}
						}
					}
					else if(world[(int)bombman.world_y/32] [(int)(bombman.world_x/32)+1]==1)
					{
						if(bombman.throughwall)
						{
							if(bombman.x>=256)			//更新窗口的世界坐标（绝对坐标）
							  if ((world_x+=4) > 192)
								world_x = 192;
							bombman.world_x+=4;		//更新主角的世界坐标
							bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
							if(bombman.world_y%32!=0)
							{
								if(bombman.y<=224)
								  if ((world_y-=bombman.world_y%32) < 0)
									world_y = 0;
								bombman.world_y-=bombman.world_y%32;
								bombman.y = bombman.world_y - world_y;
							}
						}
					}
					else	//主角将移动的位置上没有雷也没有墙
					{
						if(bombman.x>=256)			//更新窗口的世界坐标（绝对坐标）
						  if ((world_x+=4) > 192)
							world_x = 192;
						bombman.world_x+=4;		//更新主角的世界坐标
						bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
						if(bombman.world_y%32!=0)
						{
							if(bombman.y<=224)
							  if ((world_y-=bombman.world_y%32) < 0)
								world_y = 0;
							bombman.world_y-=bombman.world_y%32;
							bombman.y = bombman.world_y - world_y;
						}
					}
				}
			}
			else if( bombman.world_y%32 >=24)
			{
				if(   world[(int)bombman.world_y/32+1] [(int)(bombman.world_x/32)+1]<=1
					||world[(int)bombman.world_y/32+1] [(int)(bombman.world_x/32)+1]>=5
				  )
				{
					if(world[(int)bombman.world_y/32+1] [(int)(bombman.world_x/32)+1]==1)
					{
						if(bombman.throughwall)
						{
							if(bombman.x>=256)			//更新窗口的世界坐标（绝对坐标）
							  if ((world_x+=4) >192)
								world_x = 192;
							bombman.world_x+=4;		//更新主角的世界坐标（绝对坐标）
							bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
							if(bombman.y>=224)
							  if ((world_y+=32-bombman.world_y%32) > 128)
							   world_y = 128;
							bombman.world_y+= 32-bombman.world_y%32;
							bombman.y = bombman.world_y - world_y;
						}
					}
					else
					{
						if(bombman.x>=256)			//更新窗口的世界坐标（绝对坐标）
						  if ((world_x+=4) >192)
							world_x = 192;
						bombman.world_x+=4;		//更新主角的世界坐标（绝对坐标）
						bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
						if(bombman.y>=224)
						  if ((world_y+=32-bombman.world_y%32) > 128)
						   world_y = 128;
						bombman.world_y+= 32-bombman.world_y%32;
						bombman.y = bombman.world_y - world_y;
					}
				}
			}
	  } // end if(KEYDOWN(VK_RIGHT))
	  else
	  if (KEYDOWN(VK_LEFT))
	  {
			if( bombman.world_y%32 <=8)		//包括水平处在整位置上，即向左或左上方移动
			{
				if(   world[(int)bombman.world_y/32] [(int)(bombman.world_x+31)/32-1]<=1
					||world[(int)bombman.world_y/32] [(int)(bombman.world_x+31)/32-1]>=4)
				{	
					//如果主角将移动的位置上有雷
					if(world[(int)bombman.world_y/32] [(int)(bombman.world_x+31)/32-1]==4)
					{
						  if( bombman.world_x%32!=0 )
						  {
							if(bombman.x<=256)			//更新窗口的世界坐标（绝对坐标）
							  if ((world_x-=4) < 0)
								world_x = 0;
							bombman.world_x-=4;		//更新主角的世界坐标（绝对坐标）
							bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
							if(bombman.world_y%32!=0)
							{
								if(bombman.y<=224)
								  if ((world_y-=bombman.world_y%32) < 0)
									world_y = 0;
								bombman.world_y-=bombman.world_y%32;
								bombman.y = bombman.world_y - world_y;
							}
						  }
					}
					//主角将移动的位置上有砖墙
					else if( world[(int)bombman.world_y/32][(int)(bombman.world_x+31)/32-1]==1)
					{
						if(bombman.throughwall)
						{
							if(bombman.x<=256)			//更新窗口的世界坐标（绝对坐标）
							  if ((world_x-=4) < 0)
								world_x = 0;
							//world.y=??
							bombman.world_x-=4;		//更新主角的世界坐标（绝对坐标）
							bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
							if(bombman.world_y%32!=0)
							{
								if(bombman.y<=224)
								  if ((world_y-=bombman.world_y%32) < 0)
									world_y = 0;
								bombman.world_y-=bombman.world_y%32;
								bombman.y = bombman.world_y - world_y;
							}
						}
					}
					else	//主角将移动的位置上没有雷也没有砖墙
					{
						if(bombman.x<=256)			//更新窗口的世界坐标（绝对坐标）
						  if ((world_x-=4) < 0)
							world_x = 0;
						//world.y=??
						bombman.world_x-=4;		//更新主角的世界坐标（绝对坐标）
						bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
						if(bombman.world_y%32!=0)
						{
							if(bombman.y<=224)
							  if ((world_y-=bombman.world_y%32) < 0)
								world_y = 0;
							bombman.world_y-=bombman.world_y%32;
							bombman.y = bombman.world_y - world_y;
						}
					}
				}
			}
			else if( bombman.world_y%32 >=24)	//即下左向方移动
				if(   world[(int)bombman.world_y/32+1] [(int)(bombman.world_x+31)/32-1]<=1
					||world[(int)bombman.world_y/32+1] [(int)(bombman.world_x+31)/32-1]>=5)
				{
					if(world[(int)bombman.world_y/32+1][(int)(bombman.world_x+31)/32-1]==1 )
					{
						if(bombman.throughwall)
						{
							if(bombman.x<=256)			//更新窗口的世界坐标（绝对坐标）
							  if ((world_x-=4) < 0)
								world_x = 0;
							bombman.world_x-=4;		//更新主角的世界坐标（绝对坐标）
							bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
							if(bombman.y>=224)
							  if ((world_y+=32-bombman.world_y%32) > 128)
							   world_y = 128;
							bombman.world_y+= 32-bombman.world_y%32;
							bombman.y = bombman.world_y - world_y;
						}
					}
					else
					{
						if(bombman.x<=256)			//更新窗口的世界坐标（绝对坐标）
						  if ((world_x-=4) < 0)
							world_x = 0;
						bombman.world_x-=4;		//更新主角的世界坐标（绝对坐标）
						bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
						if(bombman.y>=224)
						  if ((world_y+=32-bombman.world_y%32) > 128)
						   world_y = 128;
						bombman.world_y+= 32-bombman.world_y%32;
						bombman.y = bombman.world_y - world_y;
					}
				}
	  } // end if(KEYDOWN(VK_LEFT))

	  if (KEYDOWN(VK_UP))
	  {
			if( bombman.world_x%32 <=8)
			{
				if( world[(int)(bombman.world_y+31)/32-1] [(int)bombman.world_x/32]<=1
				  ||world[(int)(bombman.world_y+31)/32-1] [(int)bombman.world_x/32]>=4)
				{
					//如果主角将移动的位置上有雷
					if(world[(int)(bombman.world_y+31)/32-1] [(int)bombman.world_x/32]==4)
					{
						if(bombman.world_y%32!=0)
						{
							if(bombman.y<=224)
							  if ((world_y-=4) < 0)
							   world_y = 0;
							bombman.world_y-=4;
							bombman.y = bombman.world_y - world_y;
							if(bombman.world_x%32!=0)
							{
								if(bombman.x<=256)
								  if ((world_x-=bombman.world_x%32) < 0)
									world_x = 0;
								bombman.world_x-=bombman.world_x%32;	//更新主角的世界坐标（绝对坐标）
								bombman.x = bombman.world_x - world_x;	//更新主角在屏幕上的位置（相对坐标）
							}							
						}
					}
					else if(world[(int)(bombman.world_y+31)/32-1] [(int)bombman.world_x/32]==1)
					{
						if(bombman.throughwall)
						{
							if(bombman.y<=224)
							  if ((world_y-=4) < 0)
							   world_y = 0;
							bombman.world_y-=4;
							bombman.y = bombman.world_y - world_y;
							if(bombman.world_x%32!=0)
							{
								if(bombman.x<=256)
								  if ((world_x-=bombman.world_x%32) < 0)
									world_x = 0;
								bombman.world_x-=bombman.world_x%32;	//更新主角的世界坐标（绝对坐标）
								bombman.x = bombman.world_x - world_x;	//更新主角在屏幕上的位置（相对坐标）
							}
						}
					}
					else//如果主角将移动的位置上没有雷
					{
							if(bombman.y<=224)
							  if ((world_y-=4) < 0)
							   world_y = 0;
							bombman.world_y-=4;
							bombman.y = bombman.world_y - world_y;
							if(bombman.world_x%32!=0)
							{
								if(bombman.x<=256)
								  if ((world_x-=bombman.world_x%32) < 0)
									world_x = 0;
								bombman.world_x-=bombman.world_x%32;	//更新主角的世界坐标（绝对坐标）
								bombman.x = bombman.world_x - world_x;	//更新主角在屏幕上的位置（相对坐标）
						}
					}
				}
			}
			else if( bombman.world_x%32 >=24 )
				if(   world[(int)(bombman.world_y+31)/32-1] [(int)bombman.world_x/32+1]<=1
					||world[(int)(bombman.world_y+31)/32-1] [(int)bombman.world_x/32+1]>=5)
				{
					if(world[(int)(bombman.world_y+31)/32-1] [(int)bombman.world_x/32+1]==1)
					{
						if(bombman.throughwall)
						{
							if(bombman.y<=224)
							  if ((world_y-=4) < 0)
							   world_y = 0;
							bombman.world_y-=4;
							bombman.y = bombman.world_y - world_y;
							if(bombman.x>=256)			//更新窗口的世界坐标（绝对坐标）
							  if ((world_x+=32-bombman.world_x%32) >192)
								world_x = 192;
							bombman.world_x+=32-bombman.world_x%32;		//更新主角的世界坐标（绝对坐标）
							bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
						}
					}
					else
					{
						if(bombman.y<=224)
						  if ((world_y-=4) < 0)
						   world_y = 0;
						bombman.world_y-=4;
						bombman.y = bombman.world_y - world_y;
						if(bombman.x>=256)			//更新窗口的世界坐标（绝对坐标）
						  if ((world_x+=32-bombman.world_x%32) >192)
							world_x = 192;
						bombman.world_x+=32-bombman.world_x%32;		//更新主角的世界坐标（绝对坐标）
						bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
					}
				}
	  } // end if(KEYDOWN(VK_UP))
	  else
	  if (KEYDOWN(VK_DOWN))
	  {
			if( bombman.world_x%32 <=8)
			{
				if( world[(int)bombman.world_y/32+1] [(int)bombman.world_x/32]<=1
				  ||world[(int)bombman.world_y/32+1] [(int)bombman.world_x/32]>=4)
				{
					//如果主角将移动的位置上有雷
					if(world[(int)bombman.world_y/32+1] [(int)bombman.world_x/32]==4)
					{
						if(bombman.world_y%32!=0)
						{
							if(bombman.y>=224)
							  if ((world_y+=4) > 128)
							   world_y = 128;
							bombman.world_y+=4;
							bombman.y = bombman.world_y - world_y;
							if(bombman.world_x%32!=0)
							{
								if(bombman.x<=256)			//更新窗口的世界坐标（绝对坐标）
								  if ((world_x-=bombman.world_x%32) < 0)
									world_x = 0;
								bombman.world_x-=bombman.world_x%32;		//更新主角的世界坐标（绝对坐标）
								bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
							}
						}
					}
					else if(world[(int)bombman.world_y/32+1] [(int)bombman.world_x/32]==1)
					{
						if(bombman.throughwall)
						{
							if(bombman.y>=224)
							  if ((world_y+=4) > 128)
							   world_y = 128;
							bombman.world_y+=4;
							bombman.y = bombman.world_y - world_y;
							if(bombman.world_x%32!=0)
							{
								if(bombman.x<=256)			//更新窗口的世界坐标（绝对坐标）
								  if ((world_x-=bombman.world_x%32) < 0)
									world_x = 0;
								bombman.world_x-=bombman.world_x%32;		//更新主角的世界坐标（绝对坐标）
								bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
							}
						}
					}
					else	//如果主角将移动的位置上没有雷
					{
						if(bombman.y>=224)
						  if ((world_y+=4) > 128)
						   world_y = 128;
						bombman.world_y+=4;
						bombman.y = bombman.world_y - world_y;
						if(bombman.world_x%32!=0)
						{
							if(bombman.x<=256)			//更新窗口的世界坐标（绝对坐标）
							  if ((world_x-=bombman.world_x%32) < 0)
								world_x = 0;
							bombman.world_x-=bombman.world_x%32;		//更新主角的世界坐标（绝对坐标）
							bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
						}
					}
				}
			}
			else if( bombman.world_x%32 >= 24 )
				if(  world[(int)bombman.world_y/32+1] [(int)bombman.world_x/32+1]<=1
				   ||world[(int)bombman.world_y/32+1] [(int)bombman.world_x/32+1]>=5)
				{
					if(world[(int)bombman.world_y/32+1] [(int)bombman.world_x/32+1]==1)
					{
						if(bombman.throughwall)
						{
							if(bombman.y>=224)
							  if ((world_y+=4) > 128)
							   world_y = 128;
							bombman.world_y+=4;
							bombman.y = bombman.world_y - world_y;
							if(bombman.x>=256)			//更新窗口的世界坐标（绝对坐标）
							  if ((world_x+=bombman.world_x%32) >192)
								world_x = 192;
							bombman.world_x+=32-bombman.world_x%32;		//更新主角的世界坐标（绝对坐标）
							bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
						}
					}
					else
					{
						if(bombman.y>=224)
						  if ((world_y+=4) > 128)
						   world_y = 128;
						bombman.world_y+=4;
						bombman.y = bombman.world_y - world_y;
						if(bombman.x>=256)			//更新窗口的世界坐标（绝对坐标）
						  if ((world_x+=bombman.world_x%32) >192)
							world_x = 192;
						bombman.world_x+=32-bombman.world_x%32;		//更新主角的世界坐标（绝对坐标）
						bombman.x = bombman.world_x - world_x;//更新主角在屏幕上的位置（相对坐标）
					}
				}
	  } // end if(KEYDOWN(VK_DOWN))

	}	// end of if (!KEYDOWN(VK_SPACE))	//如果按空格时就不能移动


		//判断主角被炸死,吃道具或过关------------------------------------------------------------
		  if(bombman.world_y%32!=0)	//主角水平方向不在“整坐标”上  
		  {  //如果 当前位置有爆炸
				if(	(world[bombman.world_y/32][bombman.world_x/32]>=5
					&& world[bombman.world_y/32][bombman.world_x/32]<=12 )					
					||(world[bombman.world_y/32+1][bombman.world_x/32]>=5
					&& world[bombman.world_y/32+1][bombman.world_x/32]<=12) )
				{
					switch(world[bombman.world_y/32][bombman.world_x/32])					
					{
						case 5:
						case 6:
						case 7:
						{
							bombman.state=DEAD;
							bombman.dead_counter=1;							
						}break;
						case 8:
						{
							bombman.throughwall=true;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break;
						case 9:
						{
							bombman.maxbomb_num++;
							bombman.bomb_num++;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break;
						case 10:
						{
							bombman.bombcontrol=true;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break;
						case 11:
						{
							bombman.firelevel++;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break; 
						case 12:
						{
							win=true;
						}
						default:break;
					} //end of switch
					switch(world[bombman.world_y/32+1][bombman.world_x/32])					
					{
						case 5:
						case 6:
						case 7:
						{
							bombman.state=DEAD;
							bombman.dead_counter=1;							
						}break;
						case 8:
						{
							bombman.throughwall=true;
							item[bombman.world_y/32+1][bombman.world_x/32]=0;
							world[bombman.world_y/32+1][bombman.world_x/32]=0;
						}break;
						case 9:
						{
							bombman.maxbomb_num++;
							bombman.bomb_num++;
							item[bombman.world_y/32+1][bombman.world_x/32]=0;
							world[bombman.world_y/32+1][bombman.world_x/32]=0;
						}break;
						case 10:
						{
							bombman.bombcontrol=true;
							item[bombman.world_y/32+1][bombman.world_x/32]=0;
							world[bombman.world_y/32+1][bombman.world_x/32]=0;
						}break;
						case 11:
						{
							bombman.firelevel++;
							item[bombman.world_y/32+1][bombman.world_x/32]=0;
							world[bombman.world_y/32+1][bombman.world_x/32]=0;
						}break; 
						case 12:
						{
							win=true;
						}
						default:break;
					} //end of switch
				}
		  }
		  else if(bombman.world_x%32!=0 )					//主角垂直方向不在“整坐标”上 
		  {
				if(  (world[bombman.world_y/32][bombman.world_x/32]>=5
				  && world[bombman.world_y/32][bombman.world_x/32]<=12 )
				  || (world[bombman.world_y/32][bombman.world_x/32+1]>=5
				  && world[bombman.world_y/32][bombman.world_x/32+1]<=12 ) )
				{
					switch(world[bombman.world_y/32][bombman.world_x/32])					
					{
						case 5:
						case 6:
						case 7:
						{
							bombman.state=DEAD;
							bombman.dead_counter=1;							
						}break;
						case 8:
						{
							bombman.throughwall=true;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break;
						case 9:
						{
							bombman.maxbomb_num++;
							bombman.bomb_num++;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break;
						case 10:
						{
							bombman.bombcontrol=true;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break;
						case 11:
						{
							bombman.firelevel++;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break; 
						case 12:
						{
							win=true;
						}
						default:break;
					} //end of switch
					switch(world[bombman.world_y/32][bombman.world_x/32+1])					
					{
						case 5:
						case 6:
						case 7:
						{
							bombman.state=DEAD;
							bombman.dead_counter=1;							
						}break;
						case 8:
						{
							bombman.throughwall=true;
							item[bombman.world_y/32][bombman.world_x/32+1]=0;
							world[bombman.world_y/32][bombman.world_x/32+1]=0;
						}break;
						case 9:
						{
							bombman.maxbomb_num++;
							bombman.bomb_num++;
							item[bombman.world_y/32][bombman.world_x/32+1]=0;
							world[bombman.world_y/32][bombman.world_x/32+1]=0;
						}break;
						case 10:
						{
							bombman.bombcontrol=true;
							item[bombman.world_y/32][bombman.world_x/32+1]=0;
							world[bombman.world_y/32][bombman.world_x/32+1]=0;
						}break;
						case 11:
						{
							bombman.firelevel++;
							item[bombman.world_y/32][bombman.world_x/32+1]=0;
							world[bombman.world_y/32][bombman.world_x/32+1]=0;
						}break; 
						case 12:
						{
							win=true;
						}
						default:break;
					} //end of switch
				}
		  }
		else		//主角位于“整坐标”上
		{
				if(  world[bombman.world_y/32][bombman.world_x/32]>=5 
				  && world[bombman.world_y/32][bombman.world_x/32]<=12)
				{
					switch(world[bombman.world_y/32][bombman.world_x/32])					
					{
						case 5:
						case 6:
						case 7:
						{
							bombman.state=DEAD;
							bombman.dead_counter=1;							
						}break;
						case 8:
						{
							bombman.throughwall=true;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break;
						case 9:
						{
							bombman.maxbomb_num++;
							bombman.bomb_num++;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break;
						case 10:
						{
							bombman.bombcontrol=true;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break;
						case 11:
						{
							bombman.firelevel++;
							item[bombman.world_y/32][bombman.world_x/32]=0;
							world[bombman.world_y/32][bombman.world_x/32]=0;
						}break; 
						case 12:
						{
							win=true;
						}
						default:break;
					} //end of switch
				}
		}
	//判断主角是否与敌人相砬，是则GAME OVER
	for(i=0; i<4; i++)
		if(   abs(bombman.world_x-ghost[i].world_x)<32 
		   && abs(bombman.world_y-ghost[i].world_y)<32 )
		{
			bombman.state=DEAD;
			break;
		}


	//放炸弹------------------------------------------------------------------
	if (KEYDOWN(VK_SPACE))		
	{
//		if(	  ((bombman.world_y%32<=8
//			|| bombman.world_y%32>=24) 
//			&& (bombman.world_x%32<=8		
//			|| bombman.world_x%32>=24) )		//这个位置可以放炸弹
		if(	 bombman.bomb_num!=0)		//并且手里有炸弹
		{	int index_i,index_j;
			//确定放哪个炸弹？
			for(index_i=0; index_i<bombman.maxbomb_num; index_i++)
				if(bomb[index_i].state==TAKE) //counter==0 && bomb[i].counter_explode==0)
				{
				  index_j=index_i;		//放第j个雷
				  break;
				}
			if(bombman.world_y%32==0)	//Y为整坐标，即只判断放左边还是放右边
			{
				if(bombman.world_x%32<=15)	//放左边
				{	
				  if(world[(int)bombman.world_y/32][(int)bombman.world_x/32]!=4)	//现在爆炸的地方也可放
				  {
					world[(int)bombman.world_y/32][(int)bombman.world_x/32]=4;	//在场景数组中设置炸弹的位置
					bomb[index_j].world_x=bombman.world_x-bombman.world_x%32;
					bomb[index_j].world_y=bombman.world_y;
					if(!bombman.bombcontrol)
						bomb[index_j].counter=1;			//炸弹开始计时
					bomb[index_j].state=SET;
					bombman.bomb_num--;	//手里的雷数减1 		
				  }
				}
				else	//放右边
				{
				  if(world[(int)bombman.world_y/32][(int)bombman.world_x/32+1]!=4)
				  {
					world[(int)bombman.world_y/32][(int)bombman.world_x/32+1]=4;	//在场景数组中设置炸弹的位置
					bomb[index_j].world_x=bombman.world_x+32-bombman.world_x%32;
					bomb[index_j].world_y=bombman.world_y;
					if(!bombman.bombcontrol)					
						bomb[index_j].counter=1;			//炸弹开始计时
					bomb[index_j].state=SET;
					bombman.bomb_num--;	//手里的雷数减1 
				  }
				}
			}
			else	//X为整坐标，即只判断放上面还是放下面
			{
				if(bombman.world_y%32<=15)	//	放上面
				{
				  if(world[(int)bombman.world_y/32][(int)bombman.world_x/32]!=4)
				  {
					world[(int)bombman.world_y/32][(int)bombman.world_x/32]=4;	//在场景数组中设置炸弹的位置
					bomb[index_j].world_x=bombman.world_x;
					bomb[index_j].world_y=bombman.world_y-bombman.world_y%32;	
					if(!bombman.bombcontrol)
						bomb[index_j].counter=1;			//炸弹开始计时
					bomb[index_j].state=SET;
					bombman.bomb_num--;	//手里的雷数减1 
				  }
				}
				else			//放下面
				{
				  if(world[(int)bombman.world_y/32+1][(int)bombman.world_x/32]!=4)
				  {
					world[(int)bombman.world_y/32+1][(int)bombman.world_x/32]=4;	//在场景数组中设置炸弹的位置
					bomb[index_j].world_x=bombman.world_x;
					bomb[index_j].world_y=bombman.world_y+32-bombman.world_y%32;	
					if(!bombman.bombcontrol)
						bomb[index_j].counter=1;			//炸弹开始计时
					bomb[index_j].state=SET;
					bombman.bomb_num--;	//手里的雷数减1 
				  }
				}
			}
		}
	}

	//爆炸-----------------------------------------------------------------------
	if(!bombman.bombcontrol)
	{
		for(j=0;j<bombman.maxbomb_num; j++)		//扫瞄每一个炸弹（包括放下的和没放下的）
		  if(bomb[j].state == SET)				//如果是已放下的雷
			if(++bomb[j].counter>40)
			{	
				world[bomb[j].world_y/32][bomb[j].world_x/32] = 5;	//准备在炸弹位置画爆炸贴图
				bomb[j].counter=0;
				bomb[j].counter_explode=1;
				bomb[j].state=EXPLODE;

				//这里采用switch语句是为了以后处理更多情况
				//炸弹上方 
				bool flag=true;
				for(i=1; i<=bombman.firelevel && flag; i++)
				{
					switch(world[bomb[j].world_y/32-i][bomb[j].world_x/32])	
					{
						case 1:
						{
							world[bomb[j].world_y/32-i][bomb[j].world_x/32]=7;
							flag=false;
						} break; 
						case 2:
						case 3:
						{
							flag=false;
						}break; 
						case 4:	//如果是炸弹就引爆
						{		
							for(int b=0; b<bombman.maxbomb_num; b++)	
								if(bomb[b].world_x==bomb[j].world_x 
								  && bomb[b].world_y == bomb[j].world_y-32*i
								  && bomb[b].state==SET)
								{
							 		bomb[b].counter=35;//或39
									flag=false;
									break;
								}
						}break;
						case 5:	//交叉火力
						case 6:	//交叉火力
						{
							world[bomb[j].world_y/32-i][bomb[j].world_x/32]=5;
						}break; 
						case 0:
						case 7:	
						case 8:
						case 9:
						case 10:
						case 11:
						case 12:
						{
							world[bomb[j].world_y/32-i][bomb[j].world_x/32]=7;
						} break; 
						default:break;
					}
				}

				//炸弹下方 
				flag=true;
				for(i=1; i<=bombman.firelevel && flag; i++)
				{
					switch(world[bomb[j].world_y/32+i][bomb[j].world_x/32])	
					{
						case 1:
						{
							world[bomb[j].world_y/32+i][bomb[j].world_x/32]=7;
							flag=false;
						} break; 
						case 2:
						case 3:
						{
							flag=false;
						}break; 
						case 4:	//如果是炸弹就引爆
						{		
							for(int b=0; b<bombman.maxbomb_num; b++)	
								if(bomb[b].world_x==bomb[j].world_x 
								  && bomb[b].world_y == bomb[j].world_y+32*i
								  && bomb[b].state==SET)
								{
							 		bomb[b].counter=35;//或39
									flag=false;
									break;
								}
						}break;
						case 5:	//交叉火力
						case 6:	//交叉火力
						{
							world[bomb[j].world_y/32+i][bomb[j].world_x/32]=5;
						}break; 
						case 0:
						case 7:	
						case 8:
						case 9:
						case 10:
						case 11:
						case 12:
						{
							world[bomb[j].world_y/32+i][bomb[j].world_x/32]=7;
						} break; 
						default:break;
					}
				}

				//炸弹左方
				flag=true;
				for(i=1; i<=bombman.firelevel && flag; i++)
				{
					switch(world[bomb[j].world_y/32][bomb[j].world_x/32-i])	
					{
						case 1:
						{
							world[bomb[j].world_y/32][bomb[j].world_x/32-i]=6;
							flag=false;
						} break; 
						case 2:
						case 3:
						{
							flag=false;
						}break; 
						case 4:	//如果是炸弹就引爆
						{		
							for(int b=0; b<bombman.maxbomb_num; b++)	
								if(  bomb[b].world_x == bomb[j].world_x-32*i
								  && bomb[b].world_y == bomb[j].world_y
								  && bomb[b].state==SET)
								{
							 		bomb[b].counter=35;//或39
									flag=false;
									break;
								}
						}break;
						case 5:	//交叉火力
						case 7:	//交叉火力
						{
							world[bomb[j].world_y/32][bomb[j].world_x/32-i]=5;
						}break; 
						case 0:
						case 6:	
						case 8:
						case 9:
						case 10:
						case 11:
						case 12:
						{
							world[bomb[j].world_y/32][bomb[j].world_x/32-i]=6;
						} break; 
						default:break;
					}
				}

				//炸弹右方 
				flag=true;
				for(i=1; i<=bombman.firelevel && flag; i++)
				{
					switch(world[bomb[j].world_y/32][bomb[j].world_x/32+i])	
					{
						case 1:
						{
							world[bomb[j].world_y/32][bomb[j].world_x/32+i]=6;
							flag=false;
						} break; 
						case 2:
						case 3:
						{
							flag=false;
						}break; 
						case 4:	//如果是炸弹就引爆
						{		
							for(int b=0; b<bombman.maxbomb_num; b++)	
								if(bomb[b].world_x==bomb[j].world_x +32*i
								  && bomb[b].world_y == bomb[j].world_y
								  && bomb[b].state==SET)
								{
							 		bomb[b].counter=35;//或39
									flag=false;
									break;
								}
						}break;
						case 5:	//交叉火力
						case 7:	//交叉火力
						{
							world[bomb[j].world_y/32][bomb[j].world_x/32+i]=5;
						}break; 
						case 0:
						case 6:	
						case 8:
						case 9:
						case 10:
						case 11:
						case 12:
						{
							world[bomb[j].world_y/32][bomb[j].world_x/32+i]=6;
						} break; 
						default:break;
					}	// end of switch
				}	//end of for
			}	//end of if counter>40
	}		//end of 不可控制
	else	//能控制炸弹
	{
		if (KEYDOWN(VK_CONTROL))	//CTRL键引爆		
		{
			bool explode_flag=false;
			for(j=0;j<bombman.maxbomb_num && !explode_flag; j++)		//扫瞄每一个炸弹（包括放下的和没放下的）
			  if(bomb[j].state == SET)				//如果是已放下的雷
				{	
					world[bomb[j].world_y/32][bomb[j].world_x/32] = 5;	//准备在炸弹位置画爆炸贴图
					bomb[j].counter=0;
					bomb[j].counter_explode=1;
					bomb[j].state=EXPLODE;
					explode_flag=true;

					//这里采用switch语句是为了以后处理更多情况
					//炸弹上方 
					bool flag=true;
					for(i=1; i<=bombman.firelevel && flag; i++)
					{
						switch(world[bomb[j].world_y/32-i][bomb[j].world_x/32])	
						{
							case 1:
							{
								world[bomb[j].world_y/32-i][bomb[j].world_x/32]=7;
								flag=false;
							} break; 
							case 2:
							case 3:
							{
								flag=false;
							}break; 
							case 4:	//如果是炸弹就引爆
							{		
								for(int b=0; b<bombman.maxbomb_num; b++)	
									if(bomb[b].world_x==bomb[j].world_x 
									  && bomb[b].world_y == bomb[j].world_y-32*i
									  && bomb[b].state==SET)
									{
							 			bomb[b].counter=35;//或39
										flag=false;
										break;
									}
							}break;
							case 5:	//交叉火力
							case 6:	//交叉火力
							{
								world[bomb[j].world_y/32-i][bomb[j].world_x/32]=5;
							}break; 
							case 0:
							case 7:	
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
							{
								world[bomb[j].world_y/32-i][bomb[j].world_x/32]=7;
							} break; 
							default:break;
						}
					}

					//炸弹下方 
					flag=true;
					for(i=1; i<=bombman.firelevel && flag; i++)
					{
						switch(world[bomb[j].world_y/32+i][bomb[j].world_x/32])	
						{
							case 1:
							{
								world[bomb[j].world_y/32+i][bomb[j].world_x/32]=7;
								flag=false;
							} break; 
							case 2:
							case 3:
							{
								flag=false;
							}break; 
							case 4:	//如果是炸弹就引爆
							{		
								for(int b=0; b<bombman.maxbomb_num; b++)	
									if(bomb[b].world_x==bomb[j].world_x 
									  && bomb[b].world_y == bomb[j].world_y+32*i
									  && bomb[b].state==SET)
									{
							 			bomb[b].counter=35;//或39
										flag=false;
										break;
									}
							}break;
							case 5:	//交叉火力
							case 6:	//交叉火力
							{
								world[bomb[j].world_y/32+i][bomb[j].world_x/32]=5;
							}break; 
							case 0:
							case 7:	
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
							{
								world[bomb[j].world_y/32+i][bomb[j].world_x/32]=7;
							} break; 
							default:break;
						}
					}

					//炸弹左方
					flag=true;
					for(i=1; i<=bombman.firelevel && flag; i++)
					{
						switch(world[bomb[j].world_y/32][bomb[j].world_x/32-i])	
						{
							case 1:
							{
								world[bomb[j].world_y/32][bomb[j].world_x/32-i]=6;
								flag=false;
							} break; 
							case 2:
							case 3:
							{
								flag=false;
							}break; 
							case 4:	//如果是炸弹就引爆
							{		
								for(int b=0; b<bombman.maxbomb_num; b++)	
									if(  bomb[b].world_x == bomb[j].world_x-32*i
									  && bomb[b].world_y == bomb[j].world_y
									  && bomb[b].state==SET)
									{
							 			bomb[b].counter=35;//或39
										flag=false;
										break;
									}
							}break;
							case 5:	//交叉火力
							case 7:	//交叉火力
							{
								world[bomb[j].world_y/32][bomb[j].world_x/32-i]=5;
							}break; 
							case 0:
							case 6:	
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
							{
								world[bomb[j].world_y/32][bomb[j].world_x/32-i]=6;
							} break; 
							default:break;
						}
					}

					//炸弹右方 
					flag=true;
					for(i=1; i<=bombman.firelevel && flag; i++)
					{
						switch(world[bomb[j].world_y/32][bomb[j].world_x/32+i])	
						{
							case 1:
							{
								world[bomb[j].world_y/32][bomb[j].world_x/32+i]=6;
								flag=false;
							} break; 
							case 2:
							case 3:
							{
								flag=false;
							}break; 
							case 4:	//如果是炸弹就引爆
							{		
								for(int b=0; b<bombman.maxbomb_num; b++)	
									if(bomb[b].world_x==bomb[j].world_x +32*i
									  && bomb[b].world_y == bomb[j].world_y
									  && bomb[b].state==SET)
									{
							 			bomb[b].counter=35;//或39
										flag=false;
										break;
									}
							}break;
							case 5:	//交叉火力
							case 7:	//交叉火力
							{
								world[bomb[j].world_y/32][bomb[j].world_x/32+i]=5;
							}break; 
							case 0:
							case 6:	
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
							{
								world[bomb[j].world_y/32][bomb[j].world_x/32+i]=6;
							} break; 
							default:break;
						}	// end of switch
					}	//end of for
				}	//end of if state==SET
		}
	}

	//爆炸结束-----------------------------------------------------------------
	for(j=0;j<bombman.maxbomb_num; j++)//扫瞄每一个炸弹（包括放下的和没放下的）
	  if(bomb[j].state==EXPLODE)		//如果当前雷为爆炸结束			
		if(++bomb[j].counter_explode>5)
		{
		  bomb[j].counter_explode=0;	//爆炸结束
		  bomb[j].state=TAKE;
		  bombman.bomb_num++;	//手里的炸弹多一个
			
		  if(item[bomb[j].world_y/32][bomb[j].world_x/32]>=8)	//还原爆炸点的图片
			world[bomb[j].world_y/32][bomb[j].world_x/32]=item[bomb[j].world_y/32][bomb[j].world_x/32];
		  else
			world[bomb[j].world_y/32][bomb[j].world_x/32] = 0;

		  //炸弹上方
		  for(i=1; i<=bombman.firelevel;i++)
			  if(world[bomb[j].world_y/32-i][bomb[j].world_x/32]==7
				  ||world[bomb[j].world_y/32-i][bomb[j].world_x/32]==5
				  ||world[bomb[j].world_y/32-i][bomb[j].world_x/32]==6)	
			  {
				if(item[bomb[j].world_y/32-i][bomb[j].world_x/32]>=8)
					world[bomb[j].world_y/32-i][bomb[j].world_x/32]=item[bomb[j].world_y/32-i][bomb[j].world_x/32];
				else
					world[bomb[j].world_y/32-i][bomb[j].world_x/32]=0;
			  }	
			  else 
				  break;
		  
		  //炸弹下方
		  for(i=1; i<=bombman.firelevel;i++)
			  if(world[bomb[j].world_y/32+i][bomb[j].world_x/32]==7
				 ||world[bomb[j].world_y/32+i][bomb[j].world_x/32]==5
				 ||world[bomb[j].world_y/32+i][bomb[j].world_x/32]==6)	
			  {
						if(item[bomb[j].world_y/32+i][bomb[j].world_x/32]>=8)
							world[bomb[j].world_y/32+i][bomb[j].world_x/32]=item[bomb[j].world_y/32+i][bomb[j].world_x/32];
						else
							world[bomb[j].world_y/32+i][bomb[j].world_x/32]=0;
			  }	
			  else 
				  break;			  
		  
		  //炸弹左方
		  for(i=1; i<=bombman.firelevel;i++)
			  if(world[bomb[j].world_y/32][bomb[j].world_x/32-i]==7
				||world[bomb[j].world_y/32][bomb[j].world_x/32-i]==5
				||world[bomb[j].world_y/32][bomb[j].world_x/32-i]==6)	
			  {
						if(item[bomb[j].world_y/32][bomb[j].world_x/32-i]>=8)
							world[bomb[j].world_y/32][bomb[j].world_x/32-i]=item[bomb[j].world_y/32][bomb[j].world_x/32-i];
						else
							world[bomb[j].world_y/32][bomb[j].world_x/32-i]=0;
			  }		
			  else 
				  break;			  
		  
		  //炸弹右方
		  for(i=1; i<=bombman.firelevel;i++)
			  if(world[bomb[j].world_y/32][bomb[j].world_x/32+i]==7
				||world[bomb[j].world_y/32][bomb[j].world_x/32+i]==5
				||world[bomb[j].world_y/32][bomb[j].world_x/32+i]==6 )	
			  {
						if(item[bomb[j].world_y/32][bomb[j].world_x/32+i]>=8)
							world[bomb[j].world_y/32][bomb[j].world_x/32+i]=item[bomb[j].world_y/32][bomb[j].world_x/32+i];
						else
							world[bomb[j].world_y/32][bomb[j].world_x/32+i]=0;
			  }
			  else
				  break;
		}

	//移动敌人-----------------------------------------------------------------------
	for(i=0; i<4; i++)
	{
		if(ghost[i].state==ALIVE)
		{
			//先判断是否要改变方向
			if( ghost[i].cannotmove											//不能移动了
				|| ( (ghost[i].world_x-32)%64==0 && (ghost[i].world_y-32)%64==0	//或正处在叉路口
				&& rand()%2==1 )	)									//且想要“改变”
			{
				int dir;
				if( ghost[i].cannotmove )
				{	
					do{
						dir=rand()%5-2;
					}while(dir == ghost[i].direction || dir==0 );
					ghost[i].direction=dir;
					ghost[i].cannotmove=false;
				}
				else if( (ghost[i].world_x/32)%2!=0 && (ghost[i].world_x/32)%2!=0	
						&& rand()%2==1 )
				{
					do{
						dir=rand()%5-2;
					}while(dir == ghost[i].direction || dir==-ghost[i].direction  || dir==0 );
					ghost[i].direction=dir;
					//ghost.direction=rand()%4;
				}
			}
			//判断是哪个方向/////////////
			switch(ghost[i].direction)
			{
			case -1://向上
			{
			  if( (world[(int)(ghost[i].world_y+31)/32-1] [(int)ghost[i].world_x/32] == 0
					||world[(int)(ghost[i].world_y+31)/32-1] [(int)ghost[i].world_x/32] == 5
					||world[(int)(ghost[i].world_y+31)/32-1] [(int)ghost[i].world_x/32] == 6
					||world[(int)(ghost[i].world_y+31)/32-1] [(int)ghost[i].world_x/32] == 7)
					&&(ghost[i].world_x%32==0) )
				{
					ghost[i].world_y-=2;
				}
			  else ghost[i].cannotmove=true;
			}break; 
			case 1://向下
			{
				if( (world[(int)ghost[i].world_y/32+1] [(int)ghost[i].world_x/32] == 0
					||world[(int)ghost[i].world_y/32+1] [(int)ghost[i].world_x/32] == 5
					||world[(int)ghost[i].world_y/32+1] [(int)ghost[i].world_x/32] == 6
					||world[(int)ghost[i].world_y/32+1] [(int)ghost[i].world_x/32] == 7)
					&&(ghost[i].world_x%32==0) )
				{
					ghost[i].world_y+=2;
				}
			  else ghost[i].cannotmove=true;
			}break; 
			case -2:	//向左
			{
				if(( world[(int)ghost[i].world_y/32] [(int)(ghost[i].world_x+31)/32-1]==0
					||world[(int)ghost[i].world_y/32] [(int)(ghost[i].world_x+31)/32-1]==5
					||world[(int)ghost[i].world_y/32] [(int)(ghost[i].world_x+31)/32-1]==6
					||world[(int)ghost[i].world_y/32] [(int)(ghost[i].world_x+31)/32-1]==7)
					&&(ghost[i].world_y%32==0) )
				{
					ghost[i].world_x-=2;		//更新主角的世界坐标（绝对坐标）
				}
			  else ghost[i].cannotmove=true;
			}break; 
			case 2:		//向右
			{
				if( (world[(int)ghost[i].world_y/32] [(int)(ghost[i].world_x/32)+1]==0
					||world[(int)ghost[i].world_y/32] [(int)(ghost[i].world_x/32)+1]==5
					||world[(int)ghost[i].world_y/32] [(int)(ghost[i].world_x/32)+1]==6
					||world[(int)ghost[i].world_y/32] [(int)(ghost[i].world_x/32)+1]==7)
					&&(ghost[i].world_y%32==0) )
				{
					ghost[i].world_x+=2;		//更新主角的世界坐标
				}
			  else ghost[i].cannotmove=true;
			}break; 
			default:break;
			}
		}
	}	//移动结束	end of for
	for(i=0; i<4; i++)
	{
		ghost[i].y = ghost[i].world_y - world_y;
		ghost[i].x = ghost[i].world_x - world_x;	//更新敌人在屏幕上的位置（相对坐标）
	}

	//画场景--(重点代码）-----------------------------------------------------------------
	start_map_x = world_x/32; // use >> 6 for speed, but this is clearer
	start_map_y = world_y/32; 
	end_map_x = start_map_x + 16;	//这里先按对齐情况计算，下面再调节	
	end_map_y = start_map_y + 14;
	 // 偏移量
	offset_x = -(world_x % 32);
	offset_y = -(world_y % 32);
	// 如果有偏移，调节末端的TILE，
	if (offset_x)		
	   end_map_x++;
	if (offset_y)
	   end_map_y++;
	// 贴图在屏幕上的位置（相对坐标）
	texture.x = offset_x;
	texture.y = offset_y;
	// draw the current window
	for (int index_y = start_map_y; index_y <= end_map_y; index_y++)
		{
		for (int index_x = start_map_x; index_x <= end_map_x; index_x++)
			{
			DDraw_Draw_Surface( texture.tile[world[index_y][index_x]-1] , 
								texture.x, texture.y,
								32, 32,
								lpddsbackground);
			// update texture position
			texture.x+=32;
			}
		// reset x postion, update y
		texture.x =  offset_x;
		texture.y += 32;
		} // end for map_y

	//--------------------------------------------------------------------------------
	//敌人死亡   判断敌人当前位置是否有爆炸
	for(i=0; i<4; i++)
	{
		if(	ghost[i].world_y%32!=0 || ghost[i].world_x%32!=0 		 )
		{
		  if(ghost[i].world_y%32!=0)	//敌人水平方向不在“整坐标”上  
		  {  //如果 当前位置有爆炸
				if(world[ghost[i].world_y/32][ghost[i].world_x/32]==5
					|| world[ghost[i].world_y/32][ghost[i].world_x/32]==6 
					|| world[ghost[i].world_y/32][ghost[i].world_x/32]==7
					|| world[ghost[i].world_y/32+1][ghost[i].world_x/32]==5
					|| world[ghost[i].world_y/32+1][ghost[i].world_x/32]==6 
					|| world[ghost[i].world_y/32+1][ghost[i].world_x/32]==7)
				{
				  ghost[i].state=DEAD;
				  ghost[i].dead_counter=1;
				  score+=25;
				}
		  }
		  else						//敌人垂直方向不在“整坐标”上 
		  {
				if(world[ghost[i].world_y/32][ghost[i].world_x/32]==5
					|| world[ghost[i].world_y/32][ghost[i].world_x/32]==6 
					|| world[ghost[i].world_y/32][ghost[i].world_x/32]==7
					|| world[ghost[i].world_y/32][ghost[i].world_x/32+1]==5
					|| world[ghost[i].world_y/32][ghost[i].world_x/32+1]==6 
					|| world[ghost[i].world_y/32][ghost[i].world_x/32+1]==7)
				{
				  ghost[i].state=DEAD;
				  ghost[i].dead_counter=1;
				  score+=25;
				}
		  }
		}
		else		//敌人位于“整坐标”上
		{
				if(world[ghost[i].world_y/32][ghost[i].world_x/32]==5 
					|| world[ghost[i].world_y/32][ghost[i].world_x/32]==6 
					|| world[ghost[i].world_y/32][ghost[i].world_x/32]==7)
				{
				  ghost[i].state=DEAD;
				  ghost[i].dead_counter=1;
				  score+=25;
				}
		}
	}



	//画敌人---------------------------------------------------------
	for(i=0; i<4; i++)
	{
		if(ghost[i].state==ALIVE)
			DDraw_Draw_Surface( ghost[i].frame[0],			
								ghost[i].x, ghost[i].y,
								32, 32,
								lpddsbackground);
		else
			DDraw_Draw_Surface( ghost[i].frame[1] ,			
								ghost[i].x, ghost[i].y,
								32, 32,
								lpddsbackground); 
	}

	//敌人死亡画面结束-----------------------------------------------------------
	for(i=0; i<4; i++)
	{
		if(ghost[i].dead_counter)			//敌人死亡画面结束
			if(++ghost[i].dead_counter>10)
			{
				Color_Fill_Surface(lpddsbackground,			// 目标表面
							ghost[i].x, ghost[i].y,                 // 目标表面的X,Y
							32,  32,        // 目标和宽，和高
							0);		//color

				do{
				ghost[i].world_x=(rand()%21+1)*32;	//(rand()%9*2+1)*32; //初始化敌人位置
				ghost[i].world_y=(rand()%17+1)*32;//(rand()%8*2+1)*32;
				}while(world[ghost[i].world_y/32][ghost[i].world_x/32]!=0
					  || abs(ghost[i].world_x-bombman.world_x)<256 
					  || abs(ghost[i].world_y-bombman.world_y)<128 
					);	//确保不让敌入生在石头里不和主角生成太近
				ghost[i].x=ghost[i].world_x-world_x;
				ghost[i].y=ghost[i].world_y-world_y;
				do
				{
				  ghost[i].direction = rand()%5-2;//初始化方向
				}while(ghost[i].direction==0);
				ghost[i].cannotmove = false;
				ghost[i].state=ALIVE;
				ghost[i].dead_counter=0;
			}
	}

	//画主角---------------------------------------------------------
	for(i=0; i<4; i++)
	{
		if(bombman.state==ALIVE)
			DDraw_Draw_Surface( bombman.frame[0],			
								bombman.x, bombman.y,
								32, 32,
								lpddsbackground);
		else
			DDraw_Draw_Surface( bombman.frame[1] ,			
								bombman.x, bombman.y,
								32, 32,
								lpddsbackground); 
	}
	//主角死亡画面结束-----------------------------------------------------------

	//将背景画到后备表面
	DDraw_Draw_Surface( lpddsbackground2 , 
						0, 0,
					    644, 480,
						lpddsback);

	//将场景画到后备表面
	DDraw_Draw_Surface( lpddsbackground , 
						0, 0,
						544, 480,
						lpddsback);



	// 用来调试时在屏幕打印即时数据
	HDC hdc;
	char buffer[80];

	if (FAILED( lpddsback->GetDC(&hdc) ))
	   return(0);
	// set the colors for the text up
	SetTextColor(hdc,RGB(255,255,255) );
	// set background mode to transparent so black isn't copied
	SetBkMode(hdc, TRANSPARENT);

	sprintf(buffer,"炸弹数:%d",bombman.maxbomb_num);
	TextOut(hdc,SCREEN_WIDTH-96,5,buffer,strlen(buffer) );
	sprintf(buffer,"分数:%d", score);
	TextOut(hdc,SCREEN_WIDTH-96,20,buffer,strlen(buffer) );

	lpddsback->ReleaseDC(hdc);

	// 将后备缓冲切到主表面
	while (FAILED(  lpddsprimary->Flip(NULL, DDFLIP_WAIT)));

	// wait 
	Sleep(40);	//一秒25帧，一帧40毫秒

	// return success or failure or your own return code here
	return(1);

} // end Game_Main

//***************************************************************************
int Game_Shutdown(void *parms , int num_parms )
{
// this is called after the game is exited and the main event
// loop while is exited, do all you cleanup and shutdown here

// kill all the surfaces

// first the palette
if (lpddpal)
   {
   lpddpal->Release();
   lpddpal = NULL;
   } // end if

// now the primary surface
if (lpddsprimary)
   {
   lpddsprimary->Release();
   lpddsprimary = NULL;
   } // end if

// now blow away the IDirectDraw4 interface
if (lpdd)
   {
   lpdd->Release();
   lpdd = NULL;
   } // end if

// return success or failure or your own return code here
return(1);

} // end Game_Shutdown


//*************************************************************

//游戏主程序////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE hprevinsstance,
				   LPSTR lpcmdline,
				   int ncmdshow)
{
WNDCLASSEX winclass;	
HWND hwnd;				
MSG msg;				
HDC hdc;				
PAINTSTRUCT ps;
// first fill in the window class stucture
winclass.cbSize         = sizeof(WNDCLASSEX);
winclass.style			= CS_DBLCLKS | CS_OWNDC | 
                          CS_HREDRAW | CS_VREDRAW;
winclass.lpfnWndProc	= WindowProc;
winclass.cbClsExtra		= 0;
winclass.cbWndExtra		= 0;
winclass.hInstance		= hinstance;
winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
winclass.hCursor		= LoadCursor(NULL, IDC_ARROW); 
winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);  //黑色背景
winclass.lpszMenuName	= NULL;									//菜单
winclass.lpszClassName	= WINDOW_CLASS_NAME;					//类名
winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);		//

hinstance_app = hinstance;
// 注册窗口类
if (!RegisterClassEx(&winclass))
	return(0);
// 建立窗口
if (!(hwnd = CreateWindowEx(NULL,                  // extended style
                            WINDOW_CLASS_NAME,     // class
						    "Terris Demo",         // title
						    WS_POPUP | WS_VISIBLE,
					 	    0,0,	  // initial x,y
						    SCREEN_WIDTH,SCREEN_HEIGHT,  // initial width, height
						    NULL,	  // handle to parent 
						    NULL,	  // handle to menu
						    hinstance,// instance of this application
						    NULL)))	// extra creation parms
return(0);

main_window_handle = hwnd;	// save main window handle

// initialize game here
Game_Init();

// enter main event loop
while(TRUE)
	{
    // test if there is a message in queue, if so get it
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	   { 
	   // test if this is a quit
       if (msg.message == WM_QUIT)
           break;
	
	   // translate any accelerator keys
	   TranslateMessage(&msg);

	   // send the message to the window proc
	   DispatchMessage(&msg);
	   } // end if
    
       // main game processing goes here
       Game_Main();
       
	} // end while

// closedown game here
Game_Shutdown();

// return to Windows like this
return(msg.wParam);

} // end WinMain

// 函数定义 //函数定义//函数定义/////函数定义///////函数定义///////////////////////////////

int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename)
{
// this function opens a bitmap file and loads the data into bitmap

	int file_handle,  // the file handle
		index;        // looping index

	UCHAR   *temp_buffer = NULL; // used to convert 24 bit images to 16 bit
	OFSTRUCT file_data;          // the file data information

	//  打开文件如果文件存在的话open the file if it exists
	if ((file_handle = OpenFile(filename,&file_data,OF_READ))==-1)
	   return(0);

	// 载入文件头
	_lread(file_handle, &bitmap->bitmapfileheader,sizeof(BITMAPFILEHEADER));

	//是否为位图
	if (bitmap->bitmapfileheader.bfType!=BITMAP_ID)
	   {
	   _lclose(file_handle);	   // close the file
	   return(0);	   // return error
	   } // end if

	// now we know this is a bitmap, so read in all the sections

	// 载入信息头 now load the bitmap info header
	_lread(file_handle, &bitmap->bitmapinfoheader,sizeof(BITMAPINFOHEADER));

	// 载入调色板
	if (bitmap->bitmapinfoheader.biBitCount == 8)	//如果有调色板的话
	   {
	   _lread(file_handle, &bitmap->palette,MAX_COLORS_PALETTE*sizeof(PALETTEENTRY));
	   // now set all the flags in the palette correctly and fix the reversed 
	   // BGR RGBQUAD data format
	   for (index=0; index < MAX_COLORS_PALETTE; index++)
		   {
		   // 反转红和绿色，
		   int temp_color                = bitmap->palette[index].peRed;
		   bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
		   bitmap->palette[index].peBlue = temp_color;
       
		   // always set the flags word to this
		   bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
		   } // end for index
		} // end if

	// finally the image data itself
	_lseek(file_handle,-(int)(bitmap->bitmapinfoheader.biSizeImage),SEEK_END);

	// now read in the image, if the image is 8 or 16 bit then simply read it
	// but if its 24 bit then read it into a temporary area and then convert
	// it to a 16 bit image

	if (bitmap->bitmapinfoheader.biBitCount==8 || bitmap->bitmapinfoheader.biBitCount==16 || 
		bitmap->bitmapinfoheader.biBitCount==24)
	   {
	   // delete the last image if there was one
	   if (bitmap->buffer)
		   free(bitmap->buffer);

	   // allocate the memory for the image
	   if (!(bitmap->buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		  {
		  // close the file
		  _lclose(file_handle);

		  // return error
		  return(0);
		  } // end if

	   // now read it in
	   _lread(file_handle,bitmap->buffer,bitmap->bitmapinfoheader.biSizeImage);

	   } // end if
	else
	   {//不是8位，16位或是24位
	   // serious problem
	   return(0);

	   } // end else

	#if 0
	// write the file info out 
	printf("\nfilename:%s \nsize=%d \nwidth=%d \nheight=%d \nbitsperpixel=%d \ncolors=%d \nimpcolors=%d",
			filename,
			bitmap->bitmapinfoheader.biSizeImage,
			bitmap->bitmapinfoheader.biWidth,
			bitmap->bitmapinfoheader.biHeight,
			bitmap->bitmapinfoheader.biBitCount,
			bitmap->bitmapinfoheader.biClrUsed,
			bitmap->bitmapinfoheader.biClrImportant);
	#endif

	// close the file
	_lclose(file_handle);

	// flip the bitmap
	Flip_Bitmap(bitmap->buffer, 
				bitmap->bitmapinfoheader.biWidth*(bitmap->bitmapinfoheader.biBitCount/8), 
				bitmap->bitmapinfoheader.biHeight);
	//int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height)

	// return success
	return(1);

} // end Load_Bitmap_File

///***********************************************************

int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap)
{
// this function releases all memory associated with "bitmap"
	if (bitmap->buffer)
	   {
	   // release memory
	   free(bitmap->buffer);

	   // reset pointer
	   bitmap->buffer = NULL;

	   } // end if

	// return success
	return(1);

} // end Unload_Bitmap_File

//**********************************************************

int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height)
{
// this function is used to flip bottom-up .BMP images

	UCHAR *buffer; // used to perform the image processing
	int index;     // looping index

	// allocate the temporary buffer
	if (!(buffer = (UCHAR *)malloc(bytes_per_line*height)))
	   return(0);

	// copy image to work area
	memcpy(buffer,image,bytes_per_line*height);

	// flip vertically
	for (index=0; index < height; index++)
		memcpy(&image[((height-1) - index)*bytes_per_line],
			   &buffer[index*bytes_per_line], bytes_per_line);

	// release the memory
	free(buffer);

	// return success
	return(1);

} // end Flip_Bitmap

///////////////////////////////////////////////////////////////

LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,
                                         int num_rects,
                                         LPRECT clip_list)
{
// this function creates a clipper from the sent clip list and attaches
// it to the sent surface

	int index;                         // looping var
	LPDIRECTDRAWCLIPPER lpddclipper;   // pointer to the newly created dd clipper
	LPRGNDATA region_data;             // pointer to the region data that contains
									   // the header and clip list

	// first create the direct draw clipper
	if (FAILED(lpdd->CreateClipper(0,&lpddclipper,NULL)))
	   return(NULL);

	// now create the clip list from the sent data

	// first allocate memory for region data
	region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER)+num_rects*sizeof(RECT));

	// now copy the rects into region data
	memcpy(region_data->Buffer, clip_list, sizeof(RECT)*num_rects);

	// set up fields of header
	region_data->rdh.dwSize          = sizeof(RGNDATAHEADER);
	region_data->rdh.iType           = RDH_RECTANGLES;
	region_data->rdh.nCount          = num_rects;
	region_data->rdh.nRgnSize        = num_rects*sizeof(RECT);

	region_data->rdh.rcBound.left    =  64000;
	region_data->rdh.rcBound.top     =  64000;
	region_data->rdh.rcBound.right   = -64000;
	region_data->rdh.rcBound.bottom  = -64000;

	// find bounds of all clipping regions
	for (index=0; index<num_rects; index++)
		{
		// test if the next rectangle unioned with the current bound is larger
		if (clip_list[index].left < region_data->rdh.rcBound.left)
		   region_data->rdh.rcBound.left = clip_list[index].left;

		if (clip_list[index].right > region_data->rdh.rcBound.right)
		   region_data->rdh.rcBound.right = clip_list[index].right;

		if (clip_list[index].top < region_data->rdh.rcBound.top)
		   region_data->rdh.rcBound.top = clip_list[index].top;

		if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
		   region_data->rdh.rcBound.bottom = clip_list[index].bottom;

		} // end for index

	// now we have computed the bounding rectangle region and set up the data
	// now let's set the clipping list

	if (FAILED(lpddclipper->SetClipList(region_data, 0)))
	   {
	   // release memory and return error
	   free(region_data);
	   return(NULL);
	   } // end if

	// now attach the clipper to the surface
	if (FAILED(lpdds->SetClipper(lpddclipper)))
	   {
	   // release memory and return error
	   free(region_data);
	   return(NULL);
	   } // end if

	// all is well, so release memory and send back the pointer to the new clipper
	free(region_data);
	return(lpddclipper);

} // end DDraw_Attach_Clipper


////用指定color填充入指定表面//////////////////////////////////////////////////   
int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds,int color)
{
	DDBLTFX ddbltfx; // this contains the DDBLTFX structure

	// clear out the structure and set the size field 
	DDRAW_INIT_STRUCT(ddbltfx);

	// set the dwfillcolor field to the desired color
	ddbltfx.dwFillColor = color; 

	// ready to blt to surface
	lpdds->Blt(NULL,       // ptr to dest rectangle NULL为整个表面
			   NULL,       // ptr to source surface, NA            
			   NULL,       // ptr to source rectangle, NA
			   DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait                   
			   &ddbltfx);  // ptr to DDBLTFX structure

	// return success
	return(1);
} // end DDraw_Fill_Surface

//在目标表面任意矩形内填充颜色
int Color_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds,		//目标表面
						int x, int y,                 // 目标表面的X,Y
						int width,  int height,        // 目标和宽，和高
						int color)
{
	DDBLTFX ddbltfx; // this contains the DDBLTFX structure

	// clear out the structure and set the size field 
	DDRAW_INIT_STRUCT(ddbltfx);

	// set the dwfillcolor field to the desired color
	ddbltfx.dwFillColor = color; 

	RECT dest_rect;  // the destination rectangle
	// fill in the destination rect
	dest_rect.left   = x;
	dest_rect.top    = y;
	dest_rect.right  = x+width;
	dest_rect.bottom = y+height;

	// ready to blt to surface
	lpdds->Blt(&dest_rect, // ptr to dest rectangle NULL为整个表面
			   NULL,       // ptr to source surface, NA            
			   NULL,       // ptr to source rectangle, NA
			   DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait                   
			   &ddbltfx);  // ptr to DDBLTFX structure

	// return success
	return(1);
} // end Color_Fill_Surface

//////将原表面传到目标表面的X,Y位置/////////////////////////////////////////////////////////

int DDraw_Draw_Surface(LPDIRECTDRAWSURFACE7 source, // 源表面
                       int x, int y,                 // 目标表面的X,Y
                       int width, int height,        // 源图和宽，和高
                       LPDIRECTDRAWSURFACE7 dest,    // 目标表面
                       int transparent )          // transparency flag
{
// draw a bob at the x,y defined in the BOB
// on the destination surface defined in dest

	RECT dest_rect,   // the destination rectangle
		 source_rect; // the source rectangle                             

	// fill in the destination rect
	dest_rect.left   = x;
	dest_rect.top    = y;
	dest_rect.right  = x+width;
	dest_rect.bottom = y+height;

	// fill in the source rect
	source_rect.left    = 0;
	source_rect.top     = 0;
	source_rect.right   = width;
	source_rect.bottom  = height;

	// test transparency flag

	if (transparent)        //带透明
	   {
	   // enable color key blit
	   // blt to destination surface
	   if (FAILED(dest->Blt(&dest_rect, // ptr to dest rectangle NULL为整个表面
							source,		// ptr to source surface, NA
						 &source_rect,	// ptr to source rectangle, NA
						 (DDBLT_WAIT | DDBLT_KEYSRC),// fill and wait 
						 NULL)))		// ptr to DDBLTFX structure
			   return(0);
	   } // end if
	else					//不透明
	   {
	   // perform blit without color key
	   // blt to destination surface
	   if (FAILED(dest->Blt(&dest_rect,
							source,
							&source_rect,
							(DDBLT_WAIT),
							NULL)))
			   return(0);

	   } // end if

	// return success
	return(1);

} // end DDraw_Draw_Surface

//从一个表面  blt到 另一个表面////////////////////////////////////////////////
int SurfacetoSurface(LPDIRECTDRAWSURFACE7 source, // 源表面
					   int sx, int sy,				// 源表面的X,Y
					   int swidth, int sheight,		// 源和宽，和高
                       int dx, int dy,                 // 目标表面的X,Y
                       int dwidth,  int dheight,        // 目标和宽，和高
                       LPDIRECTDRAWSURFACE7 dest,    // 目标表面
                       int transparent )          // transparency flag
{
// draw a bob at the x,y defined in the BOB
// on the destination surface defined in dest

	RECT dest_rect,   // the destination rectangle
		 source_rect; // the source rectangle                             

	// fill in the source rect
	source_rect.left    = sx;
	source_rect.top     = sy;
	source_rect.right   = sx+swidth;
	source_rect.bottom  = sy+sheight;

	// fill in the destination rect
	dest_rect.left   = dx;
	dest_rect.top    = dy;
	dest_rect.right  = dx+dwidth;
	dest_rect.bottom = dy+dheight;

	// test transparency flag

	if (transparent)        //带透明
	   {
	   // enable color key blit
	   // blt to destination surface
	   if (FAILED(dest->Blt(&dest_rect, // ptr to dest rectangle NULL为整个表面
							source,		// ptr to source surface, NA
						 &source_rect,	// ptr to source rectangle, NA
						 (DDBLT_WAIT | DDBLT_KEYSRC),// fill and wait 
						 NULL)))		// ptr to DDBLTFX structure
			   return(0);
	   } // end if
	else					//不透明
	   {
	   // perform blit without color key
	   // blt to destination surface
	   if (FAILED(dest->Blt(&dest_rect,
							source,
							&source_rect,
							(DDBLT_WAIT),
							NULL)))
			   return(0);

	   } // end if

	// return success
	return(1);

} // end SurfacetoSurface


//*******从bitmap结构中装载  各小位  图数据   到表面************************************/
int Scan_Image_Bitmap(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
                      LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
                      int cx, int cy)             // cell to scan image from
{
	UCHAR *source_ptr,   // working pointers
		  *dest_ptr;

	DDSURFACEDESC2 ddsd;  //  direct draw surface description 

	DDRAW_INIT_STRUCT(ddsd)

	// get the addr to destination surface memory

	// 对目标表面加锁
	lpdds->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL);

	// compute position to start scanning bits from
	cx = cx*(ddsd.dwWidth+1) + 1;	// 1代表边框的1个像素
	cy = cy*(ddsd.dwHeight+1) + 1;

	// extract bitmap data
	source_ptr = bitmap->buffer + (cy*(bitmap->bitmapinfoheader.biWidth) )+cx;

	// assign a pointer to the memory surface for manipulation
	dest_ptr = (UCHAR *)ddsd.lpSurface;

	// iterate thru each scanline and copy bitmap
	for (int index_y=0; index_y < (int)ddsd.dwHeight; index_y++)
    {
		// copy next line of data to destination
		memcpy(dest_ptr, source_ptr, ddsd.dwWidth);

		// advance pointers
		source_ptr += bitmap->bitmapinfoheader.biWidth;
		dest_ptr   += (ddsd.lPitch);
    } // end for index_y

	// 解锁 
	lpdds->Unlock(NULL);

	// return success
	return(1);

} // end Scan_Image_Bitmap

//*******从bitmap结构中装载 位图数据  到表面************************************/
int BitmaptoSurface(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
                      LPDIRECTDRAWSURFACE7 lpdds // surface to hold data
                      )         
{
	UCHAR *source_ptr,   // working pointers
		  *dest_ptr;

	DDSURFACEDESC2 ddsd;  //  direct draw surface description 

	DDRAW_INIT_STRUCT(ddsd)

	// 对目标表面加锁，得到目标表面的地址
	lpdds->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL);

	// 源指针指向位图数据首地址
	source_ptr = bitmap->buffer;

	// 目的指针指向目标表面
	dest_ptr = (UCHAR *)ddsd.lpSurface;

	// iterate thru each scanline and copy bitmap
	for (int index_y=0; index_y < (int)ddsd.dwHeight; index_y++)
    {
		// copy next line of data to destination
		memcpy(dest_ptr, source_ptr, ddsd.dwWidth);

		// advance pointers
		source_ptr += bitmap->bitmapinfoheader.biWidth;
		dest_ptr   += (ddsd.lPitch);
    } // end for index_y

	// 解锁 
	lpdds->Unlock(NULL);

	// return success
	return(1);

} // end Scan_Image_Bitmap

//*************建立任意表面（离屏表面）***************************
LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height, int mem_flags, int color_key = 0)
{
// this function creates an offscreen plain surface

	DDSURFACEDESC2 ddsd;         // working description
	LPDIRECTDRAWSURFACE7 lpdds;  // temporary surface
    
	// set to access caps, width, and height
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize  = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	// set dimensions of the new bitmap surface
	ddsd.dwWidth  =  width;		//表面宽(像素)
	ddsd.dwHeight =  height;	//表面高(像素)

	// set surface to offscreen plain
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;	//mem-flags表明在显存还是内存

	// create the surface
	if (FAILED(lpdd->CreateSurface(&ddsd,&lpdds,NULL)))
	   return(NULL);

	// test if user wants a color key
	if (color_key >= 0)
	   {
	   // set color key to color 0
	   DDCOLORKEY color_key; // used to set color key
	   color_key.dwColorSpaceLowValue  = 0;
	   color_key.dwColorSpaceHighValue = 0;

	   // now set the color key for source blitting
	   lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);
	   } // end if

	// return surface
	return(lpdds);
} // end DDraw_Create_Surface

int Draw_Text_GDI(char *text,int value, int x,int y,COLORREF color, LPDIRECTDRAWSURFACE7 lpdds)
{
	// this function draws the sent text on the sent surface 
	// using color index as the color in the palette

	HDC xdc; // the working dc

	// get the dc from surface
	if (FAILED(lpdds->GetDC(&xdc)))
	   return(0);

	// set the colors for the text up
	SetTextColor(xdc,color);

	// set background mode to transparent so black isn't copied
	SetBkMode(xdc, TRANSPARENT);

	// draw the text a
	TextOut(xdc,x,y,text,strlen(text));

	// release the dc
	lpdds->ReleaseDC(xdc);

	// return success
	return(1);
} // end Draw_Text_GDI

