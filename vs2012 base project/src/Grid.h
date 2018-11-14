#pragma once
#include <array>
#include <vector>

#include <king/Engine.h>

#include "InputReceiver.h"

class AnimationManager;

class Grid: public InputReceiver
{
public:
	Grid() = delete;
	explicit Grid(int gemWidth, int gemHeight, std::shared_ptr<AnimationManager> animationManager);
	~Grid();
	void CheckChangedPositions();
	void ClearPendingMatches();
	void Draw(King::Engine& engine) const;
	void FixGrid();
	const std::vector<std::vector<std::pair<King::Engine::Texture, glm::vec2>>>& GetPendingMatches() const;
	void OnMouseDown(float x, float y) override;
	void OnMouseMove(float x, float y, float mouseDownX, float mouseDownY) override;
	void OnMouseUp(float x, float y) override;
	void RealoadGrid();
	void ResolvePendingMatches();
	bool SelectGem(float x, float y);
private:
	struct GridImplementation;
	std::unique_ptr<GridImplementation> mPimpl;
};

