#include "TitleScene.h"

using namespace KamataEngine;

TitleScene::~TitleScene() { delete fade_; }

void TitleScene::Initialize() {

	// 3Dモデル
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	worldTransform_.Initialize();
	worldTransformPlayer_.Initialize();
	// カメラ初期化
	camera_.Initialize();
	camera_.translation_ = {9.0f, 1.5f, 1.5f};
	camera_.rotation_ = {0.0f, -1.57f, 0.0f};
	camera_.UpdateMatrix();

	worldTransformPlayer_.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransformPlayer_.TransferMatrix();

	vibeTimer_ = 0.0f;

	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {

	vibeTimer_ += 0.5f;

	if (static_cast<int>(vibeTimer_) % 6 == 0) {
		vibeY_ = ((std::rand() % 100) / 100.0f - 0.5f) * 0.16f;
	} else {
		vibeY_ = ((std::rand() % 100) / 100.0f - 0.5f) * 0.03f;
	}
	camera_.translation_.x = 9.0f;
	camera_.translation_.y = 1.5f + vibeY_;
	camera_.translation_.z = 1.5f;
	camera_.UpdateMatrix();

	switch (phase_) {
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 3.0f, Fade::Type::kSmoke);
		}
		break;
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->isFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->isFinished()) {
			finished_ = true;
		}
	}
}

void TitleScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	if (modelPlayer_) {
		modelPlayer_->Draw(worldTransformPlayer_, camera_);
	}
	Model::PostDraw();

	fade_->Draw();
}
