#pragma once

#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)

#define REGISTER(BASE, TYPE_NAME, CLASS) size_t CONCAT(CONCAT(g_, BASE), __COUNTER__) = BASE::Register<CLASS>(TYPE_NAME)