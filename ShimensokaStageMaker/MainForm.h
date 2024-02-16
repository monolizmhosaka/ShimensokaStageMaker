#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vcclr.h>
#include <iostream>
#include <string.h>

#include "common.h"

#include "SettingForm.h"
#include "SettingData.h"

namespace ShimensokaStageMaker {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Diagnostics;
	using namespace System::Text;
	/// <summary>
	/// MainForm の概要
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	private :
		//表示物系を初期化
		void InitializeForm()
		{
			textBoxCrownScore->Text = "0";
			textBoxCrownChain->Text = "0";
			textBoxCrownTime->Text = "0";
			comboBoxCrownDead->SelectedIndex = -1;

			labelGlassTotal->Text = "";
			labelGlassItem1->Text = "";
			labelGlassItem2->Text = "";
			labelGlassItem3->Text = "";
			labelGlassItem4->Text = "";
			labelGlassItem5->Text = "";

			//最低点スコア設定
			labelInfoMinScore->Text = "";
			labelInfoMaxScore->Text = "";

			labelItemTotal->Text = "";
			labelItemItem1->Text = "";
			labelItemItem2->Text = "";
			labelItemItem3->Text = "";
			labelItemItem4->Text = "";
			labelItemItem5->Text = "";
			labelItemItem6->Text = "";
			labelItemItem7->Text = "";
			labelItemItem8->Text = "";
			labelItemItem9->Text = "";
		}

		//ヘッダ情報を読み込む
		template <class X> bool loadFmxFile(String^ path, FmsData^% pfmsData)
		{
			bool ret = false;

			//対象データの読み込み
			//ヘッダから
			FileStream^ fs = nullptr;
			try
			{
				fs = gcnew FileStream(path, FileMode::Open);
				BinaryReader^ br = gcnew BinaryReader(fs);

				array<unsigned char>^ data = br->ReadBytes( sizeof(X));

				//読取バイト数のチェック（念のため
				if ( data->Length == sizeof(X) )
				{
					X fhd;

					pin_ptr<unsigned char> pin = &data[0];
					memcpy(&fhd, &pin[0], sizeof(X));

					//エラーチェック
					if ( fhd.fhdn.map_width == MAP_WIDTH && fhd.fhdn.map_height == MAP_HEIGHT && 
							fhd.fhdn.chip_width == CHIP_WIDTH && fhd.fhdn.chip_height == CHIP_HEIGHT &&
								fhd.fhdn.layer_count >= LAYER_COUNT && fhd.fhdn.bit_count == BIT_COUNT )
					{//条件に当てはまらない場合はエラー
						array<System::Byte>^ body = br->ReadBytes(fhd.fhdn.data_size);
						pfmsData = gcnew FmsData(&fhd, body, path);

						ret = true;
					} else 
					{
						logTextBox->AppendTextLine("データ内容が不正でした。");
						logTextBox->AppendTextLine("＝＝＝＝＝＝＝＝＝＝＝＝＝");
						logTextBox->AppendTextLine("マップ幅＝" + fhd.fhdn.map_width.ToString());
						logTextBox->AppendTextLine("マップ高さ＝" + fhd.fhdn.map_height.ToString());
						logTextBox->AppendTextLine("チップ幅＝" + fhd.fhdn.chip_width.ToString());
						logTextBox->AppendTextLine("チップ高さ＝" + fhd.fhdn.chip_height.ToString());
						logTextBox->AppendTextLine("レイヤー数＝" + fhd.fhdn.layer_count.ToString());
						logTextBox->AppendTextLine("ビット数＝" + fhd.fhdn.bit_count.ToString());
						logTextBox->AppendTextLine("＝＝＝＝＝＝＝＝＝＝＝＝＝");
					}
				} else
				{
					logTextBox->AppendTextLine("ステージデータ[" + path + "]の読み込みに失敗しました。");
				}
			} catch (Exception^ e)
			{
				logTextBox->AppendTextLine("ステージデータ[" + path + "]の読み込みに失敗しました。");
				if (dynamic_cast<FileNotFoundException^>(e))
					Debug::WriteLine("File '{0}' not found", path);
				else
					Debug::WriteLine("Exception: ({0})", e);
			} finally
			{
				if ( fs != nullptr ) fs->Close( );
			}


			return ret;
		}


		//チップ画像を読み込む
		bool loadLayerImage(String^ path, int layer_index)
		{
			bool ret = false;

			//レイヤー画像2を読み込み
			if ( path != "" )
			{
				try 
				{
					cellBackImageLayers[layer_index] = gcnew Bitmap(path);
					Color tColor = Color::FromArgb(255, 0, 255);
					cellBackImageLayers[layer_index]->MakeTransparent(tColor);

					logTextBox->AppendTextLine("レイヤー画像[" + path + "]を読み込みました。");

					ret = true;
				} catch (Exception^ e)
				{
					logTextBox->AppendTextLine("レイヤー画像[" + path + "]の読み込みに失敗しました。");
					if (dynamic_cast<FileNotFoundException^>(e))
						Debug::WriteLine("File '{0}' not found", path);
					else
						Debug::WriteLine("Exception: ({0})", e);
				}
			}

			return ret;
		}

		//ファイルのトークンを返す
		String^ getFileHeaderToken(String^ path)
		{
			String^ ret = nullptr;

			//対象データの読み込み
			//ヘッダから
			FileStream^ fs = nullptr;
			try
			{
				fs = gcnew FileStream(path, FileMode::Open);
				BinaryReader^ br = gcnew BinaryReader(fs);

				//ＦＭＦかＦＭＳを判定するためトークンを取得
				array<unsigned char>^ tokenData = br->ReadBytes(4);
				//読取バイト数のチェック（念のため
				if ( tokenData->Length == 4 )
				{
					ret = Encoding::ASCII->GetString(tokenData, 0, tokenData->Length);
				}
			} catch (Exception^ e)
			{
				if (dynamic_cast<FileNotFoundException^>(e))
					Debug::WriteLine("File '{0}' not found", path);
				else
					Debug::WriteLine("Exception: ({0})", e);
			} finally
			{
				if ( fs != nullptr ) fs->Close( );
			}

			return ret;
		}

		//指定されたフォルダからＦＭＦ、ＢＹＴＥＳファイルを検索してリストに設定する。
		void SetListView(String^ path)
		{
			//.fmf or .fms をサーチ。　
			array<String^>^ file;
			try {
				file = Directory::GetFiles(path, L"*.fmf");
			}
			catch(Exception^)
			{
				logTextBox->AppendTextLine("ステージフォルダ[" + path + "]が見つかりませんでした。");
				return;
			}
			Array::Sort(file);
			Debug::WriteLine("--== Files inside '{0}' ==--", path);

			this->listView1->Items->Clear();
			for (int i=0; i<file->Length; i++)
			{
				//ファイル名抽出しリストに追加
				array<String^>^ splitpath = file[i]->Split(gcnew cli::array<Char >(1) {'\\'});
				System::Windows::Forms::ListViewItem^  listViewItem = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(2) {splitpath[splitpath->Length-1], L"dummy"}, -1));
				this->listView1->Items->Add(listViewItem);

