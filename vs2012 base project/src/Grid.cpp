#include "Grid.h"

#include "AnimationManager.h"
#include "Gem.h"

#include "MoveBy.h"
#include "Sequencer.h"
#include "Caller.h"

#include <string>
#include <random>

struct Grid::GridImplementation {
	static constexpr size_t GRID_SIZE = 8;
	static constexpr size_t GEM_TYPES = 5;
	static constexpr float MIN_DISTANCE_MOVE = 5.0f;
	static constexpr float INITIAL_ANIMATION_DISTANCE = 350.0f;
	static constexpr float VERTICAL_MARGIN_BETWEEN_GEMS = 5.0f;
	static constexpr float INITIAL_Y = 95.0f;
	static constexpr float TOP_MARGIN = 10.0f;

	enum class PositionState : uint8_t { RIGHT, PENDING };

	std::array<std::array<Gem, GRID_SIZE>, GRID_SIZE> mGrid;
	std::array<std::array<PositionState, GRID_SIZE>, GRID_SIZE> mGridState;
	std::array<King::Engine::Texture, GEM_TYPES> mGemType;
	int mGemWidth, mGemHeight;
	std::pair<int, int> mGemSelected;
	std::vector<std::vector<std::pair<King::Engine::Texture, glm::vec2>>> mPendingMatches;
	std::vector<std::pair<int, int>> mChangedPositions;

	std::shared_ptr<AnimationManager> mAnimationManager;

	explicit GridImplementation(int gemWidth, int gemHeight, std::shared_ptr<AnimationManager> animationManager) :
		mGemWidth(gemWidth),
		mGemHeight(gemHeight),
		mGemSelected(std::make_pair(-1, -1)),
		mAnimationManager(animationManager),
		mGemType{
		King::Engine::TEXTURE_BLUE,
		King::Engine::TEXTURE_GREEN,
		King::Engine::TEXTURE_PURPLE,
		King::Engine::TEXTURE_RED,
		King::Engine::TEXTURE_YELLOW }
	{

	}

	bool CheckMatch(int i, int j);
	King::Engine::Texture GetRandomGemType() const;
	void InitGrid();
	void InitialAnimation();
	bool IsMatch(int i, int j) const;
	bool TryMatch(const std::pair<int, int>& firstPosition, const std::pair<int, int>& secondPosition);
	void Test();
};

Grid::Grid(int gemWidth, int gemHeight, std::shared_ptr<AnimationManager> animationManager) :
	mPimpl(std::make_unique<Grid::GridImplementation>(gemWidth, gemHeight, animationManager))
{
	mPimpl->InitGrid();
	mPimpl->InitialAnimation();
}


Grid::~Grid()
{
}

void Grid::Draw(King::Engine & engine) const
{
	for (size_t i = 0; i < mPimpl->GRID_SIZE; ++i) {
		for (size_t j = 0; j < mPimpl->GRID_SIZE; ++j) {
			engine.Render(mPimpl->mGrid[i][j].mTexture,
				mPimpl->mGrid[i][j].mPosition.x,
				mPimpl->mGrid[i][j].mPosition.y);
		}
	}

	if (mPimpl->mGemSelected.first != -1) {
		engine.Write("sel",
			mPimpl->mGrid[mPimpl->mGemSelected.first][mPimpl->mGemSelected.second].mPosition.x,
			mPimpl->mGrid[mPimpl->mGemSelected.first][mPimpl->mGemSelected.second].mPosition.y);
	}
}


void Grid::RealoadGrid()
{
	for (size_t i = 0; i < mPimpl->GRID_SIZE; ++i) {
		for (size_t j = 0; j < mPimpl->GRID_SIZE; ++j) {
			mPimpl->mGrid[i][j].mTexture = mPimpl->GetRandomGemType();
		}
	}
}

