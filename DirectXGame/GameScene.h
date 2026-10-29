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

public:
	enum class Phase { kFadeIn, kPlay, kDeath, kFadeOut, kContinue };

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デストラクタ
	~GameScene();

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	// セーフゾーンに居てゲームが止まってるか

	bool isFinished() const { return finished_; }

	bool IsRestartRequested() const { return isRestartRequested_; }

private:
	// ゲームプレイから開始
	Phase phase_ = Phase::kFadeIn;
	// 3Dモデル
	KamataEngine::Model* modelBlock_;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelEnemy_ = nullptr;
	KamataEngine::Model* modelLord_ = nullptr;
	KamataEngine::Model* modelAttack_ = nullptr;
	KamataEngine::Model* modelParticle_ = nullptr;
	KamataEngine::Model* modelGosign_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandleContinue_ = 0;
	uint32_t textureHandle_ = 0;
	uint32_t textureHandleGraph_ = 0;
	uint32_t textureHandleWhite_ = 0;

	// フラグ
	bool finished_ = false;
	bool isclear_ = false;
	bool isAllKill_ = false;
	// ゲーム開始時にセーフゾーンにいるかどうか
	bool isInStartRoom = false;
	bool wasExploding = false;
	bool hasLeftSafeZone_ = false;
	bool isRestartRequested_ = false;
	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;
	KamataEngine::WorldTransform worldTransformGoSign_;
	KamataEngine::Vector3 gosignPositionm_ = {0.0f, 0.0f, 0.0f};

	// フラッシュ
	std::unique_ptr<KamataEngine::Sprite> flashSprite_ = nullptr;
	float flashTimer_ = 0.0f;

	// 自キャラ
	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<PlayerAttack> playerAttack_ = nullptr;
	std::unique_ptr<Fade> fade_ = nullptr;
	// 敵
	std::vector<std::unique_ptr<Enemy>> enemies_;
	// sky
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// パーティクル
	std::vector<std::unique_ptr<Particle>> particles_;
	// カメラコントローラ
	std::unique_ptr<CameraController> cameraController_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	std::unique_ptr<GraphBar> graphBar_ = nullptr;
	std::unique_ptr<GraphBar> enemyHpBar_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> continueSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> continueBackSprite_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	std::vector<std::vector<WorldTransform*>> worldTransformLayer0_;
	std::vector<std::vector<WorldTransform*>> worldTransformLayer1_;
	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// 敵
	float enemySpawnTimer_ = 0.0f; // スポーン用タイマー
	const float kEnemySpawnInterval = 3.0f;
	int enemyArmyHp_ = 10; // 総合HP　撃破数

	// デス時のタイマー
	float deathTimer_ = 0.0f;
	bool hasSpawnedExplosion_ = false;

	float continueAlphaTimer_ = 0.0f;
	// 消す
	float debugDeathTimer_;
};
