#pragma once

#include <array>
#include <set>

class KeyMap
{
public:
	KeyMap();
	KeyMap(const int* packet);

	void Set(const size_t& index, bool pressed);
	const bool& operator[](size_t index) const;
	size_t size();
	void Print() const;
	bool IsDirty() const { return m_dirty; };
	bool IsNull() const { return m_null; };

	const int* ToPacket() const;
	static constexpr const int GetKeyMapSize() { return 350; };
	static constexpr const int GetPacketArraySize() { return 11; };

private:
	bool m_keyMap[350];
	mutable int m_asPacket[11];
	mutable bool m_dirty = false;
	mutable bool m_null = true;

	std::set<int> m_pressedKeys;
};
