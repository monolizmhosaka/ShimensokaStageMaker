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

//�ݒ�t�@�C��
#define SETTING_FILE "config.xml"

//�t�@�C���t�H�[�}�b�g�@�l�C�e�B�u
#pragma pack(1)
typedef struct _FMF_HEADER_DATA_NATIVE
{
	unsigned char token[4];				//+0 identifier [U4] // �t�@�C�����ʎq 'FMF_' (0x5F464D46)
	unsigned int data_size;				//+4 size  [U4] // �w�b�_���������f�[�^�T�C�Y
	unsigned int map_width;				//+8 mapWidth [U4] // �}�b�v�̉���
	unsigned int map_height;			//+12 mapHeight [U4] // �}�b�v�̏c��
	unsigned char chip_width;			//+16 chipHeight [U1] // �p�[�c�̉���
	unsigned char chip_height;			//+17 chipHeight [U1] // �p�[�c�̏c��
	unsigned char layer_count;			//+18 layerCount [U1] // ���C���[��
	unsigned char bit_count;			//+19 bitCount [U1] // ���C���[�f�[�^�̃r�b�g�J�E���g(8/16)
} FmfHeaderDataNative;

typedef struct _FMF_HEADER_DATA
{
	FmfHeaderDataNative fhdn;
} FmfHeaderData;

//�t�@�C���t�H�[�}�b�g�@�l�C�e�B�u
typedef struct _FMS_HEADER_DATA
{
	FmfHeaderDataNative fhdn;

	//�g��
	unsigned int crown_score;		//+20 crown_score [U4] �����������_
	unsigned short crown_chain;		//+24 crown_hain  [U2] ���������@�`�F�C����
	unsigned char crown_dead;		//+26 crown_dead  [U1] �����������S��
	unsigned short crown_time;		//+27 crown_time  [U2] ���������o�ߎ���
	unsigned short reserve1;		//+29 reserve1 [U2] �\��
	unsigned short reserve2;		//+31 reserve2  [U2] �\��
	unsigned int reserve3;			//+33 reserve3  [U4] �\��
	unsigned int reserve4;			//+37 reserve4  [U4] �\��
} FmsHeaderData;
#pragma pack()


//�e�l�e�f�[�^�@�}�l�[�W�p�f�[�^�N���X
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

	array<unsigned char>^ token;	//+0 identifier [U4] // �t�@�C�����ʎq 'FMF_' (0x5F464D46)
	unsigned int data_size;			//+4 size  [U4] // �w�b�_���������f�[�^�T�C�Y
	unsigned int map_width;			//+8 mapWidth [U4] // �}�b�v�̉���
	unsigned int map_height;		//+12 mapHeight [U4] // �}�b�v�̏c��
	unsigned char chip_width;		//+16 chipHeight [U1] // �p�[�c�̉���
	unsigned char chip_height;		//+17 chipHeight [U1] // �p�[�c�̏c��
	unsigned char layer_count;		//+18 layerCount [U1] // ���C���[��
	unsigned char bit_count;		//+19 bitCount [U1] // ���C���[�f�[�^�̃r�b�g�J�E���g(8/16)

	array<System::Byte>^ body;		//���C���[�f�[�^��
};



//�e�lS�f�[�^�@�}�l�[�W�p�f�[�^�N���X
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

	//�g��
	unsigned int crown_score;		//+20 crown_score [U4] �����������_
	unsigned short crown_chain;		//+24 crown_hain  [U2] ���������@�`�F�C����
	unsigned char crown_dead;		//+26 crown_dead  [U1] �����������S��
	unsigned short crown_time;		//+27 crown_time  [U2] ���������o�ߎ���

private :
	bool fmsFlag;


};


