#pragma once

#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"

class Player;
class PlayerAttack;
class MapChipField;

enum class EnemyType { 
	kMoving,//デフォルトの敵
	kDash,   // 突進してくる敵
};

class Enemy {
public:
	/// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, EnemyType type);
	void Update();
	void Draw();

	static Enemy* Create(KamataEngine::Model* model, KamataEngine::Camera* camera, Player* player, MapChipField* mapChipField, EnemyType type);

	// 追従対象のプレイヤーをセット
	void SetPlayer(Player* player) { player_ = player; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// AABBを取得・位置取得
	AABB GetAABB();
	KamataEngine::Vector3 GetWorldPosition();

	// 爆発攻撃に当たった時の処理
	void OnCollisionWithExplosion();

	// ブロックとの衝突判定
	void OnCollisionWithBlocks();

	// 衝突応答
	void OnCollision(const Player* player);

	// デスフラグ確認
	bool isDead() const { return isDead_; }
	// 歩行スピード
	static inline const float kWalkSpeed = 0.03f;
	static inline const float kGroundY = 0.0f; // 接地高さ（地面のy座標）

private:
	// 敵ごとにUpdate関数を持たせる
	void UpdateMoving();
	void UpdateDash();

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	Player* player_ = nullptr;

	const float radius = 0.4f;

	EnemyType state_ = EnemyType::kMoving;
	float circleAngle_ = 0.0f;
	float walkTimer_ = 0.0f;
	KamataEngine::Vector3 velocity_ = {};

	//突進敵
	KamataEngine::Vector3 targetPosition_ = {0.0f,0.0f,0.0f};
	static inline const float kDashSpeed = 0.1f;
	bool isDashing_ = false;
	bool isWaiting_ = false;
	float waitTimer_ = 0.0f;

	// スポーン
	static KamataEngine::Vector3 spawnPosition(const KamataEngine::Vector3& playerPos);
	float angle = 0.0f;

	// ブロックとの衝突判定用
	void CheckMapChipCollisionX();
	void CheckMapChipCollisionZ();
	KamataEngine::Vector3 prevPosition_ = {};

	// デスフラグ
	bool isDead_ = false;
};