				Debug::WriteLine(file[i]);
				Debug::WriteLine(splitpath[splitpath->Length-1]);
			}

			array<String^>^ file2 = Directory::GetFiles( path, L"*.bytes" );
			Array::Sort(file2);
			for (int i=0; i<file2->Length; i++)
			{
				//ファイル名抽出しリストに追加
				array<String^>^ splitpath = file2[i]->Split(gcnew cli::array<Char >(1) {'\\'});
				System::Windows::Forms::ListViewItem^  listViewItem = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(2) {splitpath[splitpath->Length-1], L"dummy"}, -1));
				this->listView1->Items->Add(listViewItem);

				Debug::WriteLine(file2[i]);
				Debug::WriteLine(splitpath[splitpath->Length-1]);
			}
		}

	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクター コードを追加します
			//
			//
			currentFmsData = nullptr;
			conf = nullptr;

			{//ログ出力ウィンドウを作成
				this->logTextBox = (gcnew LogTextBox(this->textBox1));
				this->Controls->Add(this->logTextBox);
			}

			{//設定ファイルを読み込み
				String^ settingFile = System::IO::Directory::GetCurrentDirectory() + "\\" + gcnew String(SETTING_FILE);
				XmlConfigManager^ xml = gcnew XmlConfigManager(settingFile);
				conf = xml->Load();
				if ( conf == nullptr )
				{//失敗した場合は空の設定メモリを確保
					logTextBox->AppendTextLine("設定ファイル[" + settingFile + "]の読み込みに失敗しました。");

					//自前で確保して初期化
					this->conf = gcnew AppConfigData();
				} else
				{
					logTextBox->AppendTextLine("設定ファイル[" + settingFile + "]を読み込みました。");
				}
			}

			if ( conf->stageFolderPath != "" )
			{//ステージフォルダが有効ならばリストを初期化する。
				//指定されたフォルダからファイル名一覧を取得

				SetListView(conf->stageFolderPath);
			}

			//レイヤー画像1を読み込み
			//チップ画像配列を作成
			//マネージクラスはランタイムがメモリ管理しているので
			//通常の宣言はできない。よってこうする。
			cellBackImageLayers = gcnew array<Bitmap^>(DEF_MAX_CHIP_IMAGE);
			cellBackImageLayers[DEF_CHIP_LAYER_GLASS] = nullptr;
			cellBackImageLayers[DEF_CHIP_LAYER_ITEM] = nullptr;

			if ( loadLayerImage(conf->layer1ImagePath, DEF_CHIP_LAYER_GLASS) == true )
			{//ロード成功したらピクチャボックスに画像を表示するのさ。
				//アイテム画像を設定
				array<PictureBox^>^ pics = gcnew array<PictureBox^>(5)
				{
					pictureBoxGlassItem1, 
					pictureBoxGlassItem2, 
					pictureBoxGlassItem3, 
					pictureBoxGlassItem4, 
					pictureBoxGlassItem5, 
				};
				for ( int i=0; i<pics->Length; i++)
				{
					Rectangle rec = Rectangle(i*32,0,32,32);

					Bitmap^ bmp = gcnew Bitmap(32, 32);
					Graphics^ g = Graphics::FromImage(bmp);
					g->DrawImage(cellBackImageLayers[DEF_CHIP_LAYER_GLASS], 0, 0, rec, GraphicsUnit::Pixel);

					pics[i]->Image = bmp;
				}
			}

			if ( loadLayerImage(conf->layer2ImagePath, DEF_CHIP_LAYER_ITEM) == true )
			{//ロード成功したらピクチャボックスに画像を表示するのさ。
				//アイテム画像を設定
				array<PictureBox^>^ pics = gcnew array<PictureBox^>(9)
				{
					pictureBoxItemItem1, 
					pictureBoxItemItem2, 
					pictureBoxItemItem3, 
					pictureBoxItemItem4, 
					pictureBoxItemItem5, 
					pictureBoxItemItem6, 
					pictureBoxItemItem7, 
					pictureBoxItemItem8, 
					pictureBoxItemItem9 
				};
				for ( int i=0; i<pics->Length; i++)
				{
					Rectangle rec = Rectangle(i*32,0,32,32);

					Bitmap^ bmp = gcnew Bitmap(32, 32);
					Graphics^ g = Graphics::FromImage(bmp);
					g->DrawImage(cellBackImageLayers[DEF_CHIP_LAYER_ITEM], 0, 0, rec, GraphicsUnit::Pixel);

					pics[i]->Image = bmp;
				}
			}


			//デフォルトで１９ｘ１９のマス目を作ってしまおう
			for ( int i=0; i<MAP_WIDTH; i++)
			{
				dataGridView1->Rows->Add(L"");
				dataGridView1->Rows[i]->HeaderCell->Value = (i+1).ToString();
			}

			//フォームコントロールの初期化
			InitializeForm();
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: FmsData^ currentFmsData;
	private: AppConfigData^ conf;
	private: array<Bitmap^>^ cellBackImageLayers;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: LogTextBox^  logTextBox;
	private: System::Windows::Forms::ListView^  listView1;
	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::ColumnHeader^  columnHeader2;
	private: System::Windows::Forms::DataGridView^  dataGridView1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column2;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column3;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column4;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column5;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column6;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column7;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column8;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column9;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column10;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column11;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column12;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column13;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column14;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column15;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column16;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column17;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column18;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column19;

	private: System::Windows::Forms::ToolStrip^  toolStrip1;
	private: System::Windows::Forms::ToolStripDropDownButton^  toolStripDropDownButton1;
private: System::Windows::Forms::ToolStripMenuItem^  開くToolStripMenuItem;

private: System::Windows::Forms::ToolStripMenuItem^  再読み込みToolStripMenuItem;

	private: System::Windows::Forms::ToolStripSeparator^  bbbbToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  終了ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripDropDownButton^  toolStripDropDownButton2;







	private: System::Windows::Forms::ToolStripMenuItem^  環境設定ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripDropDownButton^  toolStripDropDownButton3;
	private: System::Windows::Forms::ToolStripMenuItem^  ステージ一覧をサムネイルToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ステージ一覧をテキスト表示ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem3;
	private: System::Windows::Forms::ToolStripMenuItem^  ログ出力の表示ToolStripMenuItem;
private: System::Windows::Forms::ToolStripButton^  toolStripHelpButton;

private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::ComboBox^  comboBoxCrownDead;

private: System::Windows::Forms::TextBox^  textBoxCrownTime;

private: System::Windows::Forms::TextBox^  textBoxCrownChain;


private: System::Windows::Forms::TextBox^  textBoxCrownScore;

private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::TabPage^  tabPage3;
private: System::Windows::Forms::Label^  labelGlassItem3;
private: System::Windows::Forms::Label^  labelGlassItem4;
private: System::Windows::Forms::Label^  labelGlassItem5;




private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::PictureBox^  pictureBoxGlassItem5;


private: System::Windows::Forms::PictureBox^  pictureBoxGlassItem4;

private: System::Windows::Forms::PictureBox^  pictureBoxGlassItem3;

private: System::Windows::Forms::PictureBox^  pictureBoxGlassItem2;

private: System::Windows::Forms::PictureBox^  pictureBoxGlassItem1;

private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::TabPage^  tabPage4;
private: System::Windows::Forms::Button^  SaveBtn;

private: System::Windows::Forms::Label^  labelInfoMinScore;

private: System::Windows::Forms::Label^  labelInfoMaxScore;


private: System::Windows::Forms::Label^  label31;
private: System::Windows::Forms::Label^  label30;
private: System::Windows::Forms::Label^  labelGlassTotal;
private: System::Windows::Forms::Label^  labelGlassItem1;
private: System::Windows::Forms::Label^  labelGlassItem2;
private: System::Windows::Forms::Label^  labelItemItem7;




private: System::Windows::Forms::Label^  labelItemItem3;

private: System::Windows::Forms::PictureBox^  pictureBoxItemItem4;
private: System::Windows::Forms::PictureBox^  pictureBoxItemItem6;
private: System::Windows::Forms::Label^  labelItemTotal;



private: System::Windows::Forms::Label^  labelItemItem1;
private: System::Windows::Forms::Label^  labelItemItem6;


private: System::Windows::Forms::Label^  labelItemItem2;
private: System::Windows::Forms::Label^  labelItemItem4;
private: System::Windows::Forms::Label^  labelItemItem5;




private: System::Windows::Forms::PictureBox^  pictureBoxItemItem5;

private: System::Windows::Forms::PictureBox^  pictureBoxItemItem3;
private: System::Windows::Forms::PictureBox^  pictureBoxItemItem7;


private: System::Windows::Forms::PictureBox^  pictureBoxItemItem2;

private: System::Windows::Forms::PictureBox^  pictureBoxItemItem1;

