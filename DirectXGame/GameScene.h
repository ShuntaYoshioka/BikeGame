#pragma once
#include "CameraController.h"
#include "Enemy.h"
#include "Fade.h"
#include "GraphBar.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Particle.h"
#include "Player.h"
#include "PlayerAttack.h"
#include "Skydome.h"
#include "TitleScene.h"
#include <vector>

class GameScene {

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	bool finished_ = false;

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

	KamataEngine::Model* modelLord_ = nullptr;

	KamataEngine::Model* modelAttack_ = nullptr;

	KamataEngine::Model* modelParticle_ = nullptr;

	KamataEngine::Model* modelGosign_ = nullptr;

	uint32_t textureHandleGraph_ = 0;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;

	// 自キャラ
	Player* player_ = nullptr;

	PlayerAttack* playerAttack_ = nullptr;

	// 敵
	std::vector<std::unique_ptr<Enemy>> enemies_;

	// sky
	Skydome* skydome_ = nullptr;

	// パーティクル
	Particle* particle_ = nullptr;

	// カメラコントローラ
	CameraController* cameraController_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	GraphBar* graphBar_ = nullptr;

	KamataEngine::WorldTransform worldTransformGoSign_;
	KamataEngine::Vector3 gosignPositionm_;
	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;


	std::vector<std::vector<WorldTransform*>> worldTransformLayer0_;
	std::vector<std::vector<WorldTransform*>> worldTransformLayer1_;

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	// ゲームプレイから開始
	Phase phase_;

	// セーフゾーンに居てゲームが止まってるか

	bool isFinished() const { return finished_; }

private:
	bool isclear_ = false;

	bool isAllKill_ = false;

	Fade* fade_ = nullptr;

	bool wasExploding = false;

	bool hasLeftSafeZone_ = false;

	// 敵
	float enemySpawnTimer_ = 0.0f; // スポーン用タイマー
	const float kEnemySpawnInterval = 3.0f;
	int enemyArmyHp_ = 10; // 総合HP　用は撃破数

	GraphBar* enemyHpBar_ = nullptr;
};
