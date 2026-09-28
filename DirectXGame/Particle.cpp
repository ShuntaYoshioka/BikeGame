#include "Particle.h"
#include "MyMath.h"
#include <algorithm>
#include <cstdlib>

using namespace KamataEngine;

// 範囲を指定して float の乱数を返す関数
static float GetRandomFloat(float minVal, float maxVal) {
	float rate = static_cast<float>(std::rand()) / RAND_MAX;
	return minVal + rate * (maxVal - minVal);
}

void Particle::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	camera_ = camera;
	counter_ = 0.0f;
	isFinished_ = false;

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		worldTransforms_[i].Initialize();
		worldTransforms_[i].translation_ = position;

		// 1. 乱数で散らばる方向と速度を決定 (X, Zは全方向、Yは少し上向きに跳ね上がる)
		velocities_[i] = {
		    GetRandomFloat(-0.2f, 0.2f), // X方向の広がり
		    GetRandomFloat(0.05f, 0.3f),  // Y方向 (上への跳ね上がり)
		    GetRandomFloat(-0.2f, 0.2f)  // Z方向の広がり
		};

		// 2. 乱数で回転速度を決定
		rotSpeeds_[i] = {GetRandomFloat(-0.2f, 0.2f), GetRandomFloat(-0.2f, 0.2f), GetRandomFloat(-0.2f, 0.2f)};

		// 3. 乱数で初期サイズを決定
		scale = GetRandomFloat(0.7f, 2.0f);
		worldTransforms_[i].scale_ = {scale, scale, scale};
	}

	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
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

		// 回転処理（成分ごとに加算）
		worldTransforms_[i].rotation_.x += rotSpeeds_[i].x;
		worldTransforms_[i].rotation_.y += rotSpeeds_[i].y;
		worldTransforms_[i].rotation_.z += rotSpeeds_[i].z;
		// 重力演出（少しずつ下に引っ張る）
		velocities_[i].y -= 0.01f;

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