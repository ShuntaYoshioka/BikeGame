#pragma once

#include "KamataEngine.h"

class GraphBar {
public:
	// デストラクタ
	~GraphBar();

	// 初期化
	void Initialize(uint32_t textureHandle);

	// 更新
	void Update( float currentFuel, float maxFuel);

	// 描画
	void Draw();

private:
	// テクスチャー
	uint32_t textureHandle_;

	// ガソリンゲージ用
	KamataEngine::Sprite* fuelBarBG_ = nullptr;
	KamataEngine::Sprite* fuelBarCurrent_ = nullptr;
};