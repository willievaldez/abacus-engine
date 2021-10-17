#include <Utility/KeyMap.h>

KeyPress& KeyMap::operator[](size_t index)
{
	return m_keyMap[index];
}

const KeyPress& KeyMap::operator[](size_t index) const
{
	return m_keyMap[index];
}

size_t KeyMap::size()
{
	return m_keyMap.size();
}

const char* KeyMap::ToPacket() const
{
	char packet[44] = { 0 };

	for (int i = 0; i < 350; i++)
	{
		int charOffset = i / 8;
		int bitOffset = i % 8;
		int val = m_keyMap[i] ? 1 : 0;
		packet[charOffset] |= val << bitOffset;
	}

	return packet;
}

KeyMap::KeyMap(const char* packet)
{
	for (int i = 0; i < 350; i++)
	{
		int val = (packet[i / 8] >> (i % 8)) & 1;
		m_keyMap[i].m_state = val ? KeyPress::State::Depressed : KeyPress::State::Released;
	}
}