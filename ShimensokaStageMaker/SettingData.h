#pragma once

/**
 * �A�v���P�[�V�����ݒ���ێ��N���X
 */
public ref class AppConfigData
{
public:

	AppConfigData(void){
		Initialize();
	}

	//�R�s�[�R���X�g���N�^
	AppConfigData(AppConfigData %obj)
	{
		stageFolderPath = obj.stageFolderPath;
		layer1ImagePath = obj.layer1ImagePath;
		layer2ImagePath = obj.layer2ImagePath;
	}
	System::String^ stageFolderPath;
	System::String^ layer1ImagePath;
	System::String^ layer2ImagePath;

	//�ϐ�������������B
	void Initialize()
	{
		stageFolderPath = gcnew String("");
		layer1ImagePath = gcnew String("");
		layer2ImagePath = gcnew String("");
	}
};

using namespace System::IO;
using namespace System::Xml::Serialization;
using namespace System::Runtime::InteropServices;
using namespace System::Diagnostics;

/**
 * �A�v���P�[�V�����ݒ���ێ��N���X
 */
ref class XmlConfigManager {
    private: 
		System::String^ configFile;

    public: 
		XmlConfigManager(System::String^ filename) {
		   this->configFile = filename;
		}

	//�ǂݍ��܂ꂽ�����w�肳�ꂽPosConfig�֎Q�Ƃ�n���܂��B
    public: 
		AppConfigData^ Load() 
		{
			FileStream^ fs = nullptr;
			AppConfigData^ ret = nullptr;

			try 
			{
				XmlSerializer^ serializer = gcnew XmlSerializer(AppConfigData::typeid);

				fs = gcnew FileStream(this->configFile, FileMode::Open);
				ret = (AppConfigData^)serializer->Deserialize(fs);
			} catch(Exception^ e)
			{
				Debug::WriteLine("%s", e->Message);
			} finally
			{
				if ( fs != nullptr )
				{
					fs->Close();
				}
			}
			return ret;
		}

	//�w�肳�ꂽ����ۑ����܂��B
    public: 
		bool Save(AppConfigData^ conf) 
		{
			FileStream^ fs = nullptr;
			bool ret = false;

			try 
			{
				XmlSerializer^ serializer = gcnew XmlSerializer(AppConfigData::typeid);

				fs = gcnew FileStream(this->configFile, FileMode::OpenOrCreate);
				serializer->Serialize(fs, conf);
				ret = true;
			} catch(Exception^ e)
			{
				Debug::WriteLine("%s", e->Message);
			} finally
			{
				if ( fs != nullptr )
				{
					fs->Close();
				}
			}
			return ret;
		}
};