void Grid::ResolvePendingMatches()
{
	/** 
	*   the index is the column
	*	the first is the first row in which there is a match
	*   the second is the number of matches in that column
	*/
	
	std::vector<std::pair<int , int>> pendingPerColumn(mPimpl->GRID_SIZE, std::make_pair<int, int>(-1, 0));
	for (int i = mPimpl->GRID_SIZE - 1; i >= 0; --i) {
		for (size_t j = 0; j < mPimpl->GRID_SIZE; ++j) {
			if (mPimpl->mGridState[i][j] == Grid::GridImplementation::PositionState::PENDING) {
				if (pendingPerColumn[j].first == -1) {
					pendingPerColumn[j].first = i;
				}
				++pendingPerColumn[j].second;
				mPimpl->mGridState[i][j] = Grid::GridImplementation::PositionState::RIGHT;

			}
		}
	}
	
	static constexpr float MOVE_SPEED = 200.0f; //pixel per second
	float distanceBetweenGems = mPimpl->mGemHeight + mPimpl->VERTICAL_MARGIN_BETWEEN_GEMS;
	for (size_t i = 0; i < mPimpl->GRID_SIZE; ++i) {
		if (pendingPerColumn[i].first != -1) {
			for (int j = pendingPerColumn[i].first; j >= 0; --j) {
				if (j - 1 >= 0) {
					mPimpl->mGrid[j][i].mTexture = mPimpl->mGrid[j - 1][i].mTexture;
				}
				else {
					mPimpl->mGrid[j][i].mTexture = mPimpl->GetRandomGemType();
					size_t i = 0;
					size_t typesOfGems = mPimpl->mGemType.size();
					while (mPimpl->IsMatch(j, i) && i <= typesOfGems) {
						mPimpl->mGrid[j][i].mTexture =
							mPimpl->mGemType[mPimpl->mGrid[j][i].mTexture % typesOfGems];
						++i;
					}
				}
				float fallDistance = distanceBetweenGems * pendingPerColumn[i].second;
				mPimpl->mGrid[j][i].mPosition.y = mPimpl->mGrid[j][i].mPosition.y - fallDistance;
				auto animation = std::make_unique<Sequencer>();
				float duration = fallDistance / MOVE_SPEED;
				auto moveBy = std::make_unique<MoveBy>(glm::vec2(0, fallDistance), duration, mPimpl->mGrid[j][i]);
				animation->Add(std::move(moveBy));

				std::function<void()> call = [this, j, i]() {
					mPimpl->mChangedPositions.emplace_back(std::make_pair(j, i));
				};
				auto caller = std::make_unique<Caller>(call);
				animation->Add(std::move(caller));
				mPimpl->mAnimationManager->Add(std::move(animation));
			}
		}
	}
}

void Grid::OnMouseDown(float x, float y)
{
	auto lastGemSelected = mPimpl->mGemSelected;
	mPimpl->mGemSelected.first = -1;
	mPimpl->mGemSelected.second = -1;
	if (SelectGem(x, y) &&
		lastGemSelected.first != -1 &&
		mPimpl->mGemSelected != lastGemSelected) {
		bool isHorizontalNeighboor = lastGemSelected.first == mPimpl->mGemSelected.first - 1;
		isHorizontalNeighboor |= lastGemSelected.first == mPimpl->mGemSelected.first + 1;
		isHorizontalNeighboor &= (lastGemSelected.second == mPimpl->mGemSelected.second);
		bool isVerticalNeighboor = lastGemSelected.second == mPimpl->mGemSelected.second - 1;
		isVerticalNeighboor |= lastGemSelected.second == mPimpl->mGemSelected.second + 1;
		isVerticalNeighboor &= (lastGemSelected.first == mPimpl->mGemSelected.first);
		if (isHorizontalNeighboor != isVerticalNeighboor) {
			if (mPimpl->TryMatch(lastGemSelected, mPimpl->mGemSelected)) {
				mPimpl->mGemSelected.first = -1;
				mPimpl->mGemSelected.second = -1;
			}
		}
	}
}


