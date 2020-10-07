#pragma once

#include <string>

#define INSTALL_DIR std::string("../../")

enum class MovementType {
	Keyboard,
	PointAndClick,
	Controller
};

enum class FrustumType {
	Perspective,
	Orthographic
};

struct Config
{
	bool loaded = false;

	bool useSteam = false;
	bool useVR = false;
	bool is3D = false;
	float tileSize = 2.0f;
	int windowWidth = 1920;
	int windowHeight = 1080;
	int ticksPerSecond = 60;
	MovementType movementType = MovementType::Keyboard;
	FrustumType frustumType = FrustumType::Orthographic;
	std::string level = "testlevel.csv";
};

const Config& GetConfig();
