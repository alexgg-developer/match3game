#pragma once

#include <king\Engine.h>

#include <array>
#include <vector>

#include<glm\glm.hpp>

#include "AnimationManager.h"

class ScoreCounter
{
public:
	ScoreCounter(std::shared_ptr<AnimationManager> animationManager);
	void AddScores(const std::vector<std::vector<std::pair<King::Engine::Texture, glm::vec2>>>& pendingMatches);
	void Draw(King::Engine& engine);
private:
	static constexpr size_t NUM_GEMS = 5;

	std::array<size_t, NUM_GEMS> mGemScore;
	size_t mScore;

	std::shared_ptr<AnimationManager> mAnimationManager;
};

