#include "stdafx.h"
#include "MainForm.h"

#include "SettingForm.h"

namespace ShimensokaStageMaker {
		
/**
 * コンストラクタ
 * 既に存在している親クラスのパラメータを設定して初期化する。
 */
	SettingForm::SettingForm(AppConfigData^ pc)
{
	InitializeComponent();
	//
	//TODO: ここにコンストラクター コードを追加します
	//

	//オーナーが持つ設定情報を参照
	ownerConfig = pc;

	//テキストボックスに設定に設定する。
	textBoxLayer1Image->Text = ownerConfig->layer1ImagePath;
	textBoxLayer2Image->Text = ownerConfig->layer2ImagePath;
}

} // namespace ShimensokaStageMaker
