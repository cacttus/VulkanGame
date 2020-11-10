#include "../base/UnitTests.h"
#include "../base/Gu.h"

// #define GLM_CONFIG_XYZW_ONLY
#include <glm/vec3.hpp>    // glm::vec3
#include <glm/vec4.hpp>    // glm::vec4
#include <glm/mat4x4.hpp>  // glm::mat4

#include "../math/Vec4x.h"
#include "../math/Mat4x.h"

#include <iostream>

namespace VG {
void UnitTests::run(bool waitforkey) {
  UnitTests::mathTest();

  if (waitforkey) {
    std::cin.get();
  }
}
void glmTest() {
  //Evaluating GLM vs BR2.
  uint_fast64_t count = 0;
#ifdef _DEBUG
  count = 10000000;
#else
  count = 10000000000;
#endif
  {
    t_timeval a = Gu::getMilliSeconds();
    glm::vec4 v2 = {0, 0, 0, 0};
    for (size_t i = 0; i < count; ++i) {
      glm::vec4 v = {1, 0, 0, 0};
      glm::mat4 m = glm::mat4(1.0);
      v2 = (v2 + (m * v)) / 2.0f;
    }
    std::cout << "glm (val=" << v2.x << "," << v2.y << "," << v2.z << "," << v2.w << "): " << (Gu::getMilliSeconds() - a) << "ms" << std::endl;
  }
  {
    t_timeval a = Gu::getMilliSeconds();
    vec4 v2(0, 0, 0, 0);
    for (size_t i = 0; i < count; ++i) {
      vec4 v(1, 0, 0, 0);
      mat4 m;
      m.setIdentity();
      v2 = (v2 + (m * v)) / 2.0f;
    }
    std::cout << "br2 (val=" + v2.toString() + "): " << (Gu::getMilliSeconds() - a) << "ms" << std::endl;
  }
}

void UnitTests::mathTest() {
  vec2 v1{0, 1};
  vec3 v2{2, 3, 4};
  vec4 v3{0, 0, 1, 1};

  vec2 v1_a{0, 1};
  vec3 v2_a{2, 3, 4};
  vec4 v3_a{0, 0, 1, 1};

  mat4 m1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  mat4 m2 = mat4::identity();

#define PRINTV(x) std::cout << std::string(#x) <<": "<<  (x) << std::endl;
#define PRINT(x) std::cout << std::string(#x) <<": " << (x).toString(4) << std::endl;

  PRINTV(sin(M_PI_2));
  PRINTV(sin(M_PI/2));
  PRINTV(cos(M_PI_2));
  PRINTV(cos(M_PI/2));

  PRINT(v1 + v1_a);
  PRINT(v2 + v2_a);
  PRINT(v3 + v3_a);
  PRINT(m1 * v3);
  PRINT(m2 * v3);
  std::cout << "Rotate a z=1 vector about the y axis 90 degrees." << std::endl;
  PRINT(mat4::rotation(M_PI / 2 * 1, vec3(0, 1, 0)));
  PRINT(mat4::rotation(M_PI / 2 * 1, vec3(0, 1, 0)) * v3);
  PRINT(mat4::rotation(M_PI / 2 * 2, vec3(0, 1, 0)) * v3);
  PRINT(mat4::rotation(M_PI / 2 * 3, vec3(0, 1, 0)) * v3);
  PRINT(v3);
  PRINT(
    mat4::rotation(M_PI, vec3(1, 0, 0)) *
    mat4::rotation(M_PI, vec3(0, 0, 1)) *
    mat4::rotation(M_PI, vec3(0, 1, 0)) *
     v3);

  std::cout << "Rotate a z=1 vector about the y axis 90 degrees (double)." << std::endl;
  PRINT(Mat4x<double>::rotation(M_PI / 2 * 1, Vec3x<double>(0, 1, 0)));
  PRINT(Mat4x<double>::rotation(M_PI / 2 * 1, Vec3x<double>(0, 1, 0)) * v3);
  PRINT(Mat4x<double>::rotation(M_PI / 2 * 2, Vec3x<double>(0, 1, 0)) * v3);
  PRINT(Mat4x<double>::rotation(M_PI / 2 * 3, Vec3x<double>(0, 1, 0)) * v3);
}

}  // namespace VG