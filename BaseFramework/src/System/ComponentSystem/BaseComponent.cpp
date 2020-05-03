#include "main.h"

#include "../GameManager.h"

#include "BaseComponent.h"

// ‚P‚Â‰º‚Ö


// ‚P‚Âã‚Ö

void BaseComponent::MoveUp() {
	if (m_pOwner == nullptr)return;
	auto& compList = m_pOwner->GetComponentList();
	auto it = std::find(compList.begin(), compList.end(), shared_from_this());
	if (it != compList.end() && it != compList.begin()) {
		// ‘O
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
		// ŽŸ
		auto itNext = it;
		++itNext;
		if (itNext != compList.end()) {
			std::swap(*it, *itNext);
		}
	}
}