private: System::Windows::Forms::Label^  label27;
private: System::Windows::Forms::Button^  AutoCalcBtn;
private: System::Windows::Forms::Label^  labelItemItem8;
private: System::Windows::Forms::PictureBox^  pictureBoxItemItem8;
private: System::Windows::Forms::Label^  labelItemItem9;
private: System::Windows::Forms::PictureBox^  pictureBoxItemItem9;


	private: System::ComponentModel::IContainer^  components;































































	protected: 

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle3 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->listView1 = (gcnew System::Windows::Forms::ListView());
			this->columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->columnHeader2 = (gcnew System::Windows::Forms::ColumnHeader());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->Column1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column3 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column4 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column5 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column6 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column7 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column8 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column9 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column10 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column11 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column12 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column13 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column14 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column15 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column16 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column17 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column18 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column19 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->toolStripDropDownButton1 = (gcnew System::Windows::Forms::ToolStripDropDownButton());
			this->開くToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->再読み込みToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->bbbbToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->終了ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripDropDownButton2 = (gcnew System::Windows::Forms::ToolStripDropDownButton());
			this->環境設定ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripDropDownButton3 = (gcnew System::Windows::Forms::ToolStripDropDownButton());
			this->ステージ一覧をサムネイルToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ステージ一覧をテキスト表示ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->ログ出力の表示ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripHelpButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->AutoCalcBtn = (gcnew System::Windows::Forms::Button());
			this->SaveBtn = (gcnew System::Windows::Forms::Button());
			this->comboBoxCrownDead = (gcnew System::Windows::Forms::ComboBox());
			this->textBoxCrownTime = (gcnew System::Windows::Forms::TextBox());
			this->textBoxCrownChain = (gcnew System::Windows::Forms::TextBox());
			this->textBoxCrownScore = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->labelInfoMinScore = (gcnew System::Windows::Forms::Label());
			this->labelInfoMaxScore = (gcnew System::Windows::Forms::Label());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->labelGlassTotal = (gcnew System::Windows::Forms::Label());
			this->labelGlassItem1 = (gcnew System::Windows::Forms::Label());
			this->labelGlassItem2 = (gcnew System::Windows::Forms::Label());
			this->labelGlassItem3 = (gcnew System::Windows::Forms::Label());
			this->labelGlassItem4 = (gcnew System::Windows::Forms::Label());
			this->labelGlassItem5 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->pictureBoxGlassItem5 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxGlassItem4 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxGlassItem3 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxGlassItem2 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxGlassItem1 = (gcnew System::Windows::Forms::PictureBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->labelItemItem8 = (gcnew System::Windows::Forms::Label());
			this->pictureBoxItemItem8 = (gcnew System::Windows::Forms::PictureBox());
			this->labelItemItem7 = (gcnew System::Windows::Forms::Label());
			this->labelItemItem3 = (gcnew System::Windows::Forms::Label());
			this->pictureBoxItemItem4 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxItemItem6 = (gcnew System::Windows::Forms::PictureBox());
			this->labelItemTotal = (gcnew System::Windows::Forms::Label());
			this->labelItemItem1 = (gcnew System::Windows::Forms::Label());
			this->labelItemItem6 = (gcnew System::Windows::Forms::Label());
			this->labelItemItem2 = (gcnew System::Windows::Forms::Label());
			this->labelItemItem4 = (gcnew System::Windows::Forms::Label());
			this->labelItemItem5 = (gcnew System::Windows::Forms::Label());
			this->pictureBoxItemItem5 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxItemItem3 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxItemItem7 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxItemItem2 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBoxItemItem1 = (gcnew System::Windows::Forms::PictureBox());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->labelItemItem9 = (gcnew System::Windows::Forms::Label());
			this->pictureBoxItemItem9 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
			this->toolStrip1->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->tabPage3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem5))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem1))->BeginInit();
			this->tabPage4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem8))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem6))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem5))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem7))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem9))->BeginInit();
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox1->Location = System::Drawing::Point(12, 622);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->textBox1->Size = System::Drawing::Size(714, 126);
			this->textBox1->TabIndex = 0;
			this->textBox1->Visible = false;
			this->textBox1->WordWrap = false;
			// 
			// listView1
			// 
			this->listView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->listView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->columnHeader1, this->columnHeader2});
			this->listView1->Location = System::Drawing::Point(578, 28);
			this->listView1->Name = L"listView1";
			this->listView1->Size = System::Drawing::Size(148, 589);
			this->listView1->TabIndex = 1;
			this->listView1->UseCompatibleStateImageBehavior = false;
			this->listView1->View = System::Windows::Forms::View::Details;
			this->listView1->ItemActivate += gcnew System::EventHandler(this, &MainForm::listView1_ItemActivate);
			this->listView1->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::listView1_SelectedIndexChanged);
			// 
			// columnHeader1
			// 
			this->columnHeader1->Text = L"ファイル名";
			this->columnHeader1->Width = 99;
			// 
			// columnHeader2
			// 
			this->columnHeader2->Text = L"ヘッダ状態";
			this->columnHeader2->Width = 70;
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToResizeColumns = false;
			this->dataGridView1->AllowUserToResizeRows = false;
			this->dataGridView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle1->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			dataGridViewCellStyle1->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle1->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->dataGridView1->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(19) {this->Column1, 
				this->Column2, this->Column3, this->Column4, this->Column5, this->Column6, this->Column7, this->Column8, this->Column9, this->Column10, 
				this->Column11, this->Column12, this->Column13, this->Column14, this->Column15, this->Column16, this->Column17, this->Column18, 
				this->Column19});
			dataGridViewCellStyle2->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle2->BackColor = System::Drawing::SystemColors::Window;
			dataGridViewCellStyle2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			dataGridViewCellStyle2->ForeColor = System::Drawing::SystemColors::ControlText;
			dataGridViewCellStyle2->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle2->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle2->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
			this->dataGridView1->DefaultCellStyle = dataGridViewCellStyle2;
			this->dataGridView1->Location = System::Drawing::Point(12, 28);
			this->dataGridView1->Name = L"dataGridView1";
			dataGridViewCellStyle3->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle3->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle3->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			dataGridViewCellStyle3->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle3->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle3->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle3->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->dataGridView1->RowHeadersDefaultCellStyle = dataGridViewCellStyle3;
			this->dataGridView1->RowTemplate->Height = 21;
			this->dataGridView1->Size = System::Drawing::Size(556, 430);
			this->dataGridView1->TabIndex = 2;
			this->dataGridView1->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &MainForm::dataGridView1_CellContentClick);
			this->dataGridView1->CellPainting += gcnew System::Windows::Forms::DataGridViewCellPaintingEventHandler(this, &MainForm::dataGridView1_CellPainting);
			// 
			// Column1
			// 
			this->Column1->HeaderText = L"1";
			this->Column1->Name = L"Column1";
			this->Column1->Width = 24;
			// 
			// Column2
			// 
			this->Column2->HeaderText = L"2";
			this->Column2->Name = L"Column2";
			this->Column2->Width = 24;
			// 
			// Column3
			// 
			this->Column3->HeaderText = L"3";
			this->Column3->Name = L"Column3";
			this->Column3->Width = 24;
			// 
			// Column4
			// 
			this->Column4->HeaderText = L"4";
			this->Column4->Name = L"Column4";
			this->Column4->Width = 24;
			// 
			// Column5
			// 
			this->Column5->HeaderText = L"5";
			this->Column5->Name = L"Column5";
			this->Column5->Width = 24;
			// 
			// Column6
			// 
			this->Column6->HeaderText = L"6";
			this->Column6->Name = L"Column6";
			this->Column6->Width = 24;
			// 
			// Column7
			// 
			this->Column7->HeaderText = L"7";
			this->Column7->Name = L"Column7";
			this->Column7->Width = 24;
			// 
			// Column8
			// 
			this->Column8->HeaderText = L"8";
			this->Column8->Name = L"Column8";
			this->Column8->Width = 24;
			// 
			// Column9
			// 
			this->Column9->HeaderText = L"9";
			this->Column9->Name = L"Column9";
			this->Column9->Width = 24;
			// 
			// Column10
			// 
			this->Column10->HeaderText = L"10";
			this->Column10->Name = L"Column10";
			this->Column10->Width = 24;
			// 
			// Column11
			// 
			this->Column11->HeaderText = L"11";
			this->Column11->Name = L"Column11";
			this->Column11->Width = 24;
			// 
			// Column12
			// 
			this->Column12->HeaderText = L"12";
			this->Column12->Name = L"Column12";
			this->Column12->Width = 24;
			// 
			// Column13
			// 
			this->Column13->HeaderText = L"13";
			this->Column13->Name = L"Column13";
			this->Column13->Width = 24;
			// 
			// Column14
			// 
			this->Column14->HeaderText = L"14";
			this->Column14->Name = L"Column14";
			this->Column14->Width = 24;
			// 
			// Column15
			// 
			this->Column15->HeaderText = L"15";
			this->Column15->Name = L"Column15";
			this->Column15->Width = 24;
			// 
			// Column16
			// 
			this->Column16->HeaderText = L"16";
			this->Column16->Name = L"Column16";
			this->Column16->Width = 24;
			// 
			// Column17
			// 
			this->Column17->HeaderText = L"17";
			this->Column17->Name = L"Column17";
			this->Column17->Width = 24;
			// 
			// Column18
			// 
			this->Column18->HeaderText = L"18";
			this->Column18->Name = L"Column18";
			this->Column18->Width = 24;
			// 
			// Column19
			// 
			this->Column19->HeaderText = L"19";
			this->Column19->Name = L"Column19";
			this->Column19->Width = 24;
			// 
			// toolStrip1
			// 
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->toolStripDropDownButton1, 
				this->toolStripDropDownButton2, this->toolStripDropDownButton3, this->toolStripHelpButton});
			this->toolStrip1->Location = System::Drawing::Point(0, 0);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(738, 25);
			this->toolStrip1->TabIndex = 4;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// toolStripDropDownButton1
			// 
			this->toolStripDropDownButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->toolStripDropDownButton1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->開くToolStripMenuItem, 
				this->再読み込みToolStripMenuItem, this->bbbbToolStripMenuItem, this->終了ToolStripMenuItem});
			this->toolStripDropDownButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripDropDownButton1.Image")));
			this->toolStripDropDownButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripDropDownButton1->Name = L"toolStripDropDownButton1";
			this->toolStripDropDownButton1->Size = System::Drawing::Size(54, 22);
			this->toolStripDropDownButton1->Text = L"ファイル";
			this->toolStripDropDownButton1->ToolTipText = L"ファイル";
			this->toolStripDropDownButton1->Click += gcnew System::EventHandler(this, &MainForm::toolStripDropDownButton1_Click);
			// 
			// 開くToolStripMenuItem
			// 
			this->開くToolStripMenuItem->Name = L"開くToolStripMenuItem";
			this->開くToolStripMenuItem->Size = System::Drawing::Size(132, 22);
			this->開くToolStripMenuItem->Text = L"開く";
			this->開くToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::開くToolStripMenuItem_Click);
			// 
			// 再読み込みToolStripMenuItem
			// 
			this->再読み込みToolStripMenuItem->Name = L"再読み込みToolStripMenuItem";
			this->再読み込みToolStripMenuItem->Size = System::Drawing::Size(132, 22);
			this->再読み込みToolStripMenuItem->Text = L"再読み込み";
			this->再読み込みToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::再読み込みToolStripMenuItem_Click);
			// 
			// bbbbToolStripMenuItem
			// 
			this->bbbbToolStripMenuItem->Name = L"bbbbToolStripMenuItem";
			this->bbbbToolStripMenuItem->Size = System::Drawing::Size(129, 6);
			// 
			// 終了ToolStripMenuItem
			// 
			this->終了ToolStripMenuItem->Name = L"終了ToolStripMenuItem";
			this->終了ToolStripMenuItem->Size = System::Drawing::Size(132, 22);
			this->終了ToolStripMenuItem->Text = L"終了";
			this->終了ToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::終了ToolStripMenuItem_Click);
			// 
			// toolStripDropDownButton2
			// 
			this->toolStripDropDownButton2->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->toolStripDropDownButton2->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->環境設定ToolStripMenuItem});
			this->toolStripDropDownButton2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripDropDownButton2.Image")));
			this->toolStripDropDownButton2->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripDropDownButton2->Name = L"toolStripDropDownButton2";
			this->toolStripDropDownButton2->Size = System::Drawing::Size(44, 22);
			this->toolStripDropDownButton2->Text = L"編集";
			// 
			// 環境設定ToolStripMenuItem
			// 
			this->環境設定ToolStripMenuItem->Name = L"環境設定ToolStripMenuItem";
			this->環境設定ToolStripMenuItem->Size = System::Drawing::Size(122, 22);
			this->環境設定ToolStripMenuItem->Text = L"環境設定";
			this->環境設定ToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::環境設定ToolStripMenuItem_Click);
			// 
			// toolStripDropDownButton3
			// 
			this->toolStripDropDownButton3->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->toolStripDropDownButton3->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->ステージ一覧をサムネイルToolStripMenuItem, 
				this->ステージ一覧をテキスト表示ToolStripMenuItem, this->toolStripMenuItem3, this->ログ出力の表示ToolStripMenuItem});
			this->toolStripDropDownButton3->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripDropDownButton3.Image")));
			this->toolStripDropDownButton3->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripDropDownButton3->Name = L"toolStripDropDownButton3";
			this->toolStripDropDownButton3->Size = System::Drawing::Size(44, 22);
			this->toolStripDropDownButton3->Text = L"表示";
			// 
			// ステージ一覧をサムネイルToolStripMenuItem
			// 
			this->ステージ一覧をサムネイルToolStripMenuItem->Enabled = false;
			this->ステージ一覧をサムネイルToolStripMenuItem->Name = L"ステージ一覧をサムネイルToolStripMenuItem";
			this->ステージ一覧をサムネイルToolStripMenuItem->Size = System::Drawing::Size(205, 22);
			this->ステージ一覧をサムネイルToolStripMenuItem->Text = L"ステージ一覧をサムネイル";
			this->ステージ一覧をサムネイルToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::ステージ一覧をサムネイルToolStripMenuItem_Click);
			// 
			// ステージ一覧をテキスト表示ToolStripMenuItem
			// 
			this->ステージ一覧をテキスト表示ToolStripMenuItem->Checked = true;
			this->ステージ一覧をテキスト表示ToolStripMenuItem->CheckOnClick = true;
			this->ステージ一覧をテキスト表示ToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->ステージ一覧をテキスト表示ToolStripMenuItem->Name = L"ステージ一覧をテキスト表示ToolStripMenuItem";
			this->ステージ一覧をテキスト表示ToolStripMenuItem->Size = System::Drawing::Size(205, 22);
			this->ステージ一覧をテキスト表示ToolStripMenuItem->Text = L"ステージ一覧をテキスト表示";
			// 
			// toolStripMenuItem3
			// 
			this->toolStripMenuItem3->Name = L"toolStripMenuItem3";
			this->toolStripMenuItem3->Size = System::Drawing::Size(202, 6);
			// 
			// ログ出力の表示ToolStripMenuItem
			// 
			this->ログ出力の表示ToolStripMenuItem->CheckOnClick = true;
			this->ログ出力の表示ToolStripMenuItem->Name = L"ログ出力の表示ToolStripMenuItem";
			this->ログ出力の表示ToolStripMenuItem->Size = System::Drawing::Size(205, 22);
			this->ログ出力の表示ToolStripMenuItem->Text = L"ログ出力の表示";
			this->ログ出力の表示ToolStripMenuItem->CheckedChanged += gcnew System::EventHandler(this, &MainForm::ログ出力の表示ToolStripMenuItem_CheckedChanged);
			this->ログ出力の表示ToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::ログ出力の表示ToolStripMenuItem_Click);
			// 
			// toolStripHelpButton
			// 
			this->toolStripHelpButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->toolStripHelpButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripHelpButton.Image")));
			this->toolStripHelpButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripHelpButton->Name = L"toolStripHelpButton";
			this->toolStripHelpButton->Size = System::Drawing::Size(40, 22);
			this->toolStripHelpButton->Text = L"ヘルプ";
			this->toolStripHelpButton->Click += gcnew System::EventHandler(this, &MainForm::toolStripHelpBtn_Click);
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Controls->Add(this->tabPage4);
			this->tabControl1->Location = System::Drawing::Point(12, 464);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(560, 153);
			this->tabControl1->TabIndex = 5;
			this->tabControl1->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::tabControl1_SelectedIndexChanged);
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->AutoCalcBtn);
			this->tabPage1->Controls->Add(this->SaveBtn);
			this->tabPage1->Controls->Add(this->comboBoxCrownDead);
			this->tabPage1->Controls->Add(this->textBoxCrownTime);
			this->tabPage1->Controls->Add(this->textBoxCrownChain);
			this->tabPage1->Controls->Add(this->textBoxCrownScore);
			this->tabPage1->Controls->Add(this->label4);
			this->tabPage1->Controls->Add(this->label3);
			this->tabPage1->Controls->Add(this->label2);
			this->tabPage1->Controls->Add(this->label1);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(552, 127);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"王冠条件";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// AutoCalcBtn
			// 
			this->AutoCalcBtn->Location = System::Drawing::Point(421, 13);
			this->AutoCalcBtn->Name = L"AutoCalcBtn";
			this->AutoCalcBtn->Size = System::Drawing::Size(121, 23);
			this->AutoCalcBtn->TabIndex = 10;
			this->AutoCalcBtn->Text = L"自動算出";
			this->AutoCalcBtn->UseVisualStyleBackColor = true;
			this->AutoCalcBtn->Click += gcnew System::EventHandler(this, &MainForm::AutoCalcBtn_Click);
			// 
			// SaveBtn
			// 
			this->SaveBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->SaveBtn->Location = System::Drawing::Point(421, 98);
			this->SaveBtn->Name = L"SaveBtn";
			this->SaveBtn->Size = System::Drawing::Size(125, 23);
			this->SaveBtn->TabIndex = 9;
			this->SaveBtn->Text = L"保存";
			this->SaveBtn->UseVisualStyleBackColor = true;
			this->SaveBtn->Click += gcnew System::EventHandler(this, &MainForm::SaveBtn_Click);
			// 
			// comboBoxCrownDead
			// 
			this->comboBoxCrownDead->FormattingEnabled = true;
			this->comboBoxCrownDead->Items->AddRange(gcnew cli::array< System::Object^  >(10) {L"0", L"1", L"2", L"3", L"4", L"5", L"6", 
				L"7", L"8", L"9"});
			this->comboBoxCrownDead->Location = System::Drawing::Point(306, 10);
			this->comboBoxCrownDead->Name = L"comboBoxCrownDead";
			this->comboBoxCrownDead->Size = System::Drawing::Size(43, 20);
			this->comboBoxCrownDead->TabIndex = 8;
			// 
			// textBoxCrownTime
			// 
			this->textBoxCrownTime->Location = System::Drawing::Point(90, 60);
			this->textBoxCrownTime->Name = L"textBoxCrownTime";
			this->textBoxCrownTime->Size = System::Drawing::Size(100, 19);
			this->textBoxCrownTime->TabIndex = 7;
			// 
			// textBoxCrownChain
			// 
			this->textBoxCrownChain->Location = System::Drawing::Point(90, 35);
			this->textBoxCrownChain->Name = L"textBoxCrownChain";
			this->textBoxCrownChain->Size = System::Drawing::Size(100, 19);
			this->textBoxCrownChain->TabIndex = 5;
			// 
			// textBoxCrownScore
			// 
			this->textBoxCrownScore->Location = System::Drawing::Point(90, 10);
			this->textBoxCrownScore->Name = L"textBoxCrownScore";
			this->textBoxCrownScore->Size = System::Drawing::Size(100, 19);
			this->textBoxCrownScore->TabIndex = 4;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->BackColor = System::Drawing::Color::Black;
			this->label4->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label4->ForeColor = System::Drawing::Color::White;
			this->label4->Location = System::Drawing::Point(9, 38);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(76, 15);
			this->label4->TabIndex = 3;
			this->label4->Text = L" チェイン数";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->BackColor = System::Drawing::Color::Black;
			this->label3->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label3->ForeColor = System::Drawing::Color::White;
			this->label3->Location = System::Drawing::Point(223, 13);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(77, 15);
			this->label3->TabIndex = 2;
			this->label3->Text = L" 死亡回数";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->BackColor = System::Drawing::Color::Black;
			this->label2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label2->ForeColor = System::Drawing::Color::White;
			this->label2->Location = System::Drawing::Point(8, 62);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(77, 15);
			this->label2->TabIndex = 1;
			this->label2->Text = L" 経過時間";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::Black;
			this->label1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label1->ForeColor = System::Drawing::Color::White;
			this->label1->Location = System::Drawing::Point(9, 13);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(75, 15);
			this->label1->TabIndex = 0;
			this->label1->Text = L"      得点";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->labelInfoMinScore);
			this->tabPage2->Controls->Add(this->labelInfoMaxScore);
			this->tabPage2->Controls->Add(this->label31);
			this->tabPage2->Controls->Add(this->label30);
			this->tabPage2->Controls->Add(this->label7);
			this->tabPage2->Controls->Add(this->label5);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(552, 127);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"構成情報";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// labelInfoMinScore
			// 
			this->labelInfoMinScore->AutoSize = true;
			this->labelInfoMinScore->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelInfoMinScore->Location = System::Drawing::Point(90, 47);
			this->labelInfoMinScore->Name = L"labelInfoMinScore";
			this->labelInfoMinScore->Size = System::Drawing::Size(86, 19);
			this->labelInfoMinScore->TabIndex = 21;
			this->labelInfoMinScore->Text = L"9999999";
			// 
			// labelInfoMaxScore
			// 
			this->labelInfoMaxScore->AutoSize = true;
			this->labelInfoMaxScore->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelInfoMaxScore->Location = System::Drawing::Point(90, 10);
			this->labelInfoMaxScore->Name = L"labelInfoMaxScore";
			this->labelInfoMaxScore->Size = System::Drawing::Size(86, 19);
			this->labelInfoMaxScore->TabIndex = 20;
			this->labelInfoMaxScore->Text = L"9999999";
			// 
			// label31
			// 
			this->label31->AutoSize = true;
			this->label31->BackColor = System::Drawing::Color::Black;
			this->label31->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label31->ForeColor = System::Drawing::Color::White;
			this->label31->Location = System::Drawing::Point(13, 50);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(71, 15);
			this->label31->TabIndex = 5;
			this->label31->Text = L"最低得点";
			// 
			// label30
			// 
			this->label30->AutoSize = true;
			this->label30->BackColor = System::Drawing::Color::Black;
			this->label30->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label30->ForeColor = System::Drawing::Color::White;
			this->label30->Location = System::Drawing::Point(13, 13);
			this->label30->Name = L"label30";
			this->label30->Size = System::Drawing::Size(71, 15);
			this->label30->TabIndex = 4;
			this->label30->Text = L"最高得点";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(18, 65);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(0, 12);
			this->label7->TabIndex = 3;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(206, 27);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(0, 12);
			this->label5->TabIndex = 0;
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->labelGlassTotal);
			this->tabPage3->Controls->Add(this->labelGlassItem1);
			this->tabPage3->Controls->Add(this->labelGlassItem2);
			this->tabPage3->Controls->Add(this->labelGlassItem3);
			this->tabPage3->Controls->Add(this->labelGlassItem4);
			this->tabPage3->Controls->Add(this->labelGlassItem5);
			this->tabPage3->Controls->Add(this->label14);
			this->tabPage3->Controls->Add(this->label13);
			this->tabPage3->Controls->Add(this->label12);
			this->tabPage3->Controls->Add(this->label11);
			this->tabPage3->Controls->Add(this->label10);
			this->tabPage3->Controls->Add(this->label9);
			this->tabPage3->Controls->Add(this->pictureBoxGlassItem5);
			this->tabPage3->Controls->Add(this->pictureBoxGlassItem4);
			this->tabPage3->Controls->Add(this->pictureBoxGlassItem3);
			this->tabPage3->Controls->Add(this->pictureBoxGlassItem2);
			this->tabPage3->Controls->Add(this->pictureBoxGlassItem1);
			this->tabPage3->Controls->Add(this->label8);
			this->tabPage3->Controls->Add(this->label6);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Size = System::Drawing::Size(552, 127);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = L"ガラス情報";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// labelGlassTotal
			// 
			this->labelGlassTotal->AutoSize = true;
			this->labelGlassTotal->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Underline)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
			this->labelGlassTotal->ForeColor = System::Drawing::Color::Red;
			this->labelGlassTotal->Location = System::Drawing::Point(94, 10);
			this->labelGlassTotal->Name = L"labelGlassTotal";
			this->labelGlassTotal->Size = System::Drawing::Size(34, 15);
			this->labelGlassTotal->TabIndex = 21;
			this->labelGlassTotal->Text = L"999";
			// 
			// labelGlassItem1
			// 
			this->labelGlassItem1->AutoSize = true;
			this->labelGlassItem1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelGlassItem1->Location = System::Drawing::Point(54, 63);
			this->labelGlassItem1->Name = L"labelGlassItem1";
			this->labelGlassItem1->Size = System::Drawing::Size(42, 19);
			this->labelGlassItem1->TabIndex = 20;
			this->labelGlassItem1->Text = L"999";
			// 
			// labelGlassItem2
			// 
			this->labelGlassItem2->AutoSize = true;
			this->labelGlassItem2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelGlassItem2->Location = System::Drawing::Point(145, 63);
			this->labelGlassItem2->Name = L"labelGlassItem2";
			this->labelGlassItem2->Size = System::Drawing::Size(42, 19);
			this->labelGlassItem2->TabIndex = 19;
			this->labelGlassItem2->Text = L"999";
			// 
			// labelGlassItem3
			// 
			this->labelGlassItem3->AutoSize = true;
			this->labelGlassItem3->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelGlassItem3->Location = System::Drawing::Point(243, 64);
			this->labelGlassItem3->Name = L"labelGlassItem3";
			this->labelGlassItem3->Size = System::Drawing::Size(42, 19);
			this->labelGlassItem3->TabIndex = 18;
			this->labelGlassItem3->Text = L"999";
			// 
			// labelGlassItem4
			// 
			this->labelGlassItem4->AutoSize = true;
			this->labelGlassItem4->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelGlassItem4->Location = System::Drawing::Point(345, 63);
			this->labelGlassItem4->Name = L"labelGlassItem4";
			this->labelGlassItem4->Size = System::Drawing::Size(42, 19);
			this->labelGlassItem4->TabIndex = 17;
			this->labelGlassItem4->Text = L"999";
			this->labelGlassItem4->Click += gcnew System::EventHandler(this, &MainForm::label16_Click);
			// 
			// labelGlassItem5
			// 
			this->labelGlassItem5->AutoSize = true;
			this->labelGlassItem5->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelGlassItem5->Location = System::Drawing::Point(443, 63);
			this->labelGlassItem5->Name = L"labelGlassItem5";
			this->labelGlassItem5->Size = System::Drawing::Size(42, 19);
			this->labelGlassItem5->TabIndex = 16;
			this->labelGlassItem5->Text = L"999";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label14->Location = System::Drawing::Point(145, 70);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(0, 12);
			this->label14->TabIndex = 14;
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label13->Location = System::Drawing::Point(235, 70);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(0, 12);
			this->label13->TabIndex = 13;
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label12->Location = System::Drawing::Point(327, 71);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(0, 12);
			this->label12->TabIndex = 12;
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label11->Location = System::Drawing::Point(443, 66);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(0, 12);
			this->label11->TabIndex = 11;
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label10->Location = System::Drawing::Point(245, 45);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(0, 12);
			this->label10->TabIndex = 10;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label9->Location = System::Drawing::Point(55, 70);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(0, 12);
			this->label9->TabIndex = 9;
			// 
			// pictureBoxGlassItem5
			// 
			this->pictureBoxGlassItem5->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxGlassItem5->Location = System::Drawing::Point(405, 52);
			this->pictureBoxGlassItem5->Name = L"pictureBoxGlassItem5";
			this->pictureBoxGlassItem5->Size = System::Drawing::Size(32, 32);
			this->pictureBoxGlassItem5->TabIndex = 7;
			this->pictureBoxGlassItem5->TabStop = false;
			// 
			// pictureBoxGlassItem4
			// 
			this->pictureBoxGlassItem4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxGlassItem4->Location = System::Drawing::Point(307, 52);
			this->pictureBoxGlassItem4->Name = L"pictureBoxGlassItem4";
			this->pictureBoxGlassItem4->Size = System::Drawing::Size(32, 32);
			this->pictureBoxGlassItem4->TabIndex = 6;
			this->pictureBoxGlassItem4->TabStop = false;
			// 
			// pictureBoxGlassItem3
			// 
			this->pictureBoxGlassItem3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxGlassItem3->Location = System::Drawing::Point(207, 52);
			this->pictureBoxGlassItem3->Name = L"pictureBoxGlassItem3";
			this->pictureBoxGlassItem3->Size = System::Drawing::Size(32, 32);
			this->pictureBoxGlassItem3->TabIndex = 5;
			this->pictureBoxGlassItem3->TabStop = false;
			// 
			// pictureBoxGlassItem2
			// 
			this->pictureBoxGlassItem2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxGlassItem2->Location = System::Drawing::Point(107, 52);
			this->pictureBoxGlassItem2->Name = L"pictureBoxGlassItem2";
			this->pictureBoxGlassItem2->Size = System::Drawing::Size(32, 32);
			this->pictureBoxGlassItem2->TabIndex = 4;
			this->pictureBoxGlassItem2->TabStop = false;
			// 
			// pictureBoxGlassItem1
			// 
			this->pictureBoxGlassItem1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxGlassItem1->Location = System::Drawing::Point(17, 52);
			this->pictureBoxGlassItem1->Name = L"pictureBoxGlassItem1";
			this->pictureBoxGlassItem1->Size = System::Drawing::Size(32, 32);
			this->pictureBoxGlassItem1->TabIndex = 3;
			this->pictureBoxGlassItem1->TabStop = false;
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(12, 35);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(0, 12);
			this->label8->TabIndex = 2;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->BackColor = System::Drawing::Color::Black;
			this->label6->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label6->ForeColor = System::Drawing::Color::White;
			this->label6->Location = System::Drawing::Point(23, 9);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(65, 15);
			this->label6->TabIndex = 1;
			this->label6->Text = L" ガラス数";
			// 
			// tabPage4
			// 
			this->tabPage4->Controls->Add(this->labelItemItem9);
			this->tabPage4->Controls->Add(this->pictureBoxItemItem9);
			this->tabPage4->Controls->Add(this->labelItemItem8);
			this->tabPage4->Controls->Add(this->pictureBoxItemItem8);
			this->tabPage4->Controls->Add(this->labelItemItem7);
			this->tabPage4->Controls->Add(this->labelItemItem3);
			this->tabPage4->Controls->Add(this->pictureBoxItemItem4);
			this->tabPage4->Controls->Add(this->pictureBoxItemItem6);
			this->tabPage4->Controls->Add(this->labelItemTotal);
			this->tabPage4->Controls->Add(this->labelItemItem1);
			this->tabPage4->Controls->Add(this->labelItemItem6);
			this->tabPage4->Controls->Add(this->labelItemItem2);
			this->tabPage4->Controls->Add(this->labelItemItem4);
			this->tabPage4->Controls->Add(this->labelItemItem5);
			this->tabPage4->Controls->Add(this->pictureBoxItemItem5);
			this->tabPage4->Controls->Add(this->pictureBoxItemItem3);
			this->tabPage4->Controls->Add(this->pictureBoxItemItem7);
			this->tabPage4->Controls->Add(this->pictureBoxItemItem2);
			this->tabPage4->Controls->Add(this->pictureBoxItemItem1);
			this->tabPage4->Controls->Add(this->label27);
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Size = System::Drawing::Size(552, 127);
			this->tabPage4->TabIndex = 3;
			this->tabPage4->Text = L"アイテム情報";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// labelItemItem8
			// 
			this->labelItemItem8->AutoSize = true;
			this->labelItemItem8->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelItemItem8->Location = System::Drawing::Point(239, 72);
			this->labelItemItem8->Name = L"labelItemItem8";
			this->labelItemItem8->Size = System::Drawing::Size(42, 19);
			this->labelItemItem8->TabIndex = 39;
			this->labelItemItem8->Text = L"999";
			// 
			// pictureBoxItemItem8
			// 
			this->pictureBoxItemItem8->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxItemItem8->Location = System::Drawing::Point(209, 67);
			this->pictureBoxItemItem8->Name = L"pictureBoxItemItem8";
			this->pictureBoxItemItem8->Size = System::Drawing::Size(24, 24);
			this->pictureBoxItemItem8->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxItemItem8->TabIndex = 38;
			this->pictureBoxItemItem8->TabStop = false;
			// 
			// labelItemItem7
			// 
			this->labelItemItem7->AutoSize = true;
			this->labelItemItem7->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelItemItem7->Location = System::Drawing::Point(144, 72);
			this->labelItemItem7->Name = L"labelItemItem7";
			this->labelItemItem7->Size = System::Drawing::Size(42, 19);
			this->labelItemItem7->TabIndex = 37;
			this->labelItemItem7->Text = L"999";
			// 
			// labelItemItem3
			// 
			this->labelItemItem3->AutoSize = true;
			this->labelItemItem3->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelItemItem3->Location = System::Drawing::Point(239, 42);
			this->labelItemItem3->Name = L"labelItemItem3";
			this->labelItemItem3->Size = System::Drawing::Size(42, 19);
			this->labelItemItem3->TabIndex = 36;
			this->labelItemItem3->Text = L"999";
			// 
			// pictureBoxItemItem4
			// 
			this->pictureBoxItemItem4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxItemItem4->Location = System::Drawing::Point(309, 37);
			this->pictureBoxItemItem4->Name = L"pictureBoxItemItem4";
			this->pictureBoxItemItem4->Size = System::Drawing::Size(24, 24);
			this->pictureBoxItemItem4->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxItemItem4->TabIndex = 35;
			this->pictureBoxItemItem4->TabStop = false;
			// 
			// pictureBoxItemItem6
			// 
			this->pictureBoxItemItem6->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxItemItem6->Location = System::Drawing::Point(19, 67);
			this->pictureBoxItemItem6->Name = L"pictureBoxItemItem6";
			this->pictureBoxItemItem6->Size = System::Drawing::Size(24, 24);
			this->pictureBoxItemItem6->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxItemItem6->TabIndex = 34;
			this->pictureBoxItemItem6->TabStop = false;
			// 
			// labelItemTotal
			// 
			this->labelItemTotal->AutoSize = true;
			this->labelItemTotal->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Underline)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
			this->labelItemTotal->ForeColor = System::Drawing::Color::Red;
			this->labelItemTotal->Location = System::Drawing::Point(94, 10);
			this->labelItemTotal->Name = L"labelItemTotal";
			this->labelItemTotal->Size = System::Drawing::Size(34, 15);
			this->labelItemTotal->TabIndex = 33;
			this->labelItemTotal->Text = L"999";
			// 
			// labelItemItem1
			// 
			this->labelItemItem1->AutoSize = true;
			this->labelItemItem1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelItemItem1->Location = System::Drawing::Point(49, 42);
			this->labelItemItem1->Name = L"labelItemItem1";
			this->labelItemItem1->Size = System::Drawing::Size(42, 19);
			this->labelItemItem1->TabIndex = 32;
			this->labelItemItem1->Text = L"999";
			// 
			// labelItemItem6
			// 
			this->labelItemItem6->AutoSize = true;
			this->labelItemItem6->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelItemItem6->Location = System::Drawing::Point(49, 72);
			this->labelItemItem6->Name = L"labelItemItem6";
			this->labelItemItem6->Size = System::Drawing::Size(42, 19);
			this->labelItemItem6->TabIndex = 31;
			this->labelItemItem6->Text = L"999";
			// 
			// labelItemItem2
			// 
			this->labelItemItem2->AutoSize = true;
			this->labelItemItem2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelItemItem2->Location = System::Drawing::Point(144, 42);
			this->labelItemItem2->Name = L"labelItemItem2";
			this->labelItemItem2->Size = System::Drawing::Size(42, 19);
			this->labelItemItem2->TabIndex = 30;
			this->labelItemItem2->Text = L"999";
			// 
			// labelItemItem4
			// 
			this->labelItemItem4->AutoSize = true;
			this->labelItemItem4->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelItemItem4->Location = System::Drawing::Point(339, 42);
			this->labelItemItem4->Name = L"labelItemItem4";
			this->labelItemItem4->Size = System::Drawing::Size(42, 19);
			this->labelItemItem4->TabIndex = 29;
			this->labelItemItem4->Text = L"999";
			// 
			// labelItemItem5
			// 
			this->labelItemItem5->AutoSize = true;
			this->labelItemItem5->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelItemItem5->Location = System::Drawing::Point(437, 42);
			this->labelItemItem5->Name = L"labelItemItem5";
			this->labelItemItem5->Size = System::Drawing::Size(42, 19);
			this->labelItemItem5->TabIndex = 28;
			this->labelItemItem5->Text = L"999";
			// 
			// pictureBoxItemItem5
			// 
			this->pictureBoxItemItem5->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxItemItem5->Location = System::Drawing::Point(407, 37);
			this->pictureBoxItemItem5->Name = L"pictureBoxItemItem5";
			this->pictureBoxItemItem5->Size = System::Drawing::Size(24, 24);
			this->pictureBoxItemItem5->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxItemItem5->TabIndex = 27;
			this->pictureBoxItemItem5->TabStop = false;
			// 
			// pictureBoxItemItem3
			// 
			this->pictureBoxItemItem3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxItemItem3->Location = System::Drawing::Point(209, 37);
			this->pictureBoxItemItem3->Name = L"pictureBoxItemItem3";
			this->pictureBoxItemItem3->Size = System::Drawing::Size(24, 24);
			this->pictureBoxItemItem3->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxItemItem3->TabIndex = 26;
			this->pictureBoxItemItem3->TabStop = false;
			// 
			// pictureBoxItemItem7
			// 
			this->pictureBoxItemItem7->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxItemItem7->Location = System::Drawing::Point(114, 67);
			this->pictureBoxItemItem7->Name = L"pictureBoxItemItem7";
			this->pictureBoxItemItem7->Size = System::Drawing::Size(24, 24);
			this->pictureBoxItemItem7->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxItemItem7->TabIndex = 25;
			this->pictureBoxItemItem7->TabStop = false;
			// 
			// pictureBoxItemItem2
			// 
			this->pictureBoxItemItem2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxItemItem2->Location = System::Drawing::Point(114, 37);
			this->pictureBoxItemItem2->Name = L"pictureBoxItemItem2";
			this->pictureBoxItemItem2->Size = System::Drawing::Size(24, 24);
			this->pictureBoxItemItem2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxItemItem2->TabIndex = 24;
			this->pictureBoxItemItem2->TabStop = false;
			this->pictureBoxItemItem2->Click += gcnew System::EventHandler(this, &MainForm::pictureBox9_Click);
			// 
			// pictureBoxItemItem1
			// 
			this->pictureBoxItemItem1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxItemItem1->Location = System::Drawing::Point(19, 37);
			this->pictureBoxItemItem1->Name = L"pictureBoxItemItem1";
			this->pictureBoxItemItem1->Size = System::Drawing::Size(24, 24);
			this->pictureBoxItemItem1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxItemItem1->TabIndex = 23;
			this->pictureBoxItemItem1->TabStop = false;
			// 
			// label27
			// 
			this->label27->AutoSize = true;
			this->label27->BackColor = System::Drawing::Color::Black;
			this->label27->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label27->ForeColor = System::Drawing::Color::White;
			this->label27->Location = System::Drawing::Point(16, 9);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(72, 15);
			this->label27->TabIndex = 22;
			this->label27->Text = L"アイテム数";
			// 
			// labelItemItem9
			// 
			this->labelItemItem9->AutoSize = true;
			this->labelItemItem9->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->labelItemItem9->Location = System::Drawing::Point(339, 72);
			this->labelItemItem9->Name = L"labelItemItem9";
			this->labelItemItem9->Size = System::Drawing::Size(42, 19);
			this->labelItemItem9->TabIndex = 41;
			this->labelItemItem9->Text = L"999";
			// 
			// pictureBoxItemItem9
			// 
			this->pictureBoxItemItem9->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBoxItemItem9->Location = System::Drawing::Point(309, 67);
			this->pictureBoxItemItem9->Name = L"pictureBoxItemItem9";
			this->pictureBoxItemItem9->Size = System::Drawing::Size(24, 24);
			this->pictureBoxItemItem9->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxItemItem9->TabIndex = 40;
			this->pictureBoxItemItem9->TabStop = false;
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(738, 627);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->toolStrip1);
			this->Controls->Add(this->dataGridView1);
			this->Controls->Add(this->listView1);
			this->Controls->Add(this->textBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"MainForm";
			this->Text = L"めんづくり";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->tabPage3->ResumeLayout(false);
			this->tabPage3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem5))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxGlassItem1))->EndInit();
			this->tabPage4->ResumeLayout(false);
			this->tabPage4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem8))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem6))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem5))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem7))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxItemItem9))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void dataGridView1_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
			 }
