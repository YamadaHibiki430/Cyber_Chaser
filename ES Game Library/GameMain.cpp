// #include "Extension\DirectX11\DXGraphics11.hpp"
#include "StdAfx.h"
#include "GameMain.h"

/// <summary>
/// Allows the game to perform any initialization it needs to before starting to run.
/// This is where it can query for any required services and load all of your content.
/// Initialize will enumerate through any components and initialize them as well.
/// </summary>
bool GameMain::Initialize()
{
	// TODO: Add your initialization logic here
	WindowTitle(_T("ES Game Library"));

	Light light;
	light.Type = Light_Directional; // �f�B���N�V���i�����C�g
	light.Diffuse = Color(1.0f, 1.0f, 1.0f); // �f�B�t���[�Y�F
	light.Ambient = Color(1.0f, 1.0f, 1.0f); // �A���r�G���g�F
	light.Specular = Color(1.0f, 1.0f, 1.0f); // �X�y�L�����[�F
	light.Direction = Vector3_Down; // ���C�g�̕���(���͉��֐i��)
	GraphicsDevice.SetLight(light); // �f�o�C�X�Ƀ��C�g�����蓖�Ă�(0��)
	GraphicsDevice.EnableLight(0, TRUE);



	//�L�[�{�[�h������
	Input::Init();
	//�^�C�g���V�[���𐶐�
	SceneManager::ChangeScene(SceneManager::TITLE);
	

	return true;
}

/// <summary>
/// Finalize will be called once per game and is the place to release
/// all resource.
/// </summary>
void GameMain::Finalize()
{
	// TODO: Add your finalization logic here

}

/// <summary>
/// Allows the game to run logic such as updating the world,
/// checking for collisions, gathering input, and playing audio.
/// </summary>
/// <returns>
/// Scene continued value.
/// </returns>
int GameMain::Update()
{
	Input::Update();
	// TODO: Add your update logic here
	sceneManager.Update();
	

	return 0;
}

/// <summary>
/// This is called when the game should draw itself.
/// </summary>
void GameMain::Draw()
{
	// TODO: Add your drawing code here
	GraphicsDevice.Clear(Color_CornflowerBlue);

	GraphicsDevice.BeginScene();

	sceneManager.Draw3D();
	
	sceneManager.DrawAlpha3D();
	SpriteBatch.Begin();
	sceneManager.Draw2D();

	SpriteBatch.End();

	GraphicsDevice.EndScene();
}
