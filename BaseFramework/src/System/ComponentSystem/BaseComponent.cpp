#include "main.h"

#include "../GameManager.h"

#include "BaseComponent.h"

// １つ下へ


// １つ上へ

void BaseComponent::MoveUp() {
	if (m_pOwner == nullptr)return;
	auto& compList = m_pOwner->GetComponentList();
	auto it = std::find(compList.begin(), compList.end(), shared_from_this());
	if (it != compList.end() && it != compList.begin()) {
		// 前
		auto itPrev = it;
		--itPrev;
		if (itPrev != compList.end()) {
			std::swap(*it, *itPrev);
		}
	}
}

void BaseComponent::MoveDown() {
	if (m_pOwner == nullptr)return;
	auto& compList = m_pOwner->GetComponentList();
	auto it = std::find(compList.begin(), compList.end(), shared_from_this());
	if (it != compList.end()) {
		// 次
		auto itNext = it;
		++itNext;
		if (itNext != compList.end()) {
			std::swap(*it, *itNext);
		}
	}
}
