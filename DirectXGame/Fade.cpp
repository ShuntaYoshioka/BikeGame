#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

void Fade::Initialize() {
	// テクスチャ読み込み
	textureHandle_ = KamataEngine::TextureManager::Load("white1x1.png");
	bikeTextureHandle_ = KamataEngine::TextureManager::Load("2DBike.png");

	// Fadeのスプライト
	sprite_ = Sprite::Create(textureHandle_, {0, 0});
	sprite_->SetSize(Vector2(1280, 720));
	sprite_->SetColor(Vector4(0, 0, 0, 1));

	// ストライプのスプライト
	stripes_.resize(kStripeCount);
	for (int i = 0; i < kStripeCount; i++) {
		stripes_[i].sprite = Sprite::Create(textureHandle_, {0, 0});
		stripes_[i].sprite->SetSize(Vector2(1280, 720 / kStripeCount));
		stripes_[i].sprite->SetColor(Vector4(0, 0, 0, 1));
		stripes_[i].delay = (kStripeCount - 1 - i) * 0.08f;
	}

	// 煙のスプライト
	bikeSprite_ = Sprite::Create(bikeTextureHandle_, {0.0f, 0.0f});
	bikeSprite_->SetSize(Vector2(128.0f, 64.0f));
	bikeSprite_->SetColor(Vector4(0.2f, 0.2f, 0.2f, 1.0f));

	for (int i = 0; i < 4; i++) {
		irisSprites_[i] = Sprite::Create(textureHandle_, {0, 0});
		irisSprites_[i]->SetColor(Vector4(0, 0, 0, 1)); // 黒色
	}
}

