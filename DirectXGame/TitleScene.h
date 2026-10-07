#pragma once
#include "Skydome.h"
#include <KamataEngine.h>

#include "Fade.h"
class TitleScene {
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

	void Initialize();
	void Update();
	void Draw();
	~TitleScene();

	bool isFinished() const { return finished_; }

private:
	bool finished_ = false;

	KamataEngine::Model* modelPlayer_ = nullptr;

	KamataEngine::Model* modelRoad_ = nullptr;

	KamataEngine::Model* modelTitle_ = nullptr;

	KamataEngine::Model* modelGround_ = nullptr;

	KamataEngine::Camera camera_;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::WorldTransform worldTransformPlayer_;

	KamataEngine::WorldTransform worldTransformTitle_;

	KamataEngine::WorldTransform worldTransformGround_;

	KamataEngine::WorldTransform worldTransformRoad_[4];

	float vibeTimer_ = 0.0f;
	float vibeY_ = 0.0f;

	const float kRoadLength = 44.5f;

	float roadZ_ = 0.0f;
	const float kRoadSpeed = 0.2f;   // 道路が流れるスピード
	const float kRoadLoopZ = -44.5f; // 奥に戻すまでの値
	const float kRoadResetZ = 0.0f;

	float titleTimer_ = 0.0f;

	uint32_t soundHandleBike_ = 0;
	uint32_t bikeSoundPlayTimer_ = 0;

	uint32_t textureHandleTitleBack_ = 0;
	KamataEngine::Sprite* spriteTitleBack_ = nullptr;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;
};
