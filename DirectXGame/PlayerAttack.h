#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class Enemy;

class Player;

class PlayerAttack {
public:
	/// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, Player* player);
	void Update();
	void StartAttack();
	void Draw();
	 // 有効かどうか
	bool IsActive() const { return isActive_; }

	void SetDirection(const KamataEngine::Vector3& dir) { direction_ = dir; }

	KamataEngine::Vector3 velocity_ = {};
	// AABBを取得
	AABB GetAABB();
	KamataEngine::Vector3 GetWorldPosition();

	void OnCollision();

	bool IsExploding() const { return isExploding_; }

private:


	KamataEngine::Vector3 direction_{0, 0, 1};    // 攻撃方向ベクトル
	KamataEngine::WorldTransform worldTransform_; // ワールドトランスふぉーむ
	KamataEngine::Model* model_ = nullptr;        // モデル
	KamataEngine::Camera* camera_ = nullptr;      // カメラ
	Player* player_ = nullptr;

	// 攻撃のON/OFF
	bool isActive_ = false;

	// 生存時間管理
	float timer_ = 0.0f;

	// 定数
	//爆弾
	bool isExploding_ = false;

	static inline const float kFuseTime = 1.5f;      // 設置から爆発までの時間（秒）
	static inline const float kExplodeTime = 0.4f;   // 爆発の持続時間（秒）
	static inline const float kSize = 1.0f;          // 爆弾のサイズ
	static inline const float kExplosionSize = 2.5f; // 爆発の判定サイズ

	static inline const float kRequiredFuel = 20.0f; // 使う燃料の量
	static inline const float kCoolTime = 1.0f;
	float coolTimer_ = 0.0f;
};
