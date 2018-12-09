#ifndef _COLOR_H_
#define _COLOR_H_
#include "glm/vec3.hpp"

glm::vec3 Red() { return glm::vec3(1, 0, 0); }
glm::vec3 Green() { return glm::vec3(0, 1, 0); }
glm::vec3 Blue() { return glm::vec3(0, 0, 1); }
glm::vec3 Yellow() { return Red() + Green(); }
glm::vec3 Teal() { return Blue() + Green(); }
glm::vec3 Purple() { return Blue() + Red(); }
glm::vec3 White() { return glm::vec3(1); }
glm::vec3 Black() { return glm::vec3(0); }

#endif
