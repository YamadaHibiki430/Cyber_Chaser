#include "../StdAfx.h"
#include "TemplateScene.hpp"

/// <summary>
/// Allows the game to perform any initialization it needs to before starting to run.
/// This is where it can query for any required services and load any non-graphic
/// related content.  Calling base.Initialize will enumerate through any components
/// and initialize them as well.
/// </summary>
bool TemplateScene::Initialize()
{
	// TODO: Add your initialization logic here


	return true;
}

/// <summary>
/// Finalize will be called once per game and is the place to release
/// all resource.
/// </summary>
void TemplateScene::Finalize()
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
int TemplateScene::Update()
{
    // TODO: Add your update logic here


	return 0;
}

/// <summary>
/// This is called when the game should draw itself.
/// </summary>
void TemplateScene::Draw()
{
	// TODO: Add your drawing code here
	GraphicsDevice.Clear(Color_CornflowerBlue);

	GraphicsDevice.BeginScene();


	GraphicsDevice.EndScene();
}
