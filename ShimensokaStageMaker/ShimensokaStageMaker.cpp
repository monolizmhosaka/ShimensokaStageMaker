// ShimensokaStageMaker.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

#include "stdafx.h"
#include "MainForm.h"

using namespace ShimensokaStageMaker;

// 2024/2/15 N.Hosaka �v���Ԃ�ɋN��������2022�ł͋N���ł���B�i�ă^�[�Q�b�g�͂ЂƂ܂�OK�ɂ���)
// �ŁAVisual Studio Installer ��2022�̌ʂ̃R���|�[�l���g�Łu.NET Frameworks 4.7.2 SDK�v��
// �u.NET Frameworks 4.7.2 Targeting Pack�v������B
// MVC�`.lib���Ȃ��Ƃ�����̂Ŏ������
// �̂ňȉ��́u��Visual Studio 2013 Windows XP (v120_xp) Platform Toolset ���C���X�g�[������Ă��Ȃ����v
// �̍��ڂ����΃r���h�ł���
// 
// https://www.kunimiyasoft.com/visualstudio2022-update1753/
//
// ARM�Ԃł̑Ή��̓r���h�c�[����ARM�ł���ꂽ��A�\�����[�V�����̃r���h�\����ǉ�
[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// �R���g���[�����쐬�����O�ɁAWindows XP �r�W���A�����ʂ�L���ɂ��܂�
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// ���C�� �E�B���h�E���쐬���āA���s���܂�
	Application::Run(gcnew MainForm());
	return 0;
}
