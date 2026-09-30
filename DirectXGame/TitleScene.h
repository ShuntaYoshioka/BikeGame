#pragma once
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

	KamataEngine::Camera camera_;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::WorldTransform worldTransformPlayer_;

	bool isFinished() const { return finished_; }

private:
	bool finished_ = false;

	KamataEngine::Model* modelPlayer_ = nullptr;

	float vibeTimer_ = 0.0f;

	float vibeY_ = 0.0f;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;
};
