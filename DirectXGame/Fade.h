#pragma once
#include "KamataEngine.h"
class Fade {

public:
	// 1,フェード　2,横から流れてくる 3.アイリスアウト 4.煙
	enum class Type {
		kFade,
		kStripe,
		kIrisOut,
		kSmoke
	};

	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

	struct Stripe {
		KamataEngine::Sprite* sprite = nullptr;
		float delay = 0.0f;
	};

	struct Smoke {
		KamataEngine::Sprite* sprite = nullptr;
		KamataEngine::Vector2 position;
		float speed = 0.0f;
		float scale = 0.5f;
		float maxScale = 6.0f;
		float rotation = 0.0f;
		float rotationSpeed = 0.0f;
		float alpha = 0.0f;
	};

	void Initialize();
	void Update();
	void Draw();

	void Start(Status status, float duration, Type type = Type::kFade);

	void Stop();

	bool isFinished() const;

private:
	uint32_t textureHandle_ = 0;
	uint32_t bikeTextureHandle_ = 0;

	KamataEngine::Sprite* sprite_ = nullptr;

	static inline const int kStripeCount = 6;
	std::vector<Stripe> stripes_;

	//煙
	KamataEngine::Sprite* bikeSprite_ = nullptr;
	KamataEngine::Vector2 bikePos_ = {0.0f, 0.0f};
	std::vector<Smoke> smokes_;

	Type type_ = Type::kFade;
	Status status_ = Status::None;

	float duration_ = 0.0f;
	float counter_ = 0.0f;

	// イージング関数
	float EaseOutCubic(float x) const {
		float t = 1.0f - x;
		return 1.0f - (t * t * t);
	}


	std::array<KamataEngine::Sprite*, 4> irisSprites_;
};
