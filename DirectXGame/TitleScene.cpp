#include "TitleScene.h"
#include "MyMath.h"

using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete fade_;
	delete modelPlayer_;
	delete modelRoad_;
	delete modelTitle_;
	delete modelGround_;
	delete spriteTitleBack_;
}

void TitleScene::Initialize() {

	// 3Dモデル
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	modelRoad_ = Model::CreateFromOBJ("titleRoad", true);
	modelTitle_ = Model::CreateFromOBJ("titleFont", true);
	modelGround_ = Model::CreateFromOBJ("titleGround", true);

	textureHandleTitleBack_ = TextureManager::Load("titleBack/titleBack.png");
	spriteTitleBack_ = Sprite::Create(textureHandleTitleBack_, {0.0f, 0.0f});
	spriteTitleBack_->SetSize({1280.0f, 720.0f});

	worldTransform_.Initialize();
	// カメラ初期化
	camera_.Initialize();
	camera_.translation_ = {-1.0f, 1.0f, -9.0f};
	camera_.rotation_ = {0.0f, -0.3f, 3.0f};
	camera_.UpdateMatrix();

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransformPlayer_.TransferMatrix();

	worldTransformTitle_.Initialize();
	worldTransformTitle_.translation_ = {-4.0f, 1.0f, -4.5f};
	worldTransformTitle_.rotation_ = {0.0f, -0.5f, 0.0f};
	worldTransformTitle_.TransferMatrix();

	for (int i = 0; i < 4; ++i) {
		worldTransformRoad_[i].Initialize();
		worldTransformRoad_[i].scale_ = {8.0f, 1.0f, 1.0f};
		worldTransformRoad_[i].translation_ = {0.0f, -1.0f, kRoadLength * i};
		worldTransformRoad_[i].matWorld_ = MakeAffineMatrix(worldTransformRoad_[i].scale_, worldTransformRoad_[i].rotation_, worldTransformRoad_[i].translation_);
		worldTransformRoad_[i].TransferMatrix();
	}

	vibeTimer_ = 0.0f;

	// サウンド
	soundHandleBike_ = Audio::GetInstance()->LoadWave("SE/bikeEngine.wav");
	bikeSoundPlayTimer_ = 0;

	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {

	// 地面を流す
	roadZ_ -= kRoadSpeed;
	if (roadZ_ <= kRoadLoopZ) {
		roadZ_ = kRoadResetZ;
	}

	for (int i = 0; i < 4; ++i) {
		// 手前に流す
		worldTransformRoad_[i].translation_.z -= kRoadSpeed;

		// 手前まで移動したらループ
		if (worldTransformRoad_[i].translation_.z <= -kRoadLength) {
			float maxZ = worldTransformRoad_[0].translation_.z;
			for (int j = 1; j < 4; ++j) {
				if (worldTransformRoad_[j].translation_.z > maxZ) {
					maxZ = worldTransformRoad_[j].translation_.z;
				}
			}

			worldTransformRoad_[i].translation_.z = maxZ + kRoadLength;
		}

		worldTransformRoad_[i].matWorld_ = MakeAffineMatrix(worldTransformRoad_[i].scale_, worldTransformRoad_[i].rotation_, worldTransformRoad_[i].translation_);
		worldTransformRoad_[i].TransferMatrix();
	}

	// バイクの振動
	vibeTimer_ += 0.5f;

	if (static_cast<int>(vibeTimer_) % 6 == 0) {
		vibeY_ = ((std::rand() % 100) / 100.0f - 0.5f) * 0.16f;
	} else {
		vibeY_ = ((std::rand() % 100) / 100.0f - 0.5f) * 0.03f;
	}

	worldTransformPlayer_.translation_.y = 0.0f + vibeY_;

	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_, worldTransformPlayer_.translation_);
	worldTransformPlayer_.TransferMatrix();

	titleTimer_ += 0.05f;

	worldTransformTitle_.translation_.y = 1.0f + (std::sin(titleTimer_) * 0.2f);

	worldTransformTitle_.matWorld_ = MakeAffineMatrix(worldTransformTitle_.scale_, worldTransformTitle_.rotation_, worldTransformTitle_.translation_);
	worldTransformTitle_.TransferMatrix();

	// サウンド
	bikeSoundPlayTimer_++;
	if (bikeSoundPlayTimer_ >= 360) {
		Audio::GetInstance()->PlayWave(soundHandleBike_, false, 0.2f);

		// タイマーをリセット
		bikeSoundPlayTimer_ = 0;
	}

	switch (phase_) {
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kRunAway;
			runAwayTimer_ = 0.0f;
		}
		break;
	case Phase::kRunAway: {
		// 1. 道路は通常のスピードで流れ続ける
		roadZ_ -= kRoadSpeed;
		if (roadZ_ <= kRoadLoopZ) {
			roadZ_ = kRoadResetZ;
		}

		for (int i = 0; i < 4; ++i) {
			worldTransformRoad_[i].translation_.z -= kRoadSpeed;

			if (worldTransformRoad_[i].translation_.z <= -kRoadLength) {
				float maxZ = worldTransformRoad_[0].translation_.z;
				for (int j = 1; j < 4; ++j) {
					if (worldTransformRoad_[j].translation_.z > maxZ) {
						maxZ = worldTransformRoad_[j].translation_.z;
					}
				}
				worldTransformRoad_[i].translation_.z = maxZ + kRoadLength;
			}

			worldTransformRoad_[i].matWorld_ = MakeAffineMatrix(worldTransformRoad_[i].scale_, worldTransformRoad_[i].rotation_, worldTransformRoad_[i].translation_);
			worldTransformRoad_[i].TransferMatrix();
		}

		// 2. バイクは走り続けながら奥へ進む
		worldTransformPlayer_.translation_.z += 0.25f;

		vibeTimer_ += 0.5f;
		if (static_cast<int>(vibeTimer_) % 6 == 0) {
			vibeY_ = ((std::rand() % 100) / 100.0f - 0.5f) * 0.16f;
		} else {
			vibeY_ = ((std::rand() % 100) / 100.0f - 0.5f) * 0.03f;
		}
		worldTransformPlayer_.translation_.y = 0.0f + vibeY_;

		worldTransformPlayer_.matWorld_ = MakeAffineMatrix(worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_, worldTransformPlayer_.translation_);
		worldTransformPlayer_.TransferMatrix();

		// 3. 一定時間経過したらアイリスアウト開始
		runAwayTimer_ += 1.0f;
		if (runAwayTimer_ >= 90.0f) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.5f, Fade::Type::kIrisOut);
		}
	} break;
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->isFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();

		worldTransformPlayer_.translation_.z += 0.25f;

		if (fade_->isFinished()) {
			finished_ = true;
		}
	}
}

void TitleScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	if (spriteTitleBack_) {
		spriteTitleBack_->Draw();
	}
	Sprite::PostDraw();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw(dxCommon->GetCommandList());

	if (modelPlayer_) {
		modelPlayer_->Draw(worldTransformPlayer_, camera_);
	}

	if (modelRoad_) {
		for (int i = 0; i < 4; ++i) {
			modelRoad_->Draw(worldTransformRoad_[i], camera_);
		}
	}

	if (modelTitle_) {
		modelTitle_->Draw(worldTransformTitle_, camera_);
	}

	Model::PostDraw();

	fade_->Draw();
}
