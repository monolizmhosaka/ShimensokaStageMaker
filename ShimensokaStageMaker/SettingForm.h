#pragma once

#include "common.h"
#include "SettingData.h"

namespace ShimensokaStageMaker {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// SettingForm の概要
	/// </summary>
	public ref class SettingForm : public System::Windows::Forms::Form
	{
	public:
		SettingForm(AppConfigData^ pc);

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~SettingForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: AppConfigData^ ownerConfig;
	private: System::Windows::Forms::TextBox^  textBoxLayer1Image;
	private: System::Windows::Forms::Button^  GlassFileOpenBtn;


	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::Button^  OKBtn;
	private: System::Windows::Forms::Button^  CancelBtn;


	private: System::Windows::Forms::TextBox^  textBoxLayer2Image;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  ItemFileOpenBtn;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	protected: 

	protected: 

	protected: 

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			this->textBoxLayer1Image = (gcnew System::Windows::Forms::TextBox());
			this->GlassFileOpenBtn = (gcnew System::Windows::Forms::Button());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->OKBtn = (gcnew System::Windows::Forms::Button());
			this->CancelBtn = (gcnew System::Windows::Forms::Button());
			this->textBoxLayer2Image = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->ItemFileOpenBtn = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// textBoxLayer1Image
			// 
			this->textBoxLayer1Image->Location = System::Drawing::Point(80, 28);
			this->textBoxLayer1Image->Name = L"textBoxLayer1Image";
			this->textBoxLayer1Image->Size = System::Drawing::Size(368, 19);
			this->textBoxLayer1Image->TabIndex = 0;
			// 
			// GlassFileOpenBtn
			// 
			this->GlassFileOpenBtn->Location = System::Drawing::Point(454, 28);
			this->GlassFileOpenBtn->Name = L"GlassFileOpenBtn";
			this->GlassFileOpenBtn->Size = System::Drawing::Size(51, 19);
			this->GlassFileOpenBtn->TabIndex = 1;
			this->GlassFileOpenBtn->Tag = L"BTN_LAYER1";
			this->GlassFileOpenBtn->Text = L"．．．";
			this->GlassFileOpenBtn->UseVisualStyleBackColor = true;
			this->GlassFileOpenBtn->Click += gcnew System::EventHandler(this, &SettingForm::FileOpen_Click);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// OKBtn
			// 
			this->OKBtn->Location = System::Drawing::Point(392, 107);
			this->OKBtn->Name = L"OKBtn";
			this->OKBtn->Size = System::Drawing::Size(71, 38);
			this->OKBtn->TabIndex = 2;
			this->OKBtn->Text = L"OK";
			this->OKBtn->UseVisualStyleBackColor = true;
			this->OKBtn->Click += gcnew System::EventHandler(this, &SettingForm::OKBtn_Click);
			// 
			// CancelBtn
			// 
			this->CancelBtn->Location = System::Drawing::Point(479, 107);
			this->CancelBtn->Name = L"CancelBtn";
			this->CancelBtn->Size = System::Drawing::Size(71, 38);
			this->CancelBtn->TabIndex = 3;
			this->CancelBtn->Text = L"キャンセル";
			this->CancelBtn->UseVisualStyleBackColor = true;
			this->CancelBtn->Click += gcnew System::EventHandler(this, &SettingForm::CancelBtn_Click);
			// 
			// textBoxLayer2Image
			// 
			this->textBoxLayer2Image->Location = System::Drawing::Point(80, 54);
			this->textBoxLayer2Image->Name = L"textBoxLayer2Image";
			this->textBoxLayer2Image->Size = System::Drawing::Size(368, 19);
			this->textBoxLayer2Image->TabIndex = 4;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label1->Location = System::Drawing::Point(24, 30);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(40, 15);
			this->label1->TabIndex = 5;
			this->label1->Text = L"ガラス";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			this->label2->Location = System::Drawing::Point(22, 54);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(52, 15);
			this->label2->TabIndex = 6;
			this->label2->Text = L"アイテム";
			// 
			// ItemFileOpenBtn
			// 
			this->ItemFileOpenBtn->Location = System::Drawing::Point(454, 54);
			this->ItemFileOpenBtn->Name = L"ItemFileOpenBtn";
			this->ItemFileOpenBtn->Size = System::Drawing::Size(51, 19);
			this->ItemFileOpenBtn->TabIndex = 7;
			this->ItemFileOpenBtn->Tag = L"BTN_LAYER2";
			this->ItemFileOpenBtn->Text = L"．．．";
			this->ItemFileOpenBtn->UseVisualStyleBackColor = true;
			this->ItemFileOpenBtn->Click += gcnew System::EventHandler(this, &SettingForm::FileOpen_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->ItemFileOpenBtn);
			this->groupBox1->Controls->Add(this->textBoxLayer1Image);
			this->groupBox1->Controls->Add(this->textBoxLayer2Image);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->GlassFileOpenBtn);
			this->groupBox1->Location = System::Drawing::Point(15, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(535, 89);
			this->groupBox1->TabIndex = 8;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"チップ画像";
			// 
			// SettingForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(567, 157);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->CancelBtn);
			this->Controls->Add(this->OKBtn);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"SettingForm";
			this->Text = L"環境設定";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void OKBtn_Click(System::Object^  sender, System::EventArgs^  e) {

				//設定値を退避
				ownerConfig->layer1ImagePath = textBoxLayer1Image->Text;
				ownerConfig->layer2ImagePath = textBoxLayer2Image->Text;

				//ファイルに書き込む
				XmlConfigManager^ xml = gcnew XmlConfigManager(System::IO::Directory::GetCurrentDirectory() +"\\" + gcnew String(SETTING_FILE));
				if ( xml->Save(ownerConfig) == false )
				{
					MessageBox::Show("設定ファイルの書き込みに失敗しました。", "エラー", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}

				//ウィンドウを閉じる
				this->Close();
		 }
	private: System::Void CancelBtn_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
		 }
private: System::Void FileOpen_Click(System::Object^  sender, System::EventArgs^  e) {
				Button^ btn = (Button^)sender;

				openFileDialog1->InitialDirectory = "c:\\" ;
				openFileDialog1->Filter = "bmp files (*.bmp)|*.bmp|All files (*.*)|*.*" ;
				openFileDialog1->FilterIndex = 2 ;
				openFileDialog1->RestoreDirectory = true ;

				if(openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				{
					if ( btn->Tag == "BTN_LAYER1" ) textBoxLayer1Image->Text = openFileDialog1->FileName;
					else textBoxLayer2Image->Text = openFileDialog1->FileName;
				}
		 }
};
}
