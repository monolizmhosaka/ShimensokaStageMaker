
/**
 * ���O�o�͗p��TextBox�p���N���X�@������
 */
#include "stdafx.h"
#include "LogTextBox.h"


/**
 * �R���X�g���N�^
 * ���ɑ��݂��Ă���e�N���X�̃p�����[�^��ݒ肵�ď���������B
 */
LogTextBox::LogTextBox( TextBox^ obj)
{
	//�e�N���X���q�N���X�ւ̑���͕s�\
	//����āA�R���X�g���N�^�ɂ��p�����[�^�R�s�[�Ŏ���
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
 * �w�肳�ꂽ������Ɍ��ݎ��Ԃ�t���ă��O�o��
 */
void LogTextBox::AppendTextLine(String^ text)
{
	DateTime thisDay = DateTime::Now;

	this->AppendText("�y" + thisDay.ToString("t") + "�z�F" + text + "\r\n");
}

