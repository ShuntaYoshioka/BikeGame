#include "Enemy.h"
#include "MyMath.h"
#include "Player.h"
#include <cmath>
#include <numbers>

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	model_ = model;
	camera_ = camera;
	walkTimer_ = 0.0f;
	isDead_ = false;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// 以前決めた0.8倍サイズ（スケール0.8）にセット
	worldTransform_.scale_ = {0.8f, 0.8f, 0.8f};
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;
}

void Enemy::Update() {
	if (isDead_) {
		return;
	}

	// プレイヤーがセットされている場合、XZ平面でプレイヤーへ追従
	if (player_) {
		KamataEngine::Vector3 playerPos = player_->GetWorldPosition();

		// プレイヤーへの方向ベクトル（XZ平面）
		KamataEngine::Vector3 diff;
		diff.x = playerPos.x - worldTransform_.translation_.x;
		diff.y = 0.0f; // Y軸方向は固定
		diff.z = playerPos.z - worldTransform_.translation_.z;

		// 距離を計算
		float length = std::sqrt(diff.x * diff.x + diff.z * diff.z);

		if (length > 0.001f) {
			// 正規化して移動
			diff.x /= length;
			diff.z /= length;

			worldTransform_.translation_.x += diff.x * kWalkSpeed;
			worldTransform_.translation_.z += diff.z * kWalkSpeed;

			// プレイヤーの方を向く
			worldTransform_.rotation_.y = std::atan2(diff.x, diff.z);
		}
	}

	// 地面の高さに合わせる
	worldTransform_.translation_.y = 1.0f;

	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::OnCollision(const Player* player) { (void)player; }

void Enemy::OnCollision(const PlayerAttack* playerAttack) {
	(void)playerAttack;
	isDead_ = true;
}

KamataEngine::Vector3 Enemy::GetWorldPosition() { return worldTransform_.translation_; }

AABB Enemy::GetAABB() {
	if (isDead_) {
		return {
		    {9999.0f,  9999.0f,  9999.0f },
            {-9999.0f, -9999.0f, -9999.0f}
        };
	}

	KamataEngine::Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {
	    worldPos.x - radius,
	    worldPos.y - radius,
	    worldPos.z - radius,
	};

	aabb.max = {
	    worldPos.x + radius,
	    worldPos.y + radius,
	    worldPos.z + radius,
	};

	return aabb;
}

Enemy* Enemy::Create(KamataEngine::Model* model, KamataEngine::Camera* camera, Player* player, MapChipField* mapChipField) {
	if (!player)
		return nullptr;

	//スポーン位置を計算
	KamataEngine::Vector3 spawnPos = spawnPosition(player->GetWorldPosition());

	Enemy* enemy = new Enemy();
	enemy->Initialize(model, camera, spawnPos);
	enemy->SetPlayer(player);
	enemy->SetMapChipField(mapChipField);

	return enemy;
}

KamataEngine::Vector3 Enemy::spawnPosition(const KamataEngine::Vector3& playerPos) {
	// ランダムな角度 (0 〜 2π)
	float angle = static_cast<float>(rand()) / RAND_MAX * 3.14159265f * 2.0f;
	// プレイヤーからの距離 (12〜18ユニット)
	float distance = 12.0f + static_cast<float>(rand()) / RAND_MAX * 6.0f;

	// スポーン座標を計算 (XZ平面)
	return {playerPos.x + std::cos(angle) * distance, 0.0f, playerPos.z + std::sin(angle) * distance};
}

void Enemy::Draw() {
	if (!isDead_ && model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}