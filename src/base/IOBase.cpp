#include "../base/IOBase.h"

namespace BR2 {
template <>
void IOBase<char>::readVec3(vec3& val) {
  int32_t readSiz;
  readSiz = sizeof(vec3);
  AssertGoodRead(readSiz);

  read((char*)&val, readSiz, readSiz, memsize_max);
}
template <>
void IOBase<char>::readByte(t_byte& val) {
  int32_t readSiz;
  readSiz = sizeof(t_byte);
  AssertGoodRead(readSiz);

  read((char*)&val, readSiz, readSiz, memsize_max);
}
template <>
void IOBase<char>::readInt16(int16_t& val) {
  int32_t readSiz;
  readSiz = sizeof(int16_t);
  AssertGoodRead(readSiz);

  read((char*)&val, readSiz, readSiz, memsize_max);
}
template <>
void IOBase<char>::readInt32(int32_t& val, size_t offset) {
  int32_t readSiz;
  readSiz = sizeof(int32_t);
  AssertGoodRead(readSiz);

  read((char*)&val, readSiz, readSiz, offset);
}
template <>
void IOBase<char>::readInt64(int64_t& val) {
  int32_t readSiz;
  readSiz = sizeof(int64_t);
  AssertGoodRead(readSiz);

  read((char*)&val, readSiz, readSiz, memsize_max);
}
template <>
void IOBase<char>::readUint32(uint32_t& val, size_t offset) {
  int32_t readSiz;
  readSiz = sizeof(uint32_t);
  AssertGoodRead(readSiz);

  read((char*)&val, readSiz, readSiz, offset);
}
template <>
void IOBase<char>::readivec3(ivec3& val) {
  int32_t readSiz;
  readSiz = sizeof(ivec3);
  AssertGoodRead(readSiz);

  read((char*)&val, readSiz, readSiz, memsize_max);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
template <>
void IOBase<char>::writeivec3(ivec3& val) {
  int32_t readSiz;
  readSiz = sizeof(ivec3);
  AssertGoodWrite(readSiz);

  write((char*)&val, readSiz, memsize_max);
}
template <>
void IOBase<char>::writeInt32(int32_t& val, size_t offset) {
  int32_t readSiz;
  readSiz = sizeof(int32_t);
  AssertGoodWrite(readSiz);

  write((char*)&val, readSiz, offset);
}
template <>
void IOBase<char>::writeVec3(vec3& val) {
  int32_t readSiz;
  readSiz = sizeof(vec3);
  AssertGoodWrite(readSiz);

  write((char*)&val, readSiz, memsize_max);
}
template <>
void IOBase<char>::writeByte(t_byte& val) {
  int32_t readSiz;
  readSiz = sizeof(t_byte);
  AssertGoodWrite(readSiz);

  write((char*)&val, readSiz, memsize_max);
}
template <>
void IOBase<char>::writeInt16(int16_t& val) {
  int32_t readSiz;
  readSiz = sizeof(int16_t);
  AssertGoodWrite(readSiz);

  write((char*)&val, readSiz, memsize_max);
}
template <>
void IOBase<char>::writeInt64(int64_t& val) {
  int32_t readSiz;
  readSiz = sizeof(int64_t);
  AssertGoodWrite(readSiz);

  write((char*)&val, readSiz, memsize_max);
}
template <>
void IOBase<char>::writeUint32(uint32_t& val, size_t offset) {
  int32_t readSiz;
  readSiz = sizeof(uint32_t);
  AssertGoodWrite(readSiz);

  write((char*)&val, readSiz, offset);
}
template <>
void IOBase<char>::writeStr(const string_t& str) {
  write(str.c_str(), str.length(), memsize_max);
}
template <>
void IOBase<char>::writeTextLine(const string_t& str) {
  writeStr((str + "\n").c_str());
}










}//ns game
