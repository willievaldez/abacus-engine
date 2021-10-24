#include <Utility/KeyMap.h>

#include <iostream>

namespace 
{
	static const int s_numBits = sizeof(int) * 8;
}

void KeyMap::Set(const size_t& index, KeyPress::State state)
{
	m_keyMap[index].m_state = state;

	int intOffset = index / s_numBits;
	int bitOffset = index % s_numBits;

	if (m_keyMap[index])
	{
		m_asPacket[intOffset] |= 1 << bitOffset;
	}
	else
	{
		m_asPacket[intOffset] &= ~(1 << bitOffset);
	}

	m_dirty = true;
}

const KeyPress& KeyMap::operator[](size_t index) const
{
	return m_keyMap[index];
}

size_t KeyMap::size()
{
	return GetKeyMapSize();
}

void KeyMap::Print() const
{
	for (int i = 0; i < KeyMap::GetKeyMapSize(); i++)
	{
		printf("%i", m_keyMap[i] ? 1 : 0);
	}
	printf("\n\n");
}

const int* KeyMap::ToPacket() const
{
	if (m_dirty)
	{
		m_dirty = false;
	}

	return m_asPacket;
}

KeyMap::KeyMap()
{
	memset(m_asPacket, 0, GetPacketArraySize());
	memset(m_keyMap, 0, GetKeyMapSize());
}

KeyMap::KeyMap(const int* packet)
{
	for (int i = 0; i < GetKeyMapSize(); i++)
	{
		int val = (packet[i / s_numBits] >> (i % s_numBits)) & 1;
		m_keyMap[i].m_state = val ? KeyPress::State::Depressed : KeyPress::State::Released;
	}
}