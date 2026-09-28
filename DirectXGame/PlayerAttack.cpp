#include "PlayerAttack.h"
#include "Player.h"
#include <numbers>

void PlayerAttack::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, Player* player) {
	model_ = model;
	camera_ = camera;
	player_ = player;

	worldTransform_.Initialize();
	isActive_ = false;
	timer_ = 0.0f;
}

void PlayerAttack::StartAttack() {

	if (!player_ || isActive_ || coolTimer_ > 0.0f) {
		return;
	}

	if(player_->GetFuel() < kRequiredFuel) {
		return; // ガソリン不足なら設置不可
	}

	player_->RestoreFuel(-kRequiredFuel);

	coolTimer_ = kCoolTime;

	isActive_ = true;
	isExploding_ = false;
	timer_ = 0.0f;

	// 設置
	KamataEngine::Vector3 p = player_->GetWorldPosition();
	worldTransform_.translation_ = p;
	worldTransform_.scale_ = {kSize, kSize, kSize};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
}

void PlayerAttack::Update() {

	if (coolTimer_ > 0.0f) {
		coolTimer_ -= 1.0f / 60.0f;
		if (coolTimer_ < 0.0f) {
			coolTimer_ = 0.0f;
		}
	}

	if (!isActive_)
		return;

	timer_ += 1.0f / 60.0f;
	if (!isExploding_) {
		if (timer_ >= kFuseTime) {
			// 時間経過で爆発状態に移行
			isExploding_ = true;
			timer_ = 0.0f;
			// 爆発サイズに拡大
			worldTransform_.scale_ = {kExplosionSize, kExplosionSize, kExplosionSize};
		}
	}
	// 2. 爆発中：判定を発生させて消去
	else {
		if (timer_ >= kExplodeTime) {
			isActive_ = false;
			isExploding_ = false;
		}
	}

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

void PlayerAttack::Draw() {
	if (isActive_)
		model_->Draw(worldTransform_, *camera_);
}

void PlayerAttack::OnCollision() { isActive_ = false; }

KamataEngine::Vector3 PlayerAttack::GetWorldPosition() { return {worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]}; }

AABB PlayerAttack::GetAABB() {
	if (!isExploding_) {
		return AABB{
		    {0, 0, 0},
            {0, 0, 0}
        };
	}

	KamataEngine::Vector3 p = GetWorldPosition();
	AABB aabb;
	aabb.min = {p.x - kSize / 2.0f, p.y - kSize / 2.0f, p.z - kSize / 2.0f};
	aabb.max = {p.x + kSize / 2.0f, p.y + kSize / 2.0f, p.z + kSize / 2.0f};
	return aabb;
}
