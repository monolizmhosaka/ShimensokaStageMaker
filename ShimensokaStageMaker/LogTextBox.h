#pragma once

using namespace System;
using namespace System::Windows::Forms;

/**
 * ログ出力用にTextBox継承クラス
 */
public ref class LogTextBox : public TextBox
{
public:
	LogTextBox(TextBox^ obj);

	//引数で渡された文字列に現在時刻と改行を付加してテキストに入れます。
	void AppendTextLine(String^ text);

protected:
	~LogTextBox();

};

