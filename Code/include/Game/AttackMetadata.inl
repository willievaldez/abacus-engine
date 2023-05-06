ATTRIBUTE(std::string, sprite, "attack.png");
ATTRIBUTE(float, damage, 5.0f);
ATTRIBUTE(float, cast_cost, 0.0f);
ATTRIBUTE(std::string, type, "Melee");
ATTRIBUTE(float, cast_time, 0.5f);
ATTRIBUTE(bool, creates_light, false);

// ranged attributes only
ATTRIBUTE(float, speed, 0.2f);
ATTRIBUTE(float, radius, 0.0f);
ATTRIBUTE(float, fadeTimeSec, 0.0f);