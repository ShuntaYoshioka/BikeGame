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
	modelLord_ = Model::CreateFromOBJ("goal", true);
	modelAttack_ = Model::CreateFromOBJ("playerAttack", true);
	modelParticle_ = Model::CreateFromOBJ("deathParticle", true);
	modelGosign_ = Model::CreateFromOBJ("Go",true);

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
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(20, 16);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 自キャラの初期化
	player_->Initialize(modelPlayer_, &camera_, playerPosition);

	skydome_ = new Skydome();

	player_->SetMapChipField(mapChipField_);

	playerAttack_ = new PlayerAttack();

	playerAttack_->Initialize(modelAttack_, &camera_, player_);

	// 敵
	enemies_.clear();        // 前のシーンの敵が残っていればクリア
	enemySpawnTimer_ = 0.0f; // タイマーリセット

	// 燃料ゲージの生成
	graphBar_ = new GraphBar();
	graphBar_->Initialize(textureHandleGraph_);

	// 敵HPゲージの生成
	enemyHpBar_ = new GraphBar();
	enemyHpBar_->Initialize(textureHandleGraph_, {540.0f, 20.0f});

	//Gosign
	worldTransformGoSign_.Initialize();
	gosignPositionm_ = playerPosition;
	gosignPositionm_.y += 1.0f;
	worldTransformGoSign_.translation_ = gosignPositionm_;
	worldTransformGoSign_.scale_ = {1.0f, 1.0f, 1.0f};

	// 3. 行列転送
	worldTransformGoSign_.TransferMatrix();

	// カメラの初期化
	camera_.Initialize();

	cameraController_ = new CameraController();

	cameraController_->Initialize();

	cameraController_->SetTarget(player_);
	cameraController_->SetMapChipField(mapChipField_);

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

	worldTransformLayer0_.resize(numVertical);
	worldTransformLayer1_.resize(numVertical);

	for (uint32_t i = 0; i < numVertical; ++i) {
		worldTransformLayer0_[i].resize(numHorizontal, nullptr);
		worldTransformLayer1_[i].resize(numHorizontal, nullptr);

		for (uint32_t j = 0; j < numHorizontal; ++j) {
			// レイヤー0の生成
			MapChipType type0 = mapChipField_->GetMapChipTypeByIndex(i, j, 0);
			if (type0 != MapChipType::kBlank) {
				WorldTransform* worldTransform0 = new WorldTransform();
				worldTransform0->Initialize();
				Vector3 pos0 = mapChipField_->GetMapChipPositionByIndex(i, j);
				pos0.y = 0.0f;
				worldTransform0->translation_ = pos0;
				worldTransformLayer0_[i][j] = worldTransform0;
			}

			// レイヤー1の生成
			MapChipType type1 = mapChipField_->GetMapChipTypeByIndex(i, j, 1);
			if (type1 != MapChipType::kBlank) {
				WorldTransform* worldTransform1 = new WorldTransform();
				worldTransform1->Initialize();
				Vector3 pos1 = mapChipField_->GetMapChipPositionByIndex(i, j);
				pos1.y = 1.0f;
				worldTransform1->translation_ = pos1;
				worldTransformLayer1_[i][j] = worldTransform1;
			}
		}
	}
}
void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラの当たり判定

	if (player_ && mapChipField_) {
		AABB aabb1;
		aabb1 = player_->GetAABB();
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(player_->GetWorldPosition());
		for (int x = -1; x <= 1; ++x) {
			for (int y = -1; y <= 1; ++y) {
				uint32_t checkX = indexSet.xIndex + x;
				uint32_t checkY = indexSet.yIndex + y;

				// レイヤー確認
				MapChipType type1 = mapChipField_->GetMapChipTypeByIndex(checkX, checkY, 1);

				if (type1 == MapChipType::kBlock) {
					MapChipField::Rect rect = mapChipField_->GetRectByIndex(checkX, checkY);

					bool isCollision = (aabb1.min.x <= rect.right && aabb1.max.x >= rect.left) && (aabb1.min.z <= rect.top && aabb1.max.z >= rect.bottom);

					if (isCollision) {
						player_->OnCollisionWithBlock();
					}
				}
			}
		}
	}

	for (auto& enemy : enemies_) {
		if (enemy->isDead()) {
			continue;
		}

		// プレイヤーの攻撃との当たり判定チェック
		if (playerAttack_ && playerAttack_->IsActive() && !playerAttack_->IsExploding()) {
			AABB enemyAABB = enemy->GetAABB();
			AABB attackAABB = playerAttack_->GetAABB();

			// AABBの交差判定
			if (enemyAABB.min.x <= attackAABB.max.x && enemyAABB.max.x >= attackAABB.min.x && enemyAABB.min.y <= attackAABB.max.y && enemyAABB.max.y >= attackAABB.min.y &&
			    enemyAABB.min.z <= attackAABB.max.z && enemyAABB.max.z >= attackAABB.min.z) {

				// 敵を倒す
				enemy->OnCollisionWithExplosion();

				// 敵軍団のHPを減らす
				enemyArmyHp_--;
			}
		}
	}
	// 倒された敵を除外
	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](const std::unique_ptr<Enemy>& enemy) { return enemy->isDead(); }), enemies_.end());