private: System::Void dataGridView1_CellPainting(System::Object^ sender, System::Windows::Forms::DataGridViewCellPaintingEventArgs^  e) {

			//データが読まれていない場合は何もしない
			if ( currentFmsData == nullptr ) return;

			//読まれているデータがＦＭＦだったら読み込む
			String^ token = Encoding::ASCII->GetString(currentFmsData->token, 0, currentFmsData->token->Length);
			if ( token->Equals(gcnew String("FMF_\0")) == false && token->Equals(gcnew String("FMS_\0")) == false )
			{
				return;
			}

			//ヘッダー以外のセルで、背景を描画する時のみ実行させる。
			if (e->ColumnIndex >= 0 && e->RowIndex >= 0 && e->ColumnIndex < 19 && e->RowIndex < 19 && (e->PaintParts & System::Windows::Forms::DataGridViewPaintParts::Background) == System::Windows::Forms::DataGridViewPaintParts::Background)
			{
				//背景だけを描画する
				DataGridViewPaintParts backParts = e->PaintParts & (DataGridViewPaintParts::Background | DataGridViewPaintParts::SelectionBackground);
				e->Paint(e->ClipBounds, backParts);

				// Create rectangle for source image.
				Rectangle srcRect = Rectangle(0, 0, 32, 32);
				GraphicsUnit units = GraphicsUnit::Pixel;
	
				//レイヤー１の方のチップ番号を取得し描画
				if ( cellBackImageLayers[DEF_CHIP_LAYER_GLASS] != nullptr )
				{
					System::Byte ly1 = currentFmsData->body[e->RowIndex*currentFmsData->map_width + e->ColumnIndex];
					//画像をセルの範囲いっぱいに表示する
					int chip_w_count = (cellBackImageLayers[DEF_CHIP_LAYER_GLASS]->Width/currentFmsData->chip_width);
					e->Graphics->DrawImage(cellBackImageLayers[DEF_CHIP_LAYER_GLASS], e->CellBounds, 
							(ly1 - (ly1/chip_w_count*chip_w_count))*currentFmsData->chip_width, 
								ly1/chip_w_count*currentFmsData->chip_width, currentFmsData->chip_width, currentFmsData->chip_height, units);
				} else
				{
					logTextBox->AppendTextLine("ガラスチップ画像の読み込まれていません。");

				}
				
				//レイヤー２の方のチップ番号を取得し描画
				if ( cellBackImageLayers[DEF_CHIP_LAYER_ITEM] != nullptr )
				{
					System::Byte ly2 = currentFmsData->body[currentFmsData->map_height*currentFmsData->map_width*currentFmsData->bit_count/8 + e->RowIndex*currentFmsData->map_width + e->ColumnIndex];
					int chip_w_count2 = (cellBackImageLayers[DEF_CHIP_LAYER_ITEM]->Width/currentFmsData->chip_width);
					e->Graphics->DrawImage(cellBackImageLayers[DEF_CHIP_LAYER_ITEM], e->CellBounds, 
							(ly2 - (ly2/chip_w_count2*chip_w_count2))*currentFmsData->chip_width, 
								ly2/chip_w_count2*currentFmsData->chip_width, currentFmsData->chip_width, currentFmsData->chip_height, units);
				} else
				{
					logTextBox->AppendTextLine("アイテムチップ画像の読み込まれていません。");
				}
				
				//背景以外が描画されるようにする
				DataGridViewPaintParts paintParts = e->PaintParts & ~backParts;
				//セルを描画する
				e->Paint(e->ClipBounds, paintParts);

				e->Handled = true;
			}
		 }
