
/**
 * ログ出力用にTextBox継承クラス　実装部
 */
#include "stdafx.h"
#include "LogTextBox.h"


/**
 * コンストラクタ
 * 既に存在している親クラスのパラメータを設定して初期化する。
 */
LogTextBox::LogTextBox( TextBox^ obj)
{
	//親クラス←子クラスへの代入は不可能
	//よって、コンストラクタによりパラメータコピーで実装
	this->Location = obj->Location;
	this->Multiline = obj->Multiline;
	this->Size = obj->Size;
	this->Anchor = obj->Anchor;
	this->WordWrap = obj->WordWrap;
	this->ScrollBars = obj->ScrollBars;
	this->ReadOnly = obj->ReadOnly;
	this->Visible = obj->Visible;
}

LogTextBox::~LogTextBox()
{

}

/**
 * 指定された文字列に現在時間を付けてログ出力
 */
void LogTextBox::AppendTextLine(String^ text)
{
	DateTime thisDay = DateTime::Now;

	this->AppendText("【" + thisDay.ToString("t") + "】：" + text + "\r\n");
}

