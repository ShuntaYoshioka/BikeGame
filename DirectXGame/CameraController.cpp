#include "CameraController.h"
#include "GameScene.h"
#include "MapChipField.h"
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

void CameraController::Initialize() {
	camera_.Initialize();

	// 位置と角度をセット
	camera_.translation_ = position_;
	camera_.rotation_ = rotation_;

	introTimer_ = 0.0f;

	// 行列を計算して確定させる
	camera_.UpdateMatrix();
}

void CameraController::Update() {
	if (!mapChipField_ || !target_)
		return;

	// マップ横中央
	targetPos = target_->GetWorldTransform().translation_;

	if (!hasLeftSafeZone_) {
		introTimer_ += 1.0f / 60.0f;

		t = std::clamp(introTimer_ / 2.0f, 0.0f, 1.0f);
		easeT = 1.0f - (1.0f - t) * (1.0f - t);

		Vector3 lowStartEye = {targetPos.x, 3.0f, targetPos.z - 8.0f};
		Vector3 normalEye = {targetPos.x, 12.0f, targetPos.z - 7.0f};

		camera_.translation_.x = std::lerp(lowStartEye.x, normalEye.x, easeT);
		camera_.translation_.y = std::lerp(lowStartEye.y, normalEye.y, easeT);
		camera_.translation_.z = std::lerp(lowStartEye.z, normalEye.z, easeT);

		lowRotX = 0.3f;
		normalRotX = 1.0f;
		camera_.rotation_.x = std::lerp(lowRotX, normalRotX, easeT);
		camera_.rotation_.y = 0.0f;
		camera_.rotation_.z = 0.0f;
	} else {
		introTimer_ = 0.0f;

		// 1. カメラの位置（真上・または少し手前から見下ろす）
		camera_.translation_.x = targetPos.x;        // プレイヤーのXに追従
		camera_.translation_.y = 12.0f;              // 現在の高さのまま固定
		camera_.translation_.z = targetPos.z - 7.0f; // プレイヤーのZに追従（少し手前に引く）

		// 2. カメラの角度（下を向かせる）
		camera_.rotation_.x = 1.0f;
		camera_.rotation_.y = 0.0f;
		camera_.rotation_.z = 0.0f;
	}

	camera_.UpdateMatrix();
}