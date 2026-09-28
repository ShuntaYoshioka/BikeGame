#pragma once
#include "KamataEngine.h"

enum class MapChipType { 
	kBlank,
	kBlock,
	klord,

};

struct MapChipData {

	std::vector<std::vector<MapChipType>> data;
};



class MapChipField {
public:

	
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	static inline const uint32_t kNumLayers = 3;

	void ResetMapChipData();
	void LoadMapchipCsv(const std::string& filePath, uint32_t layer = 0);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex, uint32_t layer = 0);
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

	MapChipField::IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	private:

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 22;
	static inline const uint32_t kNumBlockHorizontal = 19;

	MapChipData mapChipData_[kNumLayers];

};
