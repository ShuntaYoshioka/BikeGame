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

private:
	KamataEngine::Camera camera_;

	Player* target_ = nullptr;
	MapChipField* mapChipField_ = nullptr;

	//カメラの位置 X横 Y高さ Z:-手前
	KamataEngine::Vector3 position_ = {20.0f, 9.0f, -25.0f};

	//傾ける　ステージを直接動かして転がす風にするならいじる
	KamataEngine::Vector3 rotation_ = {0.0f, 0.0f, 0.0f};
};