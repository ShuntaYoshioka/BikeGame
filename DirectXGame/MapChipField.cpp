#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>

using namespace KamataEngine;

namespace {

std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

void MapChipField::ResetMapChipData() {
	for (uint32_t i = 0; i < kNumLayers; ++i) {
		mapChipData_[i].data.clear();
		mapChipData_[i].data.resize(kNumBlockVirtical);
		for (std::vector<MapChipType>& mapChipDataLine : mapChipData_[i].data) {
			mapChipDataLine.resize(kNumBlockHorizontal, MapChipType::kBlank);
		}
	}
}

void MapChipField::LoadMapchipCsv(const std::string& filePath, uint32_t layer) {
	//assert(layer < kNumLayers);

	// ファイルを開く
	std::ifstream file(filePath);
	//assert(file.is_open());

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	// 固定サイズ（22行 x 19列）でレイヤー指定読み込み
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		if (!std::getline(mapChipCsv, line))
			break;

		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			if (std::getline(line_stream, word, ',')) {
				// 改行コード(\r)があれば除去
				if (!word.empty() && word.back() == '\r') {
					word.pop_back();
				}

				if (mapChipTable.contains(word)) {
					mapChipData_[layer].data[i][j] = mapChipTable[word];
				}
			}
		}
	}
}
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex, uint32_t layer) {
	if (xIndex >= kNumBlockHorizontal || yIndex >= kNumBlockVirtical || layer >= kNumLayers) {
		return MapChipType::kBlank;
	}
	return mapChipData_[layer].data[yIndex][xIndex];
}

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	float x = kBlockWidth * xIndex;
	float y = 0.0f; // 床にするため高さはフラットに固定
	float z = kBlockHeight * (kNumBlockVirtical - 1 - yIndex);

	return {x, y, z};
}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) { 
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2) / kBlockHeight);
	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) { 
	//指定ブロックの中心座標を取得する
	Vector3 center = GetMapChipPositionByIndex(xIndex,yIndex);
	// デフォルトは通常のブロックサイズ
	float w = kBlockWidth;
	float h = kBlockHeight;


	Rect rect;
	rect.left = center.x - w / 2.0f;
	rect.right = center.x + w / 2.0f;
	rect.bottom = center.z - h / 2.0f;
	rect.top = center.z + h / 2.0f;
	return rect; 
}