private: System::Void listView1_ItemActivate(System::Object^  sender, System::EventArgs^  e) {
			System::Windows::Forms::ListView^ listView = (System::Windows::Forms::ListView^)sender;

			String^ tmpPath = conf->stageFolderPath + "\\";
			String^ path = tmpPath + listView->FocusedItem->Text;
			Debug::WriteLine(path);

			//ヘッダ情報のトークンを取得
			String^ token =	getFileHeaderToken(path);
			bool success = false;
			if ( token == nullptr )
			{
				logTextBox->AppendTextLine("ヘッダ情報の取得に失敗しました。");
			} else if ( token == "FMF_" )
			{
				success = loadFmxFile<FmfHeaderData>(path,currentFmsData);
			} else if ( token == "FMS_" )
			{
				success = loadFmxFile<FmsHeaderData>(path,currentFmsData);
			}

			if ( success == false )
			{//失敗した場合はデータを初期化する。
				currentFmsData = nullptr;
				
				//DataGridViewを再描画
				textBoxCrownScore->Text = "0";
				textBoxCrownChain->Text = "0";
				textBoxCrownTime->Text = "0";
				comboBoxCrownDead->SelectedIndex = -1;

				dataGridView1->Refresh();
				return;
			}

			//取得した情報をセットする。
			textBoxCrownScore->Text = currentFmsData->crown_score.ToString();
			textBoxCrownChain->Text = currentFmsData->crown_chain.ToString();
			textBoxCrownTime->Text = currentFmsData->crown_time.ToString();
			comboBoxCrownDead->SelectedIndex = currentFmsData->crown_dead;

			//ガラス情報を取得し、情報をセットする。
			int glass_values[16] = {0};
			int glass_total = 0;
			int min_glass_score = 0;
			for ( unsigned int i=0; i<currentFmsData->data_size/2; i++)
			{
				if ( currentFmsData->body[i] < 255 ) 
				{
						glass_values[currentFmsData->body[i]%16]++;
						glass_total++;

						//スコア加算
						if ( currentFmsData->body[i]%16 == 1 ) min_glass_score += 100;
						else if ( currentFmsData->body[i]%16 == 2 ) min_glass_score += 500;
						else if ( currentFmsData->body[i]%16 == 3 ) min_glass_score += 3000;
				}
			}
			labelGlassTotal->Text = glass_total.ToString();
			labelGlassItem1->Text = glass_values[0].ToString();
			labelGlassItem2->Text = glass_values[1].ToString();
			labelGlassItem3->Text = glass_values[2].ToString();
			labelGlassItem4->Text = glass_values[3].ToString();
			labelGlassItem5->Text = glass_values[4].ToString();

			//最低点スコア設定
			labelInfoMinScore->Text = min_glass_score.ToString("N")->Replace(".00", "");
			labelInfoMaxScore->Text = (min_glass_score*5).ToString("N")->Replace(".00", "");

			//アイテム情報を取得し、情報をセットする。
			pin_ptr<System::Byte> b_ptr = &currentFmsData->body[currentFmsData->data_size/2];
			int item_values[16] = {0};
			int item_total = 0;
			for ( unsigned int i=0; i<currentFmsData->data_size/2; i++)
			{
				if ( b_ptr[i] < 255 ) 
				{
						item_values[b_ptr[i]%16]++;
						item_total++;
				}
			}
			labelItemTotal->Text = item_total.ToString();
			labelItemItem1->Text = item_values[0].ToString();
			labelItemItem2->Text = item_values[1].ToString();
			labelItemItem3->Text = item_values[2].ToString();
			labelItemItem4->Text = item_values[3].ToString();
			labelItemItem5->Text = item_values[4].ToString();
			labelItemItem6->Text = item_values[5].ToString();
			labelItemItem7->Text = item_values[6].ToString();
			labelItemItem8->Text = item_values[7].ToString();
			labelItemItem9->Text = item_values[8].ToString();


			//DataGridViewを再描画
			dataGridView1->Refresh();
		}