#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:

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

// 更新処理
void GameScene::Update() {

	fade_->Update();

	switch (phase_) {
	case Phase::kPlay: {

		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(player_->GetWorldPosition());

		//スタートかの判定
		bool isInStartRoom = (indexSet.xIndex >= 16 && indexSet.xIndex < 26 && indexSet.yIndex >= 12 && indexSet.yIndex < 18);

		if (!isInStartRoom) {
			for (auto& enemy : enemies_) {
				if (!enemy->isDead()) {
					enemy->Update();
				}
			}

			hasLeftSafeZone_ = true;
		}

		CheckAllCollisions();

		// 敵の生成
		enemySpawnTimer_ += 1.0f / 60.0f;
		if (enemySpawnTimer_ >= kEnemySpawnInterval) {
			enemySpawnTimer_ = 0.0f;

			auto newEnemy = std::unique_ptr<Enemy>(Enemy::Create(modelEnemy_, &camera_, player_, mapChipField_));
			if (newEnemy) {
				enemies_.push_back(std::move(newEnemy));
			}
		}
		break;
	}
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

		finished_= true;
	}

	wasExploding = playerAttack_->IsExploding();

	playerAttack_->Update();

	graphBar_->Update(player_->GetFuel(), player_->GetMaxFuel());
	enemyHpBar_->Update(static_cast<float>(enemyArmyHp_), 10.0f);

	if (!wasExploding && playerAttack_->IsExploding()) {
		if (particle_) {
			delete particle_;
			particle_ = nullptr;
		}

		particle_ = new Particle();
		particle_->Initialize(modelParticle_, &camera_, playerAttack_->GetWorldPosition());
	}

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
	// レイヤー0（道とブロック）の更新
	for (std::vector<WorldTransform*>& line : worldTransformLayer0_) {
		for (WorldTransform* worldTransformBlock : line) {
			if (!worldTransformBlock)
				continue;
			// アフィン変換行列の作成
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

	// レイヤー1（壁やガソリン等）の更新
	for (std::vector<WorldTransform*>& line : worldTransformLayer1_) {
		for (WorldTransform* worldTransformBlock : line) {
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
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	if (skydome_) {
		skydome_->Draw();
	}

	// ブロックの描画
	// レイヤー0の描画（道）
	for (uint32_t i = 0; i < worldTransformLayer0_.size(); ++i) {
		for (uint32_t j = 0; j < worldTransformLayer0_[i].size(); ++j) {
			WorldTransform* wt = worldTransformLayer0_[i][j];
			if (!wt)
				continue;

			MapChipType type0 = mapChipField_->GetMapChipTypeByIndex(i, j, 0);
			if (type0 == MapChipType::klord) {
				modelLord_->Draw(*wt, camera_);
			} else {
				modelBlock_->Draw(*wt, camera_);
			}
		}
	}

	// レイヤー1の描画（壁、ガソリン補給場所）
	for (uint32_t i = 0; i < worldTransformLayer1_.size(); ++i) {
		for (uint32_t j = 0; j < worldTransformLayer1_[i].size(); ++j) {
			WorldTransform* wt = worldTransformLayer1_[i][j];
			if (!wt)
				continue;

			MapChipType type1 = mapChipField_->GetMapChipTypeByIndex(i, j, 1);
			if (type1 == MapChipType::kBlock) {
				modelBlock_->Draw(*wt, camera_);
			} else {
				modelBlock_->Draw(*wt, camera_);
			}
		}
	}

	// 自キャラの描画
	if (player_) {
		player_->Draw();
	}

	for (auto& enemy : enemies_) {
		enemy->Draw();
	}

	if (particle_) {
		particle_->Draw();
	}

	// アタック描画
	if (playerAttack_->IsActive() && !playerAttack_->IsExploding()) {
		playerAttack_->Draw();
	}

	if (!hasLeftSafeZone_) {
		if (modelGosign_) {
			worldTransformGoSign_.TransferMatrix();
			modelGosign_->Draw(worldTransformGoSign_, camera_);
		}
	}

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());

	if (graphBar_) {
		graphBar_->Draw();
	}

	if (enemyHpBar_) {
		enemyHpBar_->Draw();
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
	delete enemyHpBar_;
	for (std::vector<WorldTransform*>& line : worldTransformLayer0_) {
		for (WorldTransform* wt : line) {
			delete wt;
		}
	}
	worldTransformLayer0_.clear();

	for (std::vector<WorldTransform*>& line : worldTransformLayer1_) {
		for (WorldTransform* wt : line) {
			delete wt;
		}
	}
	worldTransformLayer1_.clear();
	worldTransformBlocks_.clear();
}
