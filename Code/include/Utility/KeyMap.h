#pragma once

#include <array>

struct KeyPress
{
	enum class State
	{
		Triggered,
		Depressed,
		Released
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
	KeyMap() = default;
	KeyMap(const char* packet);

	KeyPress& operator[](size_t index);
	const KeyPress& operator[](size_t index) const;
	size_t size();

	const char* ToPacket() const;

private:
	std::array<KeyPress, 350> m_keyMap;
};
