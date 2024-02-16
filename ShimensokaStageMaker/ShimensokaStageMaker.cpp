// ShimensokaStageMaker.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "MainForm.h"

using namespace ShimensokaStageMaker;

// 2024/2/15 N.Hosaka 久しぶりに起動したら2022では起動でけん。（再ターゲットはひとまずOKにする)
// で、Visual Studio Installer の2022の個別のコンポーネントで「.NET Frameworks 4.7.2 SDK」と
// 「.NET Frameworks 4.7.2 Targeting Pack」を入れる。
// MVC～.libがないといわれるので次をやる
// ので以下の「※Visual Studio 2013 Windows XP (v120_xp) Platform Toolset がインストールされていない時」
// の項目をやればビルドできた
// 
// https://www.kunimiyasoft.com/visualstudio2022-update1753/
//
// ARM番での対応はビルドツールのARM版を入れたり、ソリューションのビルド構成を追加
[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// コントロールが作成される前に、Windows XP ビジュアル効果を有効にします
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// メイン ウィンドウを作成して、実行します
	Application::Run(gcnew MainForm());
	return 0;
}
