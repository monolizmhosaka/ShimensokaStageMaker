#pragma once

using namespace System;
using namespace System::Windows::Forms;

/**
 * ���O�o�͗p��TextBox�p���N���X
 */
public ref class LogTextBox : public TextBox
{
public:
	LogTextBox(TextBox^ obj);

	//�����œn���ꂽ������Ɍ��ݎ����Ɖ��s��t�����ăe�L�X�g�ɓ���܂��B
	void AppendTextLine(String^ text);

protected:
	~LogTextBox();

};