void Grid::GridImplementation::Test()
{
	/*constexpr float INITIAL_ANIMATION_TIME = 0.75f;
	constexpr float INITIAL_ANIMATION_LAPSE = 0.1f;
	constexpr float BOUNCE_DISTANCE = 4.0f;
	constexpr float BOUNCE_ANIMATION_TIME = 0.2f;
	size_t i = 0;
	size_t j = 0;
	auto moveBy =
		std::make_unique<MoveBy>(glm::vec2(0.0f, INITIAL_ANIMATION_DISTANCE),
			INITIAL_ANIMATION_TIME + (GRID_SIZE - 1 - j) * INITIAL_ANIMATION_LAPSE,
			mGrid[i][j]);
	auto bounceUp =
		std::make_unique<MoveBy>(glm::vec2(0.0f, -BOUNCE_DISTANCE),
			BOUNCE_ANIMATION_TIME,
			mGrid[i][j]);
	auto bounceDown =
		std::make_unique<MoveBy>(glm::vec2(0.0f, BOUNCE_DISTANCE),
			BOUNCE_ANIMATION_TIME,
			mGrid[i][j]);
	//mAnimationManager->Add(std::move(moveBy));
	auto sequencer = std::make_unique<Sequencer>();
	sequencer->Add(std::move(moveBy));
	sequencer->Add(std::move(bounceUp));
	sequencer->Add(std::move(bounceDown));
	mAnimationManager->Add(std::move(sequencer));*/
	std::cout << "tst" << std::endl;

	for (size_t j = 0; j < GRID_SIZE; ++j) {
		std::cout << mGrid[0][j].mTexture << std::endl;
	}

}

void Grid::OnMouseUp(float x, float y)
{
	//mPimpl->Test();
	auto caller = std::make_unique<Caller>(std::bind(&Grid::GridImplementation::Test, *mPimpl));
	//caller->Update(0.0f);
	mPimpl->mAnimationManager->Add(std::move(caller));
}



void Grid::OnMouseMove(float x, float y, float mouseDownX, float mouseDownY)
{
	if (mPimpl->mGemSelected.first != -1) {
		float horizontalDistance = x - mouseDownX;
		float verticalDistance = y - mouseDownY;
		float absHorizontalDistance = std::abs(horizontalDistance);
		float absVerticalDistance = std::abs(verticalDistance);
		if (absHorizontalDistance > absVerticalDistance) {
			if (horizontalDistance > mPimpl->MIN_DISTANCE_MOVE &&
				mPimpl->mGemSelected.second + 1 < static_cast<int>(mPimpl->GRID_SIZE)) {
				mPimpl->TryMatch(
					std::make_pair(mPimpl->mGemSelected.first, mPimpl->mGemSelected.second + 1),
					mPimpl->mGemSelected);
				mPimpl->mGemSelected.first = -1;
				mPimpl->mGemSelected.second = -1;
			}
			else if (horizontalDistance < -mPimpl->MIN_DISTANCE_MOVE && mPimpl->mGemSelected.second - 1 >= 0) {
				mPimpl->TryMatch(
					std::make_pair(mPimpl->mGemSelected.first, mPimpl->mGemSelected.second - 1),
					mPimpl->mGemSelected);
				mPimpl->mGemSelected.first = -1;
				mPimpl->mGemSelected.second = -1;
			}
		}
		else {
			if (verticalDistance > mPimpl->MIN_DISTANCE_MOVE &&
				mPimpl->mGemSelected.first + 1 < static_cast<int>(mPimpl->GRID_SIZE)) {
				mPimpl->TryMatch(
					std::make_pair(mPimpl->mGemSelected.first + 1, mPimpl->mGemSelected.second),
					mPimpl->mGemSelected);
				mPimpl->mGemSelected.first = -1;
				mPimpl->mGemSelected.second = -1;
			}
			else if (verticalDistance < -mPimpl->MIN_DISTANCE_MOVE &&  mPimpl->mGemSelected.first - 1 >= 0) {
				mPimpl->TryMatch(std::make_pair(mPimpl->mGemSelected.first - 1, mPimpl->mGemSelected.second),
					mPimpl->mGemSelected);
				mPimpl->mGemSelected.first = -1;
				mPimpl->mGemSelected.second = -1;
			}
		}
	}
}

