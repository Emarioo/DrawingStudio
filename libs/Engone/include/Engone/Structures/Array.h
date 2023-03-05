#pragma once

#include "Engone/Utilities/Alloc.h"

namespace engone {

	class Array {
	public:
		Array(uint32 typeSize, uint32 allocType) : m_values(typeSize,allocType) {}
		// calls cleanup
		~Array() { cleanup(); }
		// frees allocations
		void cleanup();

		uint32 size();
		void* data();

		// Item is added to the back.
		// Returns false if allocation failed
		bool add(const void* value, void** outPtr=nullptr);

		bool insert(uint32 index, const void* value);

		// nullptr if unused/invalid index
		void* get(uint32 index);
		void remove(uint32 index);

		bool copy(Array* out);

		// this function does not allow emptySpots to be a Stack.
		// emptySpots needs to be an Array where you can pop middle elements.
		//Value& operator[](uint32_t index) {
		//	if (m_values.used < index)
		//		assert((false, "out of bounds"));
		//	if (m_values.used == index) {
		//		if (m_values.used == m_values.max) {
		//			bool yes = m_values.resize(m_values.max * 2 + 4);
		//			if (!yes) assert((false, "failed allocation"));
		//		}
		//		m_values.used++;
		//		*(m_values.data + index) = {};
		//		return *(m_values.data + index);
		//	}
		//	uint32_t indexInEmpty = -1;
		//	for (int i = 0; i < m_emptySpots.size(); i++) {
		//		uint32_t index2 = m_emptySpots.peek(i);
		//		if (index == index2) {
		//			indexInEmpty = i;
		//			break;
		//		}
		//	}
		//	if(index)
		//}

		// Allocate memory in advance.
		// Method can trim values if less than size
		// count is not in bytes!
		bool reserve(uint32 count) {
			return m_values.resize(count);
		}
		bool resize(uint32 count) {
			if (!m_values.resize(count))
				return false;
			m_values.used = count;
			return true;
		}
	private:
		Memory m_values;
	};

	void TestEngoneArray();
}