private: System::Void listView1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void toolStripDropDownButton1_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void 環境設定ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			SettingForm^ setForm = gcnew SettingForm(this->conf);
			AppConfigData^ tmpConf = gcnew AppConfigData(*this->conf);

			setForm->Owner = this;
			setForm->ShowDialog(this);

			//パラメータを比較して画像をロード
			if ( tmpConf->layer1ImagePath != conf->layer1ImagePath )
			{
				if ( loadLayerImage(conf->layer1ImagePath, DEF_CHIP_LAYER_GLASS) == true )
				{//ロード成功したらピクチャボックスに画像を表示するのさ。
					//アイテム画像を設定
					array<PictureBox^>^ pics = gcnew array<PictureBox^>(5)
					{
						pictureBoxGlassItem1, 
						pictureBoxGlassItem2, 
						pictureBoxGlassItem3, 
						pictureBoxGlassItem4, 
						pictureBoxGlassItem5, 
					};
					for ( int i=0; i<pics->Length; i++)
					{
						Rectangle rec = Rectangle(i*32,0,32,32);

						Bitmap^ bmp = gcnew Bitmap(32, 32);
						Graphics^ g = Graphics::FromImage(bmp);
						g->DrawImage(cellBackImageLayers[DEF_CHIP_LAYER_GLASS], 0, 0, rec, GraphicsUnit::Pixel);

						pics[i]->Image = bmp;
					}
				}

			}

			if ( tmpConf->layer2ImagePath != conf->layer2ImagePath )
			{
				if ( loadLayerImage(conf->layer2ImagePath, DEF_CHIP_LAYER_ITEM) == true )
				{//ロード成功したらピクチャボックスに画像を表示するのさ。
					//アイテム画像を設定
					array<PictureBox^>^ pics = gcnew array<PictureBox^>(9)
					{
						pictureBoxItemItem1, 
						pictureBoxItemItem2, 
						pictureBoxItemItem3, 
						pictureBoxItemItem4, 
						pictureBoxItemItem5, 
						pictureBoxItemItem6, 
						pictureBoxItemItem7, 
						pictureBoxItemItem8, 
						pictureBoxItemItem9 
					};
					for ( int i=0; i<pics->Length; i++)
					{
						Rectangle rec = Rectangle(i*32,0,32,32);

						Bitmap^ bmp = gcnew Bitmap(32, 32);
						Graphics^ g = Graphics::FromImage(bmp);
						g->DrawImage(cellBackImageLayers[DEF_CHIP_LAYER_ITEM], 0, 0, rec, GraphicsUnit::Pixel);

						pics[i]->Image = bmp;
					}
				}

			}

		}
