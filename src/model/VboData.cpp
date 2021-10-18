#include "../model/VboData.h"

namespace BR2 {
VboData::VboData(const string_t& mesh_name, std::shared_ptr<GLContext> ct, size_t iElementSize) : GpuBufferData(mesh_name + "-vbo", ct, GL_ARRAY_BUFFER, iElementSize) {
}
VboData::~VboData() {
}


}//ns br2