bool Grid::SelectGem(float x, float y)
{
	size_t j = 0;
	for (; j < mPimpl->GRID_SIZE; ++j) {
		if (x >= mPimpl->mGrid[0][j].mPosition.x &&
			x <= mPimpl->mGrid[0][j].mPosition.x + mPimpl->mGemWidth) {
			break;
		}
	}
	for (size_t i = 0; j < mPimpl->GRID_SIZE && i < mPimpl->GRID_SIZE; ++i) {
		if (y >= mPimpl->mGrid[i][j].mPosition.y &&
			y <= mPimpl->mGrid[i][j].mPosition.y + mPimpl->mGemHeight) {
			mPimpl->mGemSelected.first = i;
			mPimpl->mGemSelected.second = j;
			return true;
		}
	}

	return false;
}


void Grid::FixGrid()
{
	for (size_t i = 0; i < mPimpl->GRID_SIZE; ++i) {
		for (size_t j = 0; j < mPimpl->GRID_SIZE; ++j) {
			while (mPimpl->IsMatch(i, j)) {
				mPimpl->mGrid[i][j].mTexture =
					mPimpl->mGemType[mPimpl->mGrid[i][j].mTexture % mPimpl->mGemType.size()];
			}
		}
	}
}

const std::vector<std::vector<std::pair<King::Engine::Texture, glm::vec2>>>& Grid::GetPendingMatches() const
{
	return mPimpl->mPendingMatches;
}

void Grid::CheckChangedPositions()
{
	for (const auto& changedPosition : mPimpl->mChangedPositions) {
		if (mPimpl->mGridState[changedPosition.first][changedPosition.second] !=
			Grid::GridImplementation::PositionState::PENDING) {
			mPimpl->CheckMatch(changedPosition.first, changedPosition.second);
		}
	}
	mPimpl->mChangedPositions.clear();
}

void Grid::ClearPendingMatches()
{
	mPimpl->mPendingMatches.clear();
}

void Grid::GridImplementation::InitGrid()
{
	constexpr float INITIAL_X = 332.0f;
	constexpr float HORIZONTAL_MARGIN_BETWEEN_GEMS = 8.0f;
	constexpr float LEFT_MARGIN = 25.0f;


	for (size_t i = 0; i < GRID_SIZE; ++i) {
		for (size_t j = 0; j < GRID_SIZE; ++j) {
			mGrid[i][j].mTexture = GetRandomGemType();
			mGrid[i][j].mPosition.x =
				LEFT_MARGIN + INITIAL_X + static_cast<float>(j * (mGemWidth + HORIZONTAL_MARGIN_BETWEEN_GEMS));
			mGrid[i][j].mPosition.y =
				TOP_MARGIN + INITIAL_Y + static_cast<float>(i * (mGemHeight + VERTICAL_MARGIN_BETWEEN_GEMS))
				- INITIAL_ANIMATION_DISTANCE;
			mGridState[i][j] = PositionState::RIGHT;
		}
	}

	for (size_t i = 0; i < GRID_SIZE; ++i) {
		for (size_t j = 0; j < GRID_SIZE; ++j) {
			while (IsMatch(i, j)) {
				mGrid[i][j].mTexture = mGemType[mGrid[i][j].mTexture % mGemType.size()];
			}
		}
	}
}

void Grid::GridImplementation::InitialAnimation()
{
	constexpr float INITIAL_ANIMATION_TIME = 0.75f;
	constexpr float INITIAL_ANIMATION_LAPSE = 0.1f;
	constexpr float BOUNCE_DISTANCE = 4.0f;
	constexpr float BOUNCE_ANIMATION_TIME = 0.1f;

	for (size_t i = 0; i < GRID_SIZE; ++i) {
		for (size_t j = 0; j < GRID_SIZE; ++j) {
			auto moveBy =
				std::make_unique<MoveBy>(glm::vec2(0.0f, INITIAL_ANIMATION_DISTANCE),
					INITIAL_ANIMATION_TIME + (GRID_SIZE - 1 - j) * INITIAL_ANIMATION_LAPSE,
					mGrid[i][j]);
			auto bounceUp =
				std::make_unique<MoveBy>(glm::vec2(0.0f, -BOUNCE_DISTANCE),
					BOUNCE_ANIMATION_TIME,
					mGrid[i][j]);
			auto bounceDown =
				std::make_unique<MoveBy>(glm::vec2(0.0f, BOUNCE_DISTANCE),
					BOUNCE_ANIMATION_TIME,
					mGrid[i][j]);
			//mAnimationManager->Add(std::move(moveBy));
			auto sequencer = std::make_unique<Sequencer>();
			sequencer->Add(std::move(moveBy));
			sequencer->Add(std::move(bounceUp));
			sequencer->Add(std::move(bounceDown));
			mAnimationManager->Add(std::move(sequencer));
		}
	}
}

