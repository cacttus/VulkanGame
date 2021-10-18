#include "../base/BaseHeader.h"
#include "./Locale.h"

namespace BR2 {
Locale::Locale() {
}
Locale::~Locale() {
}
void Locale::init() {
  //Common Locale
  std::setlocale(LC_ALL, "");
  wprintf(L"");
}

}  // namespace BR2