private: System::Void tabControl1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label16_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void pictureBox9_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void 終了ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
private: System::Void ログ出力の表示ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

		 }
private: System::Void ログ出力の表示ToolStripMenuItem_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 ToolStripMenuItem^ tsmi = (ToolStripMenuItem^)sender;

			 if ( tsmi->Checked == true )
			 {
				 logTextBox->Visible = true;
				 this->Height += logTextBox->Height;
			 } else
			 {
				 logTextBox->Visible = false;
				 this->Height -= logTextBox->Height;
			 }
		 }
private: System::Void ステージ一覧をサムネイルToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void 再読み込みToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			if ( conf->stageFolderPath != "" )
			{//ステージフォルダが有効ならばリストを初期化する。

				//指定されたフォルダからファイル名一覧を取得	
				SetListView(conf->stageFolderPath);

				//表示物と内部を初期化する。
				currentFmsData = nullptr;
				InitializeForm();

				dataGridView1->Refresh();
			}
		 }
private: System::Void 開くToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

				// フォルダ選択ダイアログの表示
				String^ folderName = nullptr;

				//デフォルトの指定があったらそれを設定
				if ( conf->stageFolderPath != "" )
				{
					folderBrowserDialog1->SelectedPath = conf->stageFolderPath;
				}

				if ( folderBrowserDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
				{
					folderName = folderBrowserDialog1->SelectedPath;	//選択されたフォルダを設定
					Debug::WriteLine("%s", folderName);

					////指定されたフォルダからファイル名一覧を取得
					SetListView(folderName);

					currentFmsData = nullptr;
				
					InitializeForm();

					dataGridView1->Refresh();

					//保存する。
					conf->stageFolderPath = folderBrowserDialog1->SelectedPath;
					//ファイルに書き込む
					String^ settingFile = System::IO::Directory::GetCurrentDirectory() +"\\" + gcnew String(SETTING_FILE);
					XmlConfigManager^ xml = gcnew XmlConfigManager(settingFile);
					if ( xml->Save(conf) == false )
					{
						logTextBox->AppendTextLine("設定ファイル[" + settingFile + "]への書き込みに失敗しました。");
					}

				} else
				{//キャンセル時は何もしない。
				}
		 }
