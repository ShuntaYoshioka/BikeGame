#include "Particle.h"
#include "MyMath.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

using namespace KamataEngine;

// 範囲を指定して float の乱数を返す関数
static float GetRandomFloat(float minVal, float maxVal) {
	float rate = static_cast<float>(std::rand()) / RAND_MAX;
	return minVal + rate * (maxVal - minVal);
}

void Particle::Initialize(Model* model, Camera* camera, const Vector3& position, ParticleType type) {
	model_ = model;
	camera_ = camera;
	counter_ = 0.0f;
	isFinished_ = false;
	type_ = type;

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		worldTransforms_[i].Initialize();

		//飛び散る爆発
		if (type == ParticleType::kExplosion) {
			worldTransforms_[i].translation_ = position;

			velocities_[i] = {
			    GetRandomFloat(-0.2f, 0.2f), 
			    GetRandomFloat(0.05f, 0.1f), 
			    GetRandomFloat(-0.2f, 0.2f) 
			};

			rotSpeeds_[i] = {GetRandomFloat(-0.2f, 0.2f), GetRandomFloat(-0.2f, 0.2f), GetRandomFloat(-0.2f, 0.2f)};
			scale = GetRandomFloat(0.7f, 2.0f);
			worldTransforms_[i].scale_ = {scale, scale, scale};

		} else if (type == ParticleType::kSmoke) {
			// 上昇する煙のパーティクル
			worldTransforms_[i].translation_ = {
				position.x + GetRandomFloat(-0.02f, 0.02f),
				position.y + GetRandomFloat(0.0f, 0.05f),
				position.z + GetRandomFloat(-0.02f, 0.02f)
			};

			startPositions_.resize(kNumParticles);
			startPositions_[i] = worldTransforms_[i].translation_;

			velocities_[i] = {0.0f,
				GetRandomFloat(0.0f, 0.2f),
				0.0f
			};

			float startAngle = GetRandomFloat(0.0f, 6.28f);
			float rotSpeed = GetRandomFloat(0.06f, 0.15f);
			float radius = GetRandomFloat(0.2f, 0.4f);

			rotSpeeds_[i] = {startAngle, rotSpeed, radius};
			float s = GetRandomFloat(0.5f, 1.5f);
			worldTransforms_[i].scale_ = {s, s, s};
		} else if (type == ParticleType::kExhaust) {
			worldTransforms_[i].translation_ = position;
			velocities_[i] = {
				position.x + GetRandomFloat(-0.05f, 0.05f),
				position.y + GetRandomFloat(-0.05f, 0.05f),
				position.z + GetRandomFloat(-0.05f, 0.05f)
			};
			rotSpeeds_[i] = {0.0f, 0.0f, 0.0f};
			float s = GetRandomFloat(0.3f, 0.7f);
			worldTransforms_[i].scale_ = {s, s, s};
		}

	}

	objectColor_.Initialize();
	if (type == ParticleType::kSmoke ) {
		color_ = {0.1f, 0.1f, 0.1f, 1.0f};
	} else {
		color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	}
	objectColor_.SetColor(color_);
}

void Particle::Update() {
	if (isFinished_) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 移動処理
		worldTransforms_[i].translation_.x += velocities_[i].x;
		worldTransforms_[i].translation_.y += velocities_[i].y;
		worldTransforms_[i].translation_.z += velocities_[i].z;

		if (type_ == ParticleType::kExplosion) {
			// 爆発：設定した回転スピードでグニャグニャ回転させる
			worldTransforms_[i].rotation_.x += rotSpeeds_[i].x;
			worldTransforms_[i].rotation_.y += rotSpeeds_[i].y;
			worldTransforms_[i].rotation_.z += rotSpeeds_[i].z;

			velocities_[i].y -= 0.01f;
			// 爆発は少しずつ小さくする（またはそのままでも）
			float currentScale = worldTransforms_[i].scale_.x - 0.005f;
			if (currentScale < 0.1f)
				currentScale = 0.1f;
			worldTransforms_[i].scale_ = {currentScale, currentScale, currentScale};

		} else if (type_ == ParticleType::kSmoke) {
			
			rotSpeeds_[i].x += rotSpeeds_[i].y;

			float radius = rotSpeeds_[i].z;
			float cosValue = std::cos(rotSpeeds_[i].x) * radius ;
			float sinValue = std::sin(rotSpeeds_[i].x) * radius;

			worldTransforms_[i].translation_.x = startPositions_[i].x + cosValue ;
			worldTransforms_[i].translation_.z = startPositions_[i].z + sinValue;

			float currentScale = worldTransforms_[i].scale_.x + 0.012f;
			worldTransforms_[i].scale_ = {currentScale, currentScale, currentScale};

		}
		// 行列転送
		worldTransforms_[i].matWorld_ = MakeAffineMatrix(worldTransforms_[i].scale_, worldTransforms_[i].rotation_, worldTransforms_[i].translation_);
		worldTransforms_[i].TransferMatrix();
	}

	// アルファフェードアウト（時間経過で消えていく）
	counter_ += 1.0f / 60.0f;
	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	objectColor_.SetColor(color_);

	// 寿命到達で終了
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}
}

void Particle::Draw() {
	if (isFinished_) {
		return;
	}

	for (WorldTransform& particleTransform : worldTransforms_) {
		model_->Draw(particleTransform, *camera_, &objectColor_);
	}
}