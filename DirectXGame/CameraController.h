#pragma once
#include "KamataEngine.h"

class Player;
class MapChipField;

class CameraController {
public:
	void Initialize();
	void Update(); // 固定カメラなので行列更新のみ

	const KamataEngine::Camera& GetViewProjection() const { return camera_; }

	void SetTarget(Player* target) { target_ = target; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void SetHasLeftSafeZone(bool flag) { hasLeftSafeZone_ = flag; }
	// ゲームオーバーの画面の揺れ
	void StartShake(float duration, float intensity);

private:
	KamataEngine::Camera camera_;

	Player* target_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	KamataEngine::Vector3 targetPos;

	// カメラの位置 X横 Y高さ Z:-手前
	KamataEngine::Vector3 position_ = {20.0f, 9.0f, -25.0f};

	// 傾け
	KamataEngine::Vector3 rotation_ = {0.0f, 0.0f, 0.0f};

	float introTimer_ = 0.0f;

	float lowRotX = 0.3f;
	float normalRotX = 1.0f;

	float t;
	float easeT;
	bool hasLeftSafeZone_ = false;

	KamataEngine::Vector3 lowStartEye;
	KamataEngine::Vector3 normalEye;

	float shakeTimer_ = 0.0f;
	float shakeIntensity_ = 0.0f;

	float shakeOffsetX;
	float shakeOffsetY;
};