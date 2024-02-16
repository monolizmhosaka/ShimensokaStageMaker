#pragma once

/**
 * アプリケーション設定情報保持クラス
 */
public ref class AppConfigData
{
public:

	AppConfigData(void){
		Initialize();
	}

	//コピーコンストラクタ
	AppConfigData(AppConfigData %obj)
	{
		stageFolderPath = obj.stageFolderPath;
		layer1ImagePath = obj.layer1ImagePath;
		layer2ImagePath = obj.layer2ImagePath;
	}
	System::String^ stageFolderPath;
	System::String^ layer1ImagePath;
	System::String^ layer2ImagePath;

	//変数を初期化する。
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
 * アプリケーション設定情報保持クラス
 */
ref class XmlConfigManager {
    private: 
		System::String^ configFile;

    public: 
		XmlConfigManager(System::String^ filename) {
		   this->configFile = filename;
		}

	//読み込まれた情報を指定されたPosConfigへ参照を渡します。
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

	//指定された情報を保存します。
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