void Fade::Update() {

	counter_ += 1.0f / 60.0f;
	if (counter_ >= duration_) {
		counter_ = duration_;
	}

	switch (status_) {
	case Status::None:
		// 何もしない
		break;
	case Status::FadeIn:
		if (type_ == Type::kStripe) {
			for (int i = 0; i < kStripeCount; ++i) {
				float posY = static_cast<float>(i) * 720 / kStripeCount;

				float activeDuration = duration_ - stripes_[i].delay;
				float progress = 0.0f;
				if (activeDuration > 0.0f) {
					progress = std::clamp((counter_ - stripes_[i].delay) / activeDuration, 0.0f, 1.0f);
				}

				float posX = 1280 * EaseOutCubic(progress);
				stripes_[i].sprite->SetPosition(Vector2(posX, posY));
			}
		} else {
			sprite_->SetColor(Vector4(0, 0, 0, 1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		}
		break;

	case Status::FadeOut:
		if (type_ == Type::kStripe) {
			//ストライプ
			for (int i = 0; i < kStripeCount; ++i) {
				float posY = static_cast<float>(i) * 720 / kStripeCount;

				// 各段のディレイ
				float activeDuration = duration_ - stripes_[i].delay;
				float progress = 0.0f;
				if (activeDuration > 0.0f) {
					progress = std::clamp((counter_ - stripes_[i].delay) / activeDuration, 0.0f, 1.0f);
				}

				float posX = -1280 + (1280 * EaseOutCubic(progress));
				stripes_[i].sprite->SetPosition(Vector2(posX, posY));
			}
		} else if (type_ == Type::kSmoke) {
			// バイクの位置を更新
			float progress = std::clamp(counter_ / duration_, 0.0f, 1.0f);

			// バイクの移動
			bikePos_.x = -150.0f + (1550.0f * progress);
			bikeSprite_->SetPosition(bikePos_);

			// バイク走行中
			if (bikePos_.x > -100.0f && bikePos_.x < 1300.0f) {
				for (int k = 0; k < 3; ++k) {
					Smoke smoke;
					smoke.sprite = KamataEngine::Sprite::Create(textureHandle_, {0.0f, 0.0f});

					float offsetX = -20.0f + static_cast<float>(rand() % 20 - 10);
					float offsetY = 10.0f + static_cast<float>(rand() % 30 - 15);
					smoke.position = KamataEngine::Vector2(bikePos_.x + offsetX, bikePos_.y + offsetY);

					smoke.scale = 0.3f;
					smoke.maxScale = 4.0f + static_cast<float>(rand() % 20) * 0.1f;
					smoke.rotation = static_cast<float>(rand() % 360);
					smoke.rotationSpeed = static_cast<float>((rand() % 100) - 50) * 0.002f;
					smoke.alpha = 1.0f;

					smoke.sprite->SetColor(KamataEngine::Vector4(0.05f, 0.05f, 0.05f, smoke.alpha));
					smokes_.push_back(smoke);
				}
			}

			// 煙
			for (auto& smoke : smokes_) {
				if (smoke.scale < smoke.maxScale) {
					smoke.scale += 0.15f;
				}
				smoke.rotation += smoke.rotationSpeed;

				float size = 100.0f * smoke.scale;
				smoke.sprite->SetSize(KamataEngine::Vector2(size, size));
				smoke.sprite->SetPosition(KamataEngine::Vector2(smoke.position.x - size * 0.5f, smoke.position.y - size * 0.5f));
			}

		} else if (type_ == Type::kIrisOut) {
			//アイリスアウト
			float progress = std::clamp(counter_ / duration_, 0.0f, 1.0f);
			float easeProg = EaseOutCubic(progress);

			// 中央の空いている穴のサイズ
			float holeW = 1280.0f * (1.0f - easeProg);
			float holeH = 720.0f * (1.0f - easeProg);

			// 中央の位置
			float centerX =920.0f;
			float centerY = 360.0f;

			// 穴の位置
			float holeX = centerX - holeW * 0.5f;
			float holeY = centerY - holeH * 0.5f;


			//上幕
			irisSprites_[0]->SetPosition(Vector2(0, 0));
			irisSprites_[0]->SetSize(Vector2(1280, holeY));

			//下幕
			irisSprites_[1]->SetPosition(Vector2(0, holeY + holeH));
			irisSprites_[1]->SetSize(Vector2(1280, 720 - (holeY + holeH)));

			// 左幕
			irisSprites_[2]->SetPosition(Vector2(0, holeY));
			irisSprites_[2]->SetSize(Vector2(holeX, holeH));

			//右幕
			irisSprites_[3]->SetPosition(Vector2(holeX + holeW, holeY));
			irisSprites_[3]->SetSize(Vector2(1280 - (holeX + holeW), holeH));

		} else {
			//徐々に黒くするフェード
			sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		}
		break;
	}
}

void Fade::Draw() {

	if (status_ == Status::None) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());

	if (type_ == Type::kStripe) {
		for (int i = 0; i < kStripeCount; ++i) {
			stripes_[i].sprite->Draw();
		}
	} else if (type_ == Type::kSmoke) {
		// Bikeの描画をする
		for (auto& smoke : smokes_) {
			smoke.sprite->Draw();
		}
	} else if (type_ == Type::kIrisOut) {
		for (int i = 0; i < 4; i++) {
			irisSprites_[i]->Draw();
		}
	} else if (type_ == Type::kFade) {
		sprite_->Draw();
	}

	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration, Type type) {

	status_ = status;
	duration_ = duration;
	type_ = type;
	counter_ = 0.0f;

	for (auto& smoke : smokes_) {
		delete smoke.sprite;
	}

	if (type_ == Type::kSmoke) {
		bikePos_ = Vector2(-150.0f, 360.0f); // 画面サイズ720pxのうち、Y=600（下部）を走行
	}

	smokes_.clear();

	if (type_ == Type::kStripe) {
		float stripeHeight = 720.0f / static_cast<float>(kStripeCount);

		for (int i = 0; i < kStripeCount; ++i) {
			float posY = static_cast<float>(i) * stripeHeight;
			float posX = 0.0f;

			if (status_ == Status::FadeOut) {
				posX = -1280.0f;
			} else if (status_ == Status::FadeIn) {

				posX = 0.0f;
			}

			stripes_[i].sprite->SetPosition(Vector2(posX, posY));
		}
	}

	if (type_ == Type::kIrisOut) {
		// 開始時は画面全体を覆うようにセット
		sprite_->SetSize(Vector2(1280.0f, 720.0f));
		sprite_->SetPosition(Vector2(0.0f, 0.0f));
		sprite_->SetColor(Vector4(0, 0, 0, 1.0f));
	}
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::isFinished() const {

	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}

	return true;
}
