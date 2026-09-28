#include "GameScene.h"
#include "CameraController.h"
#include "MyMath.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	phase_ = Phase::kFadeIn;
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("./Resources./uvChecker.png");

	// 3Dモデルの生成
	modelBlock_ = Model::CreateFromOBJ("block");
	modelSkydome_ = Model::CreateFromOBJ("SkyDome", true);
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelGoal_ = Model::CreateFromOBJ("goal", true);
	modelAttack_ = Model::CreateFromOBJ("playerAttack", true);
	modelParticle_ = Model::CreateFromOBJ("deathParticle", true);

	textureHandleGraph_ = TextureManager::Load("white1x1.png");

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;
	// マップチップフィールドの初期化
	mapChipField_->ResetMapChipData();

	mapChipField_->LoadMapchipCsv("Resources/blocks.csv", 0);
	mapChipField_->LoadMapchipCsv("Resources/blocks2.csv", 1);

	// 自キャラ生成
	player_ = new Player();

	// 自キャラ座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(9, 10);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 自キャラの初期化
	player_->Initialize(modelPlayer_, &camera_, playerPosition);

	skydome_ = new Skydome();

	player_->SetMapChipField(mapChipField_);

	playerAttack_ = new PlayerAttack();
	playerAttack_->Initialize(modelAttack_, &camera_, player_);

	graphBar_ = new GraphBar();
	graphBar_->Initialize(textureHandleGraph_);

	// カメラの初期化
	camera_.Initialize();

	cameraController_ = new CameraController();

	cameraController_->Initialize();

	cameraController_->SetTarget(player_);
	cameraController_->SetMapChipField(mapChipField_);

	// ゴールの初期化Q
	Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(18, 13); // マップ右端に置く例
	Vector3 goalSize = {1.0f, 1.0f, 1.0f};
	goal_.Initialize(goalPosition, goalSize, modelGoal_);
	// 他の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	GenerateBlocks();

	// カメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void GameScene::GenerateBlocks() {
	uint32_t numVertical = mapChipField_->GetNumBlockVirtical();
	uint32_t numHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numVertical);

	for (uint32_t i = 0; i < numVertical; ++i) {
		worldTransformBlocks_[i].resize(numHorizontal, nullptr);

		for (uint32_t j = 0; j < numHorizontal; ++j) {
			for (uint32_t layer = 0; layer < MapChipField::kNumLayers; ++layer) {
				MapChipType type = mapChipField_->GetMapChipTypeByIndex(i, j, layer);

				// 外枠壁ブロックのみ生成
				if (type == MapChipType::kBlock) {
					WorldTransform* worldTransform = new WorldTransform();
					worldTransform->Initialize();
					Vector3 pos = mapChipField_->GetMapChipPositionByIndex(i, j);
					pos.y = static_cast<float>(layer);
					worldTransform->translation_ = pos;

					worldTransformBlocks_[i][j] = worldTransform;
				}
			}
		}
	}
}

void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラの当たり判定

	AABB aabb1;

	aabb1 = player_->GetAABB();

	if (IsCollision(aabb1, goal_.GetAABB())) {
		finished_ = true;
		isclear_ = true;
	}

#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		/*ゲームプレイフェーズ処理
		if (player_->isDead() == true){
		    phase_ = Phase::kDeath;

		    const Vector3& deathParticlesPosition = player_->GetWorldPosition();
		    deathParticles_ = new DeathParticles;
		    deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);
		} else if (isAllKill_) {
		    phase_ = Phase::kFadeOut;
		    fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		*/

		break;
	case Phase::kDeath:

		phase_ = Phase::kFadeOut;
		fade_->Start(Fade::Status::FadeOut, 1.0f);

		break;

	case Phase::kFadeIn:
		if (fade_->isFinished()) {
			phase_ = Phase::kPlay;
		}
		break;

	case Phase::kFadeOut:
		if (fade_->isFinished()) {
			finished_ = true;
		}
		break;
	};
}

void GameScene::Update() {

	fade_->Update();

	switch (phase_) {
	case Phase::kPlay:
		// goal_.Update();

		CheckAllCollisions();

		enemySpawnTimer_ += 1.0f / 60.0f;
		if (enemySpawnTimer_ >= kSpawnInterval) {
			enemySpawnTimer_ = 0.0f;

			if (Enemy* newEnemy = Enemy::Create(modelEnemy_, &camera_, player_, mapChipField_)) {
				enemies_.push_back(newEnemy);
			}
		}

		break;
	case Phase::kDeath:
		break;
	case Phase::kFadeIn:

		fade_->Update();

		break;
	case Phase::kFadeOut:

		fade_->Update();
		CheckAllCollisions();
		break;
	}

	// 共通の処理
	if (phase_ != Phase::kFadeOut) {
		player_->Update();
	}

	// Skyblock
	skydome_->Update();

	if (KamataEngine::Input::GetInstance()->PushKey(DIK_SPACE)) {
		playerAttack_->StartAttack();
	}

	wasExploding = playerAttack_->IsExploding();

	playerAttack_->Update();

	graphBar_->Update(player_->GetFuel(), player_->GetMaxFuel());

	if (!wasExploding && playerAttack_->IsExploding()) {
		if (particle_) {
			delete particle_;
			particle_ = nullptr;
		}

		particle_ = new Particle();
		particle_->Initialize(modelParticle_, &camera_, playerAttack_->GetWorldPosition());
	}

	// 更新処理 & 演出終了時の破棄
	if (particle_) {
		particle_->Update();

		if (particle_->IsFinished()) {
			delete particle_;
			particle_ = nullptr;
		}
	}
	if (particle_) {
		particle_->Update();
	}

	// カメラコントロール
	cameraController_->Update();

	// デバッグカメラの更新
	debugCamera_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			// アフィン変換行列の作成
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	ChangePhase();

	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の更新と転送
		camera_.TransferMatrix();
		// camera_.UpdateMatrix();
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	if (skydome_) {
		skydome_->Draw();
	}

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 自キャラの描画
	if (player_) {
		player_->Draw();
	}

	if (particle_) {
		particle_->Draw();
	}

	// アタック描画
	if (playerAttack_->IsActive() && !playerAttack_->IsExploding()) {
		playerAttack_->Draw();
	}

	goal_.Draw(&camera_);

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());

	if (graphBar_) {
		graphBar_->Draw();
	}

	Sprite::PostDraw();

	fade_->Draw();
}

GameScene::~GameScene() {
	delete modelBlock_;
	delete debugCamera_;
	delete modelPlayer_;
	delete modelEnemy_;
	delete fade_;
	delete modelSkydome_;
	delete mapChipField_;
	delete particle_;
	delete modelParticle_;
	delete graphBar_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}
