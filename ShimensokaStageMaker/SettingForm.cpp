#include "stdafx.h"
#include "MainForm.h"

#include "SettingForm.h"

namespace ShimensokaStageMaker {
		
/**
 * �R���X�g���N�^
 * ���ɑ��݂��Ă���e�N���X�̃p�����[�^��ݒ肵�ď���������B
 */
	SettingForm::SettingForm(AppConfigData^ pc)
{
	InitializeComponent();
	//
	//TODO: �����ɃR���X�g���N�^�[ �R�[�h��ǉ����܂�
	//

	//�I�[�i�[�����ݒ�����Q��
	ownerConfig = pc;

	//�e�L�X�g�{�b�N�X�ɐݒ�ɐݒ肷��B
	textBoxLayer1Image->Text = ownerConfig->layer1ImagePath;
	textBoxLayer2Image->Text = ownerConfig->layer2ImagePath;
}

} // namespace ShimensokaStageMaker
