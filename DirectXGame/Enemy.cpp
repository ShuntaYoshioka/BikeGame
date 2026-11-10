#include "Enemy.h"
#include "MyMath.h"
#include "Player.h"
#include <algorithm>
#include <cmath>
#include <numbers>

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, EnemyType type) {
	model_ = model;
	camera_ = camera;
	state_ = type;
	walkTimer_ = 0.0f;
	isDead_ = false;
	prevPosition_ = position;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.8f, 0.8f, 0.8f};
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;
}

void Enemy::Update() {
	if (isDead_) {
		return;
	}

	prevPosition_ = worldTransform_.translation_;

	switch (state_) {
	case EnemyType::kMoving:
		UpdateMoving();
		break;
	case EnemyType::kDash:
		UpdateDash();
		break;
	}

	// 追従

	// 地面の高さに合わせる
	worldTransform_.translation_.y = 1.0f;

	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::OnCollision(const Player* player) { (void)player; }

void Enemy::UpdateMoving() {
	if (player_) {
		KamataEngine::Vector3 playerPos = player_->GetWorldPosition();

		// プレイヤーへの方向ベクトル
		KamataEngine::Vector3 diff;
		diff.x = playerPos.x - worldTransform_.translation_.x;
		diff.y = 0.0f;
		diff.z = playerPos.z - worldTransform_.translation_.z;

		// 距離を計算
		float length = std::sqrt(diff.x * diff.x + diff.z * diff.z);

		if (length > 0.001f) {
			// 正規化して移動
			diff.x /= length;
			diff.z /= length;

			worldTransform_.translation_.x += diff.x * kWalkSpeed;
			CheckMapChipCollisionX(); // X方向の衝突判定

			worldTransform_.translation_.z += diff.z * kWalkSpeed;
			CheckMapChipCollisionZ(); // Z方向の衝突判定

			// プレイヤーの方を向く
			worldTransform_.rotation_.y = std::atan2(diff.x, diff.z);
		}
	}
}

void Enemy::UpdateDash() {

	if (isWaiting_) {
		waitTimer_ += 1.0f / 60.0f;
		if (waitTimer_ > 1.0f) {
			isWaiting_ = false;
		}
		return;
	}

	if (!isDashing_) {
		targetPosition_ = player_->GetWorldPosition();

		KamataEngine::Vector3 dir;
		dir.x = targetPosition_.x - worldTransform_.translation_.x;
		dir.z = targetPosition_.z - worldTransform_.translation_.z;

		if (std::sqrt(dir.x * dir.x + dir.z * dir.z) > 0.001f) {
			worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
		}

		isDashing_ = true;
		dashTimer_ = 0.0f;
	} else {
		dashTimer_ += 1.0f / 60.0f;
		if (dashTimer_ >= 6.5f) { // 秒経過ごとにプレイヤーを追い直す場合
			targetPosition_ = player_->GetWorldPosition();
			dashTimer_ = 0.0f; 

			// 向きも再調整
			KamataEngine::Vector3 dir;
			dir.x = targetPosition_.x - worldTransform_.translation_.x;
			dir.z = targetPosition_.z - worldTransform_.translation_.z;
			if (std::sqrt(dir.x * dir.x + dir.z * dir.z) > 0.001f) {
				worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
			}
		}
		KamataEngine::Vector3 diff;
		diff.x = targetPosition_.x - worldTransform_.translation_.x;
		diff.z = targetPosition_.z - worldTransform_.translation_.z;

		float length = std::sqrt(diff.x * diff.x + diff.z * diff.z);

		if (length < kDashSpeed) {
			worldTransform_.translation_.x = targetPosition_.x;
			worldTransform_.translation_.z = targetPosition_.z;
			isDashing_ = false;
			isWaiting_ = true;
			waitTimer_ = 0.0f;
		} else {
			diff.x /= length;
			diff.z /= length;
			worldTransform_.translation_.x += diff.x * kDashSpeed;
			CheckMapChipCollisionX(); // X方向の衝突判定
			worldTransform_.translation_.z += diff.z * kDashSpeed;
			CheckMapChipCollisionZ(); // Z方向の衝突判定
		}
	}
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

Enemy* Enemy::Create(KamataEngine::Model* model, KamataEngine::Camera* camera, Player* player, MapChipField* mapChipField, EnemyType type) {
	if (!player)
		return nullptr;

	// スポーン位置を計算
	KamataEngine::Vector3 spawnPos = spawnPosition(player->GetWorldPosition(), mapChipField);

	Enemy* enemy = new Enemy();
	enemy->state_ = type;
	enemy->Initialize(model, camera, spawnPos, type);
	enemy->SetPlayer(player);
	enemy->SetMapChipField(mapChipField);

	return enemy;
}

KamataEngine::Vector3 Enemy::spawnPosition(const KamataEngine::Vector3& playerPos, MapChipField* mapChipField) {
	// ランダムな角度
	float angle = static_cast<float>(rand()) / RAND_MAX * 3.14f * 2.0f;
	// プレイヤーからの距離12〜18で、スポーン
	float distance = 12.0f + static_cast<float>(rand()) / RAND_MAX * 6.0f;

	// 仮のスポーン座標を計算
	KamataEngine::Vector3 spawnPos = {playerPos.x + std::cos(angle) * distance, 0.0f, playerPos.z + std::sin(angle) * distance};
	// マップチップフィールドが渡されている場合、範囲内に収める
	if (mapChipField) {
		MapChipField::IndexSet indexSet = mapChipField->GetMapChipIndexSetByPosition(spawnPos);

		uint32_t maxH = mapChipField->GetNumBlockHorizontal();
		uint32_t maxV = mapChipField->GetNumBlockVirtical();

		// マップの端（外側）に出ないようにインデックスをクランプする（余裕を持って端から1〜2マス内側に）
		if (indexSet.xIndex < 2) {
			indexSet.xIndex = 2;
		}
		if (indexSet.xIndex >= maxH - 2) {
			indexSet.xIndex = maxH - 3;
		}
		if (indexSet.yIndex < 2) {
			indexSet.yIndex = 2;
		}
		if (indexSet.yIndex >= maxV - 2) {
			indexSet.yIndex = maxV - 3;
		}

		// クランプした安全なインデックスから正しいワールド座標を再取得する
		spawnPos = mapChipField->GetMapChipPositionByIndex(indexSet.xIndex, indexSet.yIndex);
		spawnPos.y = 0.0f; // 高さを合わせる
	}

	return spawnPos;
}

void Enemy::CheckMapChipCollisionX() {
	if (!mapChipField_) {
		return;
	}
	AABB aabb = GetAABB();
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
	for (int32_t x = -1; x <= 1; ++x) {
		for (int32_t z = -1; z <= 1; ++z) {
			uint32_t checkX = indexSet.xIndex + x;
			uint32_t checkZ = indexSet.yIndex + z;

			MapChipType type = mapChipField_->GetMapChipTypeByIndex(checkX, checkZ, 1);
			if (type == MapChipType::kBlock) {
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(checkX, checkZ);
				bool isCollision = (aabb.min.x <= rect.right && aabb.max.x >= rect.left) && (aabb.min.z <= rect.top && aabb.max.z >= rect.bottom);
				if (isCollision) {
					worldTransform_.translation_.x = prevPosition_.x;
				}
			}
		}
	}
}

void Enemy::CheckMapChipCollisionZ() {
	if (!mapChipField_) {
		return;
	}
	AABB aabb = GetAABB();
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
	for (int32_t x = -1; x <= 1; ++x) {
		for (int32_t z = -1; z <= 1; ++z) {
			uint32_t checkX = indexSet.xIndex + x;
			uint32_t checkZ = indexSet.yIndex + z;

			MapChipType type = mapChipField_->GetMapChipTypeByIndex(checkX, checkZ, 1);
			if (type == MapChipType::kBlock) {
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(checkX, checkZ);
				bool isCollision = (aabb.min.x <= rect.right && aabb.max.x >= rect.left) && (aabb.min.z <= rect.top && aabb.max.z >= rect.bottom);
				if (isCollision) {
					worldTransform_.translation_.z = prevPosition_.z;
				}
			}
		}
	}
}

void Enemy::Draw() {
	if (!isDead_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Enemy::OnCollisionWithExplosion() { isDead_ = true; }

void Enemy::OnCollisionWithBlocks() {
	// 衝突時の処理
	worldTransform_.translation_ = prevPosition_;
}