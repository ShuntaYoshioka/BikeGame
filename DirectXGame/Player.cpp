#include "Player.h"
#include "MapChipField.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <windows.h>

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {kRadius, kRadius, kRadius};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// KamataEngineでの行列転送
	worldTransform_.TransferMatrix();

	velocity_ = {0.0f, 0.0f, 0.0f};
	fuel_ = kMaxFuel;
	isDead_ = false;
}

void Player::Update() {

	if (isDead_) {
		return;
	}

	prevPosition_ = worldTransform_.translation_;

	currentForwardSpeed_ = kForwardSpeed;

	// マップチップの種類を取得し応じた処理を行う
	if (mapChipField_) {
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);

		// レイヤー0とレイヤー1の両方を確認
		MapChipType typeLayer0 = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex, 1);
		MapChipType typeLayer1 = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex, 0);

		if (typeLayer0 == MapChipType::kFuel) {
			RestoreFuel(0.5f);
		}

		if (typeLayer1 == MapChipType::klord) {
			currentForwardSpeed_ = kRoadSpeed;
		}
	}

	// 前後同時押しを考慮した移動処理
	if (fuel_ > 0.0f) {
		Input* input = Input::GetInstance();

		isMovingForward = input->PushKey(DIK_W);
		isMovingBackward = input->PushKey(DIK_S);

		if (isMovingForward && !isMovingBackward) {
			fuel_ -= kFuelConsumption;
			if (fuel_ <= 0.0f) {
				fuel_ = 0.0f;
			}

			worldTransform_.translation_.x += velocity_.x;
			worldTransform_.translation_.z += velocity_.z;

			if (input->PushKey(DIK_A)) {
				worldTransform_.rotation_.y -= kTurnSpeed;
			}
			if (input->PushKey(DIK_D)) {
				worldTransform_.rotation_.y += kTurnSpeed;
			}
		} else if (isMovingBackward && !isMovingForward) {
			fuel_ -= kFuelConsumption;
			if (fuel_ <= 0.0f) {
				fuel_ = 0.0f;
			}

			worldTransform_.translation_.x -= velocity_.x;
			worldTransform_.translation_.z -= velocity_.z;

			if (input->PushKey(DIK_A)) {
				worldTransform_.rotation_.y += kTurnSpeed;
			}
			if (input->PushKey(DIK_D)) {
				worldTransform_.rotation_.y -= kTurnSpeed;
			}
		}
		velocity_.x = std::sin(worldTransform_.rotation_.y) * currentForwardSpeed_;
		velocity_.z = std::cos(worldTransform_.rotation_.y) * currentForwardSpeed_;

		worldTransform_.translation_.y = 1.0f;

		worldTransform_.translation_.x = (std::clamp)(worldTransform_.translation_.x, 1.0f, 78.0f);

		worldTransform_.translation_.z = (std::clamp)(worldTransform_.translation_.z, 1.0f, 78.0f);

	} else {
		velocity_ = {0.0f, 0.0f, 0.0f};
	}
	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

// 描画処理
void Player::Draw() {
	if (isVisible_ && model_ && camera_) {
			model_->Draw(worldTransform_, *camera_);
		}
}

AABB Player::GetAABB() {
	AABB aabb;
	// 球体の中心座標から、半径 kRadius 分の立方体AABBを作る
	aabb.min.x = worldTransform_.translation_.x - kRadius;
	aabb.min.y = worldTransform_.translation_.y - kRadius;
	aabb.min.z = worldTransform_.translation_.z - kRadius;

	aabb.max.x = worldTransform_.translation_.x + kRadius;
	aabb.max.y = worldTransform_.translation_.y + kRadius;
	aabb.max.z = worldTransform_.translation_.z + kRadius;
	return aabb;
}

// 燃料回復
void Player::RestoreFuel(float amount) {
	fuel_ += amount;
	if (fuel_ > kMaxFuel) {
		fuel_ = kMaxFuel; // 最大値は超えない
	}
}

void Player::OnCollisionWithBlock() { worldTransform_.translation_ = prevPosition_; }

void Player::OnCollisionWithEnemy(const Enemy* enemy) {
	if (isDead_) {
		return;
	}
	(void)enemy;

	isDead_ = true;
	// 止める
	velocity_ = {0.0f, 0.0f, 0.0f};
}
