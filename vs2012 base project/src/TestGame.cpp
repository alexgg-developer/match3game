#include "TestGame.h"
#include "AnimationManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <string>


TestGame::TestGame()
	: mEngine("./assets")
	, mTransformBackground(1.0f)
	, mAnimationManager(std::make_shared<AnimationManager>())
	, mGrid(mEngine.GetTextureWidth(King::Engine::TEXTURE_BLUE), mEngine.GetTextureHeight(King::Engine::TEXTURE_BLUE), mAnimationManager)
	, mScoreCounter(mAnimationManager)
	, mCounter(4.0f, glm::vec2(mEngine.GetWidth() * 0.5f + 100.0f, mEngine.GetHeight() * 0.5f - 100.0f), 1.0f)
{
}


void TestGame::Start()
{
	int width = mEngine.GetTextureWidth(King::Engine::TEXTURE_BACKGROUND);
	int height = mEngine.GetTextureHeight(King::Engine::TEXTURE_BACKGROUND);
	int windowWidth = mEngine.GetWidth();
	int windowHeight = mEngine.GetHeight();
	float scaleX = static_cast<float>(windowWidth) / width;
	float scaleY = static_cast<float>(windowHeight) / height;
	mTransformBackground = glm::scale(mTransformBackground, glm::vec3(scaleX, scaleY, 1.0f));
	mInputControl.Register(mGrid);
	mEngine.Start(*this);
}


//pivot top-left
//mEngine.Render(King::Engine::TEXTURE_BLUE, 0, 0);

void TestGame::Update()
{	
	float lastframeSeconds = mEngine.GetLastFrameSeconds();
	mEngine.Render(King::Engine::TEXTURE_BACKGROUND, mTransformBackground);
	mGrid.Draw(mEngine);
	mTimeCounter.Draw(mEngine);
	mScoreCounter.Draw(mEngine);
	mAnimationManager->Update(lastframeSeconds);
	mAnimationManager->Draw(mEngine);
	mGrid.CheckChangedPositions();
	if (mCounter.HasStopped()) {
		mTimeCounter.Update(lastframeSeconds);
		if (!mTimeCounter.HasFinished()) {
			mInputControl.Update(mEngine.GetMouseButtonDown(), mEngine.GetMouseX(), mEngine.GetMouseY());
		}
		else {
			mEngine.Write("Thank you for playing!", 5.0f, 300.0f);
		}
	}
	else {
		mCounter.Update(lastframeSeconds);
		mCounter.Draw(mEngine);
	}
	mScoreCounter.AddScores(mGrid.GetPendingMatches());
	mGrid.ClearPendingMatches();
	mGrid.ResolvePendingMatches();

	/*if (mEngine.GetMouseButtonDown()) {
		mEngine.Write(std::to_string(mEngine.GetMouseX()).c_str(), mEngine.GetMouseX(), mEngine.GetMouseY());
		mEngine.Write(std::to_string(mEngine.GetMouseY()).c_str(), mEngine.GetMouseX(), mEngine.GetMouseY() + 25);
	}*/
}