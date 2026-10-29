#include "GraphBar.h"
#include <algorithm>

using namespace KamataEngine;

GraphBar::~GraphBar() {
	delete fuelBarBG_;
	delete fuelBarCurrent_;
}

// ガソリンゲージ用の初期化
void GraphBar::Initialize(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	// 画面左上におく
	fuelBarBG_ = Sprite::Create(textureHandle_, {50, 20});
	fuelBarCurrent_ = Sprite::Create(textureHandle_, {50, 20});
}

// 敵HP用の初期化
void GraphBar::Initialize(uint32_t textureHandle, KamataEngine::Vector2 pos) {
	textureHandle_ = textureHandle;
	fuelBarBG_ = Sprite::Create(textureHandle_, pos);
	fuelBarCurrent_ = Sprite::Create(textureHandle_, pos);
}

void GraphBar::Update(float currentFuel, float maxFuel) {
	// 燃料の割合を計算
	float fuelRate = 0.0f;
	if (maxFuel > 0.0f) {
		fuelRate = std::clamp(currentFuel / maxFuel, 0.0f, 1.0f);
	}

	const float kBarMaxWidth = 200.0f; // ゲージの最大横幅

	// 背景
	fuelBarBG_->SetColor({0.2f, 0.2f, 0.0f, 0.5f});
	fuelBarBG_->SetSize({kBarMaxWidth, 25.0f});

	// 残量（
	fuelBarCurrent_->SetColor({1.0f, 0.6f, 0.0f, 0.9f});
	// 割合に応じて横幅を伸び縮み
	fuelBarCurrent_->SetSize({kBarMaxWidth * fuelRate, 25.0f});
}

void GraphBar::Draw() {
	fuelBarBG_->Draw();
	fuelBarCurrent_	->Draw();
}