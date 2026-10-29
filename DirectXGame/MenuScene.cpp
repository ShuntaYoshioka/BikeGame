#include "MenuScene.h"

using namespace KamataEngine;

MenuScene::~MenuScene() { delete fade_; }

void MenuScene::Initialize() {
	// 3Dモデル

	// カメラ初期化
	camera_.Initialize();

	worldTransform_.Initialize();
	worldTransformPlayer_.Initialize();

	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 3.0f, Fade::Type::kStripe);
}

void MenuScene::Update() {
	fade_->Update();

	switch (phase_) {
	case Phase::kMain:

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
		fade_->Start(Fade::Status::FadeOut, 2.0f, Fade::Type::kStripe);
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

void MenuScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	Model::PostDraw();

	fade_->Draw();
}