bool Grid::GridImplementation::IsMatch(int i, int j) const
{
	bool isMatch = false;
	King::Engine::Texture type = mGrid[i][j].mTexture;
	int endColumn = std::min(j + 2, static_cast<int>(GRID_SIZE) - 1);
	size_t matches = 0;
	for (int column = std::max(j - 2, 0); matches < 3 && column <= endColumn; ++column) {
		if (mGrid[i][column].mTexture == type) {
			++matches;
		}
		else {
			matches = 0;
		}
	}
	isMatch = matches >= 3;
	if (!isMatch) {
		matches = 0;
		int endRow = std::min(i + 2, static_cast<int>(GRID_SIZE) - 1);
		size_t matches = 0;
		for (int row = std::max(i - 2, 0); matches < 3 && row <= endRow; ++row) {
			if (mGrid[row][j].mTexture == type) {
				++matches;
			}
			else {
				matches = 0;
			}
		}
		isMatch = matches >= 3;
	}

	return isMatch;
}

bool Grid::GridImplementation::TryMatch(const std::pair<int, int>& firstPosition, const std::pair<int, int>& secondPosition)
{
	static constexpr float MOVE_DURATION = 0.2f;

	auto deltaPosition = mGrid[firstPosition.first][firstPosition.second].mPosition - mGrid[secondPosition.first][secondPosition.second].mPosition;
	
	auto firstAnimation = std::make_unique<Sequencer>();
	auto firstMoveBy = std::make_unique<MoveBy>(-deltaPosition, MOVE_DURATION, mGrid[firstPosition.first][firstPosition.second]);
	firstAnimation->Add(std::move(firstMoveBy));

	auto secondAnimation = std::make_unique<Sequencer>();
	auto secondMoveBy = std::make_unique<MoveBy>(deltaPosition, MOVE_DURATION, mGrid[secondPosition.first][secondPosition.second]);
	secondAnimation->Add(std::move(secondMoveBy));

	//CheckMatches
	std::swap(mGrid[firstPosition.first][firstPosition.second].mTexture, mGrid[secondPosition.first][secondPosition.second].mTexture);
	bool isThereMatchFirstPosition = IsMatch(firstPosition.first, firstPosition.second);
	bool isThereMatchSecondPosition = IsMatch(secondPosition.first, secondPosition.second);
	bool isMatch = isThereMatchFirstPosition || isThereMatchSecondPosition;
	std::swap(mGrid[firstPosition.first][firstPosition.second].mTexture, mGrid[secondPosition.first][secondPosition.second].mTexture);
	if (!isMatch) {
		auto firstMoveByBack = std::make_unique<MoveBy>(deltaPosition, MOVE_DURATION, mGrid[firstPosition.first][firstPosition.second]);
		firstAnimation->Add(std::move(firstMoveByBack));

		auto secondMoveByBack = std::make_unique<MoveBy>(-deltaPosition, MOVE_DURATION, mGrid[secondPosition.first][secondPosition.second]);
		secondAnimation->Add(std::move(secondMoveByBack));
	}
	else {
		std::function<void()> call = [this, firstPosition, secondPosition]() {
			std::swap(mGrid[firstPosition.first][firstPosition.second].mTexture, mGrid[secondPosition.first][secondPosition.second].mTexture);
			bool isThereMatchFirstPosition = CheckMatch(firstPosition.first, firstPosition.second);
			bool isThereMatchSecondPosition = CheckMatch(secondPosition.first, secondPosition.second);
			if (!isThereMatchFirstPosition && !isThereMatchSecondPosition) {
				std::swap(mGrid[firstPosition.first][firstPosition.second].mTexture, mGrid[secondPosition.first][secondPosition.second].mTexture);		
			}
		};
		auto caller = std::make_unique<Caller>(call);
		firstAnimation->Add(std::move(caller));
		auto firstMoveByBackInstant = std::make_unique<MoveBy>(deltaPosition, 0.001f, mGrid[firstPosition.first][firstPosition.second]);
		firstAnimation->Add(std::move(firstMoveByBackInstant));

		auto secondMoveByBackInstant = std::make_unique<MoveBy>(-deltaPosition, 0.001f, mGrid[secondPosition.first][secondPosition.second]);
		secondAnimation->Add(std::move(secondMoveByBackInstant));
	}
	mAnimationManager->Add(std::move(firstAnimation));
	mAnimationManager->Add(std::move(secondAnimation));

	return isMatch;
}



