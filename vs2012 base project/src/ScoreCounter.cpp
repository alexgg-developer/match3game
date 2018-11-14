#include "ScoreCounter.h"
#include "TextAnimation.h"

#include<string>
#include <random>

#include <glm/gtc/matrix_transform.hpp>

/**
	@brief Ctor of Score counter.

	Each position of mGemScore corresponds to a texture in the order they are declared in the King\Engine.h;

	TEXTURE_BLUE,
	TEXTURE_GREEN,
	TEXTURE_PURPLE,
	TEXTURE_RED,
	TEXTURE_YELLOW,

*/
ScoreCounter::ScoreCounter(std::shared_ptr<AnimationManager> animationManager) :
	mAnimationManager(animationManager),
	mGemScore { 50, 10, 20, 40, 30 },
	mScore(0)
{}

void ScoreCounter::AddScores(const std::vector<std::vector<std::pair<King::Engine::Texture, glm::vec2>>>& pendingMatches)
{
	static constexpr float SCORE_ANIMATION_MOVEMENT = 35.0f;
	static constexpr float SCORE_ANIMATION_DURATION = 0.65f;

	static std::random_device rd; // obtain a random number from hardware
	static std::mt19937 engine(rd()); // seed the generator


	for (const auto& pendingMatchesGroup : pendingMatches) {
		size_t scoreGroup = 0;
		for (const auto& pendingMatch : pendingMatchesGroup) {
			scoreGroup += mGemScore[pendingMatch.first - King::Engine::TEXTURE_BLUE];
		}
		//normal match is 3 so if a greater match is done a multiplier is then applied to the score.		
		scoreGroup *= (pendingMatchesGroup.size() - 2);
		mScore += scoreGroup;

		std::uniform_int_distribution<> distr(0, pendingMatchesGroup.size() - 1); // define the range
		int randomGem = distr(engine);

		auto scoreAnimation = 
			std::make_unique<TextAnimation>(
				std::to_string(scoreGroup), 
				pendingMatchesGroup[randomGem].second, 
				glm::vec2(0, -SCORE_ANIMATION_MOVEMENT), 
				SCORE_ANIMATION_DURATION);
		mAnimationManager->AddDrawable(std::move(scoreAnimation));
	}
}

void ScoreCounter::Draw(King::Engine& engine)
{
	constexpr float LEFT_MARGIN_SCORE = 125.0f;
	constexpr float TOP_MARGIN_SCORE = 75.0f;

	static const glm::vec3 MARGIN_SCORE_TEXT = glm::vec3(15.0f, 15.0f, 0.0f);
	static const glm::vec3 SCALE_SCORE_TEXT = glm::vec3(2.0f, 2.0f, 1.0f);
	static const glm::vec3 SCALE_SCORE = glm::vec3(1.5f, 1.5f, 1.0f);

	auto score = std::to_string(mScore);
	auto transform = glm::scale(glm::translate(glm::mat4(1.0), MARGIN_SCORE_TEXT), SCALE_SCORE_TEXT);
	engine.Write("Score:", transform);
	transform = glm::scale(
		glm::translate(glm::mat4(1.0), 
			glm::vec3(LEFT_MARGIN_SCORE - engine.CalculateStringWidth(score.c_str()), TOP_MARGIN_SCORE, 0.0f)), 
		SCALE_SCORE);
	engine.Write(score.c_str(), transform);
}
