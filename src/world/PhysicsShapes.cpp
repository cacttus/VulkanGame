#include "../base/BinaryFile.h"
#include "../world/PhysicsShapes.h"


namespace BR2 {

SphereShape::SphereShape(VertexBufferPointer& vbPointer, float r) : _fRadius(r) {

}
SphereShape::~SphereShape() {

}
HullShape::HullShape(VertexBufferPointer& vbPointer) {
  Box3f box;
}
HullShape::~HullShape() {

  _vecPoints.resize(0);
}
void PhysicsShape::deserialize(BinaryFile* fb) {
  fb->readBool(_bKinematic);
  fb->readBool(_bDynamic);
}
void HullShape::deserialize(BinaryFile* fb) {
  PhysicsShape::deserialize(fb);
}
void SphereShape::deserialize(BinaryFile* fb) {
  PhysicsShape::deserialize(fb);
}

void PhysicsShape::serialize(BinaryFile* fb) {
  fb->writeBool(std::move(_bKinematic));
  fb->writeBool(std::move(_bDynamic));
}
void HullShape::serialize(BinaryFile* fb) {
  PhysicsShape::deserialize(fb);
}
void SphereShape::serialize(BinaryFile* fb) {
  PhysicsShape::deserialize(fb);
}


}//ns br2
