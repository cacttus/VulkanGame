// #include "../gfx/GLObject.h"
// #include "../core/opengl/GLContext.h"
// #include "../base/Gu.h"
// 
// namespace VG {
// GLObject::GLObject(GLenum type) {
//   _type = type;
// }
// string_t GLObject::getObjectName() {
//   return Gu::getCoreContext()->getObjectLabel(_type, _glId);
// }
// void GLObject::getObjectName(string_t name) {
//   Gu::getCoreContext()->setObjectLabel(_type, _glId);
// }
// //
// // private:
// // GLenum _type;
// // string_t _name;
// 
// }  // namespace VG