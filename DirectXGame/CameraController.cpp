#include "CameraController.h"
#include "MapChipField.h"
#include "Player.h"

using namespace KamataEngine;

void CameraController::Initialize() {
	camera_.Initialize();


	// 位置と角度をセット
	camera_.translation_ = position_;
	camera_.rotation_ = rotation_;

	// 行列を計算して確定させる
	camera_.UpdateMatrix();
}

void CameraController::Update() {
	if (!target_ || !mapChipField_)
		return;

	// マップ横中央
	float mapCenterX = static_cast<float>(mapChipField_->GetNumBlockHorizontal()) / 2.0f;
	Vector3 targetPos = target_->GetWorldTransform().translation_;

	// 1. カメラの位置（真上・または少し手前から見下ろす）
	camera_.translation_.x = mapCenterX;
	camera_.translation_.y = 12.0f;               // 上空（高さ）に配置
	camera_.translation_.z = targetPos.z - 7.0f; // プレイヤーのZに追従（少し手前に引く）

	// 2. カメラの角度（下を向かせる）
	// X軸回転を 60〜90 度にして見下ろす
	camera_.rotation_.x = 1.0f; // 約57度（90度(1.57f)にすると真上からの見下ろしになります）
	camera_.rotation_.y = 0.0f;
	camera_.rotation_.z = 0.0f;

	camera_.UpdateMatrix();
}