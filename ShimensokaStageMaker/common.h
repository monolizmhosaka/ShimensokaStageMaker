#pragma once

#define DEF_CHIP_LAYER_GLASS 0
#define DEF_CHIP_LAYER_ITEM 1
#define DEF_MAX_CHIP_IMAGE 2

#define MAP_WIDTH 19
#define MAP_HEIGHT 19 
#define CHIP_WIDTH 32
#define CHIP_HEIGHT 32
#define LAYER_COUNT 2
#define BIT_COUNT 8

//設定ファイル
#define SETTING_FILE "config.xml"

//ファイルフォーマット　ネイティブ
#pragma pack(1)
typedef struct _FMF_HEADER_DATA_NATIVE
{
	unsigned char token[4];				//+0 identifier [U4] // ファイル識別子 'FMF_' (0x5F464D46)
	unsigned int data_size;				//+4 size  [U4] // ヘッダを除いたデータサイズ
	unsigned int map_width;				//+8 mapWidth [U4] // マップの横幅
	unsigned int map_height;			//+12 mapHeight [U4] // マップの縦幅
	unsigned char chip_width;			//+16 chipHeight [U1] // パーツの横幅
	unsigned char chip_height;			//+17 chipHeight [U1] // パーツの縦幅
	unsigned char layer_count;			//+18 layerCount [U1] // レイヤー数
	unsigned char bit_count;			//+19 bitCount [U1] // レイヤーデータのビットカウント(8/16)
} FmfHeaderDataNative;

typedef struct _FMF_HEADER_DATA
{
	FmfHeaderDataNative fhdn;
} FmfHeaderData;

//ファイルフォーマット　ネイティブ
typedef struct _FMS_HEADER_DATA
{
	FmfHeaderDataNative fhdn;

	//拡張
	unsigned int crown_score;		//+20 crown_score [U4] 王冠条件得点
	unsigned short crown_chain;		//+24 crown_hain  [U2] 王冠条件　チェイン数
	unsigned char crown_dead;		//+26 crown_dead  [U1] 王冠条件死亡回数
	unsigned short crown_time;		//+27 crown_time  [U2] 王冠条件経過時間
	unsigned short reserve1;		//+29 reserve1 [U2] 予約
	unsigned short reserve2;		//+31 reserve2  [U2] 予約
	unsigned int reserve3;			//+33 reserve3  [U4] 予約
	unsigned int reserve4;			//+37 reserve4  [U4] 予約
} FmsHeaderData;
#pragma pack()


//ＦＭＦデータ　マネージ用データクラス
ref class  FmfData
{
public :
	FmfData(FmfHeaderDataNative* fhdn, array<System::Byte>^ srcbody, String^ filePath)
	{
		token = gcnew array<unsigned char>{fhdn->token[0], fhdn->token[1], fhdn->token[2], fhdn->token[3], '\0'};
		data_size = fhdn->data_size;
		map_width = fhdn->map_width;
		map_height = fhdn->map_height;
		chip_width = fhdn->chip_width;
		chip_height = fhdn->chip_height;
		layer_count = fhdn->layer_count;
		bit_count = fhdn->bit_count;

		this->body = gcnew array<System::Byte>(srcbody->Length);
		Array::Copy(srcbody, this->body, srcbody->Length);

		path = filePath;
	};
	property String^ path;

	array<unsigned char>^ token;	//+0 identifier [U4] // ファイル識別子 'FMF_' (0x5F464D46)
	unsigned int data_size;			//+4 size  [U4] // ヘッダを除いたデータサイズ
	unsigned int map_width;			//+8 mapWidth [U4] // マップの横幅
	unsigned int map_height;		//+12 mapHeight [U4] // マップの縦幅
	unsigned char chip_width;		//+16 chipHeight [U1] // パーツの横幅
	unsigned char chip_height;		//+17 chipHeight [U1] // パーツの縦幅
	unsigned char layer_count;		//+18 layerCount [U1] // レイヤー数
	unsigned char bit_count;		//+19 bitCount [U1] // レイヤーデータのビットカウント(8/16)

	array<System::Byte>^ body;		//レイヤーデータ部
};



//ＦＭSデータ　マネージ用データクラス
ref class  FmsData : public FmfData
{
public :
	
	FmsData(FmfHeaderData* fhd, array<System::Byte>^ srcbody, String^ filePath) : FmfData(&fhd->fhdn, srcbody, filePath)
	{
		crown_score = 0;
		crown_chain = 0;
		crown_dead = 0;
		crown_time = 0;

		fmsFlag = false;
	};

	FmsData(FmsHeaderData* fshd, array<System::Byte>^ srcbody, String^ filePath) : FmfData(&fshd->fhdn, srcbody, filePath)
	{
		crown_score = fshd->crown_score;
		crown_chain = fshd->crown_chain;
		crown_dead = fshd->crown_dead;
		crown_time = fshd->crown_time;

		fmsFlag = true;
	};

	bool IsFms() { return fmsFlag; }

	//拡張
	unsigned int crown_score;		//+20 crown_score [U4] 王冠条件得点
	unsigned short crown_chain;		//+24 crown_hain  [U2] 王冠条件　チェイン数
	unsigned char crown_dead;		//+26 crown_dead  [U1] 王冠条件死亡回数
	unsigned short crown_time;		//+27 crown_time  [U2] 王冠条件経過時間

private :
	bool fmsFlag;


};


