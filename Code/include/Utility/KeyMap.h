#pragma once

#include <array>

struct KeyPress
{
	enum class State
	{
		Released,
		Triggered,
		Depressed
	};

	State m_state = State::Released;

	operator bool() const
	{
		return m_state != State::Released;
	}
};

class KeyMap
{
public:
	KeyMap();
	KeyMap(const int* packet);

	void Set(const size_t& index, KeyPress::State);
	const KeyPress& operator[](size_t index) const;
	size_t size();
	void Print() const;
	bool IsDirty() const { return m_dirty; };

	const int* ToPacket() const;
	static constexpr const int GetKeyMapSize() { return 350; };
	static constexpr const int GetPacketArraySize() { return 11; };

private:
	KeyPress m_keyMap[350];
	mutable int m_asPacket[11];
	mutable bool m_dirty = false;
};