/**
	It does mark as pending the matches found and save the matches to treat them afterwards.
*/
bool Grid::GridImplementation::CheckMatch(int i, int j)
{
	bool isMatch = false;
	King::Engine::Texture type = mGrid[i][j].mTexture;
	std::vector<std::pair<int, int>> matches{ std::make_pair(i, j) };
	int startColumn = j + 1;
	int endColumn = std::min(j + 2, static_cast<int>(GRID_SIZE) - 1);
	for (int column = startColumn; column <= endColumn; ++column) {
		if (mGrid[i][column].mTexture == type) {
			matches.emplace_back(std::make_pair(i, column));
		}
		else {
			break;
		}
	}

	startColumn = j - 1;
	endColumn = std::max(j - 2, 0);
	for (int column = startColumn; column >= endColumn; --column) {
		if (mGrid[i][column].mTexture == type) {
			matches.emplace_back(std::make_pair(i, column));
		}
		else {
			break;
		}
	}

	std::vector<std::pair<King::Engine::Texture, glm::vec2>> pendingMatch;
	if (matches.size() >= 3) {
		isMatch = true;
		for (const auto& match : matches) {
			mGridState[match.first][match.second] = PositionState::PENDING;
			pendingMatch.emplace_back(mGrid[match.first][match.second].mTexture, mGrid[match.first][match.second].mPosition);
		}
	}
	matches.clear();
	matches.emplace_back(std::make_pair(i, j));

	int startRow = i + 1;
	int endRow = std::min(i + 2, static_cast<int>(GRID_SIZE) - 1);
	for (int row = startRow; row <= endRow; ++row) {
		if (mGrid[row][j].mTexture == type) {
			matches.emplace_back(std::make_pair(row, j));
		}
		else {
			break;
		}
	}

	startRow = i - 1;
	endRow = std::max(i - 2, 0);
	for (int row = startRow; row >= endRow; --row) {
		if (mGrid[row][j].mTexture == type) {
			matches.emplace_back(std::make_pair(row, j));
		}
		else {
			break;
		}
	}

	if (matches.size() >= 3) {
		isMatch = true;
		//std::vector<King::Engine::Texture> pendingMatch;
		for (const auto& match : matches) {
			mGridState[match.first][match.second] = PositionState::PENDING;
			pendingMatch.emplace_back(mGrid[match.first][match.second].mTexture, mGrid[match.first][match.second].mPosition);
		}
	}
	if (isMatch) {
		mPendingMatches.push_back(pendingMatch);
	}
	return isMatch;
}

King::Engine::Texture Grid::GridImplementation::GetRandomGemType() const
{
	static std::random_device rd; // obtain a random number from hardware
	static std::mt19937 engine(rd()); // seed the generator
	static std::uniform_int_distribution<> distr(0, mGemType.size() - 1); // define the range

	int randomGem = distr(engine);
	return mGemType[randomGem];
}