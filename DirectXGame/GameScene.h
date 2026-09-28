#pragma once
#include "CameraController.h"
#include "Enemy.h"
#include "Fade.h"
#include "Goal.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "TitleScene.h"
#include "PlayerAttack.h"
#include "Particle.h"
#include "GraphBar.h"
#include <vector>

class GameScene {

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	bool finished_ = false;

	Goal goal_;

public:
	enum class Phase {
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デストラクタ
	~GameScene();

	// 3Dモデル
	KamataEngine::Model* modelBlock_;

	KamataEngine::Model* modelSkydome_ = nullptr;

	KamataEngine::Model* modelPlayer_ = nullptr;

	KamataEngine::Model* modelEnemy_ = nullptr;

	KamataEngine::Model* modelGoal_ = nullptr;

	KamataEngine::Model* modelAttack_ = nullptr;

	KamataEngine::Model* modelParticle_ = nullptr;

	uint32_t textureHandleGraph_ = 0;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;

	// 自キャラ
	Player* player_ = nullptr;

	PlayerAttack* playerAttack_ = nullptr;

	//敵
	std::vector<Enemy*> enemies_;
	
	// sky
	Skydome* skydome_ = nullptr;

	// パーティクル
	Particle* particle_ = nullptr; 

	// カメラコントローラ
	CameraController* cameraController_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	GraphBar* graphBar_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	float enemySpawnTimer_ = 0.0f;
	static inline const float kSpawnInterval = 2.0f;//出現感覚

	// ゲームプレイから開始
	Phase phase_;

	bool isFinished() const { return finished_; }

	bool isclear_ = false;

	 bool isAllKill_ = false;

	Fade* fade_ = nullptr;

	bool wasExploding = false;
};
