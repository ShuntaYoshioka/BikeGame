#pragma once
#include <KamataEngine.h>
#include <array>

class Particle {
public:

enum class ParticleType {
		kExplosion,
		kSmoke,
		kExhaust,
	};

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, ParticleType type =ParticleType::kExplosion);

	// 更新
	void Update();

	// 描画
	void Draw();

	// 終了したかどうか
	bool IsFinished() const { return isFinished_; }

private:
	static inline const uint32_t kNumParticles = 10; // パーティクルの個数
	static inline const float kDuration = 2.0f;     // 生存時間（秒）

	float scale;

	ParticleType type_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;
	std::array<KamataEngine::Vector3, kNumParticles> velocities_; // ランダムな速度
	std::array<KamataEngine::Vector3, kNumParticles> rotSpeeds_;  // 各粒子の移動方向

	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	std::vector<KamataEngine::Vector3> startPositions_;

	float counter_ = 0.0f;
	bool isFinished_ = false;
};