private: System::Void SaveBtn_Click(System::Object^  sender, System::EventArgs^  e) {

			 if ( currentFmsData == nullptr ) return;

			double dNullable;
			bool errFlag = false;
#define CHECK_NUMERIC(X) System::Double::TryParse(X,\
											System::Globalization::NumberStyles::Any,\
											nullptr,\
											dNullable\
											)
			
			if ( CHECK_NUMERIC(textBoxCrownScore->Text) == false ) 
			{
				logTextBox->AppendTextLine("エラーです。王冠条件の得点が数値ではありません。");
				errFlag = true;
			}
			if ( CHECK_NUMERIC(textBoxCrownChain->Text) == false ) 
			{
				logTextBox->AppendTextLine("エラーです。王冠条件のチェイン数が数値ではありません。");
				errFlag = true;
			}
			if ( CHECK_NUMERIC(textBoxCrownTime->Text) == false ) 
			{
				logTextBox->AppendTextLine("エラーです。王冠条件の経過時間が数値ではありません。");
				errFlag = true;
			}
			if ( errFlag == true ) return;

			//FMSファイルで保存
			//String^ tmpPath = conf->stageFolderPath + "\\";
			String^ old_path = currentFmsData->path;
			String^ path = currentFmsData->path->Replace(".fmf", ".bytes");

			//トークンの変更
			//ネイティブ構造体にいったん格納
			FmsHeaderData fshd = {0};

			memcpy(fshd.fhdn.token, "FMS_", 4);		
			fshd.fhdn.data_size = currentFmsData->data_size;		
			fshd.fhdn.map_width = currentFmsData->map_width;		
			fshd.fhdn.map_height = currentFmsData->map_height;	
			fshd.fhdn.chip_width = currentFmsData->chip_width;	
			fshd.fhdn.chip_height = currentFmsData->chip_height;	
			fshd.fhdn.layer_count = currentFmsData->layer_count;	
			fshd.fhdn.bit_count = currentFmsData->bit_count;

			if (File::Exists(path) && currentFmsData->IsFms() == false )
			{
				// Displays the MessageBox.
				System::Windows::Forms::DialogResult ret = MessageBox::Show( this, "既にbytesデータが存在しています。データを引き継ぎますか？", 
						"確認", 
						MessageBoxButtons::YesNoCancel);
				if ( ret == System::Windows::Forms::DialogResult::Cancel )
				{//何もせず終了
					return;
				} else if ( ret == System::Windows::Forms::DialogResult::Yes ) 
				{
					//既存データを読み込み設定する。
					FmsData^ tmpData = nullptr;
					bool success = false;
					success = loadFmxFile<FmsHeaderData>(path,tmpData);
					if ( success == true )
					{
						fshd.crown_score = tmpData->crown_score;
						fshd.crown_chain = tmpData->crown_chain;
						fshd.crown_time = tmpData->crown_time;
						fshd.crown_dead =  tmpData->crown_dead;
						tmpData = nullptr;
					} else
					{//失敗した場合はデータを初期化する。
						fshd.crown_score = Convert::ToInt32(textBoxCrownScore->Text);
						fshd.crown_chain = Convert::ToInt32(textBoxCrownChain->Text);
						fshd.crown_time = Convert::ToInt32(textBoxCrownTime->Text);
						fshd.crown_dead = Convert::ToInt32(comboBoxCrownDead->SelectedItem->ToString());
					}
				} else 
				{
					fshd.crown_score = Convert::ToInt32(textBoxCrownScore->Text);
					fshd.crown_chain = Convert::ToInt32(textBoxCrownChain->Text);
					fshd.crown_time = Convert::ToInt32(textBoxCrownTime->Text);
					fshd.crown_dead = Convert::ToInt32(comboBoxCrownDead->SelectedItem->ToString());
				}
			} else
			{
					fshd.crown_score = Convert::ToInt32(textBoxCrownScore->Text);
					fshd.crown_chain = Convert::ToInt32(textBoxCrownChain->Text);
					fshd.crown_time = Convert::ToInt32(textBoxCrownTime->Text);
					fshd.crown_dead = Convert::ToInt32(comboBoxCrownDead->SelectedItem->ToString());
			}

			System::Byte* bytes = ( System::Byte*)&fshd;
			array<System::Byte>^ data = gcnew array<System::Byte>(sizeof(FmsHeaderData));

			pin_ptr<System::Byte> pin = &data[0];
			memcpy(&pin[0], &fshd, sizeof(FmsHeaderData));

			if ( currentFmsData->IsFms() == false )
			{//新規の場合は名前を変えてピーコしておく。
				//念のため消す
				File::Delete(path);

				File::Move(old_path, path);

				//元は消す
				File::Delete(old_path);
			}

			FileStream^ fs = nullptr;
			fs = gcnew FileStream(path, FileMode::Open);
			BinaryWriter^ w = gcnew BinaryWriter(fs);

			try 
			{
					for (int i=0; i<data->Length; i++)
					{
						w->Write(data, i, 1);
					}
					for (int i=0; i<currentFmsData->body->Length; i++)
					{
						w->Write(currentFmsData->body, i, 1);
					}
					fs->Close();

					////指定されたフォルダからファイル名一覧を取得
					SetListView(conf->stageFolderPath);

					logTextBox->AppendTextLine("ファイル[" + currentFmsData->path + "]が保存されました。");

					//読み直す
					bool success = false;
					success = loadFmxFile<FmsHeaderData>(path,currentFmsData);
					if ( success == false )
					{//失敗した場合はデータを初期化する。
						currentFmsData = nullptr;
				
						InitializeForm();

						dataGridView1->Refresh();
						return;
					}
			}
			catch (Exception^) 
			{
				Console::WriteLine("data could not be written");
			} finally 
			{
				if ( fs != nullptr ) fs->Close();
			}
		 }
private: System::Void AutoCalcBtn_Click(System::Object^  sender, System::EventArgs^  e) {
			if ( currentFmsData == nullptr ) return;

			 //死亡回数は常に０で。
			comboBoxCrownDead->SelectedIndex = 0;

			//必要得点
			textBoxCrownScore->Text = (Convert::ToInt32(labelInfoMaxScore->Text->Replace(",", "")) * 0.5).ToString();

			//平均チェイン数　未使用とする　常に０
			textBoxCrownChain->Text = "0";

			//経過時間
			int sec = (Convert::ToInt32(labelGlassItem2->Text) * 1 + 
							Convert::ToInt32(labelGlassItem3->Text) * 3 +
								Convert::ToInt32(labelGlassItem4->Text) * 10) * 1.5;
			textBoxCrownTime->Text = sec.ToString();
		 }
private: System::Void toolStripHelpBtn_Click(System::Object^  sender, System::EventArgs^  e) {

	        // Initializes the variables to pass to the MessageBox::Show method.
			String^ message = "めんづくり ver 0.9.0\r\n Monolizm LLC";
			String^ caption = "バージョン情報";

			// Displays the MessageBox.
			MessageBox::Show( this, message, caption, MessageBoxButtons::OK);

		 }
};
}

