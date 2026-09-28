#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class MapChipField;
class Enemy;

class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	//ゲッター セッター
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }

	float GetMaxFuel() const { return kMaxFuel; }

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	void SetVelocity(const KamataEngine::Vector3& velocity) { velocity_ = velocity; }

	float GetRadius() const { return kRadius; }

	bool IsDead() const { return isDead_; }
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	KamataEngine::Vector3 GetWorldPosition();

	float GetFuel() const { return fuel_; }
	bool IsOutOfFuel() const { return fuel_ <= 0.0f; }
	void AddFuel(float amount) { fuel_ = (std::min)(fuel_ + amount, kMaxFuel); }

	AABB GetAABB();

void RestoreFuel(float amount);

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void OnCollision(const Enemy* enemy);

private:
	// ワールド変換データ & モデル & カメラ
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	// 物理計算パラメータ
	KamataEngine::Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };
	
static inline const float kForwardSpeed = 0.08f; // 前進スピード
	static inline const float kTurnSpeed = 0.15f;   // 左右の回転スピード
	static inline const float kRadius = 0.5f;       // 当たり判定の半径

	//突進
	static inline const float kMaxFuel = 100.0f;
	static inline const float kFuelConsumption = 0.1f; 
	float fuel_ = kMaxFuel;

	bool isDead_ = false;
};