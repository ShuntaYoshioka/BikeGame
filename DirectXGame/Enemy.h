#pragma once

#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"

class Player;
class PlayerAttack;
class MapChipField;

enum class EnemyState { kMoving, kCircling };

class Enemy {
public:
	/// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	static Enemy* Create(KamataEngine::Model* model, KamataEngine::Camera* camera, Player* player, MapChipField* mapChipField);

	// 追従対象のプレイヤーをセット
	void SetPlayer(Player* player) { player_ = player; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// AABBを取得・位置取得
	AABB GetAABB();
	KamataEngine::Vector3 GetWorldPosition();

	// 爆発攻撃に当たった時の処理
	void OnCollisionWithExplosion() { isDead_ = true; }

	// 衝突応答（従来のもの）
	void OnCollision(const Player* player);
	void OnCollision(const PlayerAttack* playerAttack);

	// デスフラグ確認
	bool isDead() const { return isDead_; }

	//スポーン
	static KamataEngine::Vector3 spawnPosition(const KamataEngine::Vector3& playerPos);

	// 歩行スピード
	static inline const float kWalkSpeed = 0.03f;
	static inline const float kGravity = 0.05f; // 重力加速度
	static inline const float kGroundY = 0.0f;  // 接地高さ（地面のy座標）

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	Player* player_ = nullptr; 

	const float radius = 0.4f;

	EnemyState state_ = EnemyState::kMoving;
	float circleAngle_ = 0.0f;
	float walkTimer_ = 0.0f;
	KamataEngine::Vector3 velocity_ = {};

	// デスフラグ
	bool isDead_ = false;
};