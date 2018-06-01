#include "loBytebuffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

USE_NAMESPACE(locom)

static const int DEFAULT_SIZE = 2048;

CloByteBuffer::CloByteBuffer()
{
	start_ = 0;
	end_   = 0;
	size_  = DEFAULT_SIZE;
	bytes_ = new char[size_];
}

CloByteBuffer::CloByteBuffer(const char* bytes, size_t len) {
	start_ = 0;
	end_   = len;
	size_  = len;
	bytes_ = new char[size_];
	memcpy(bytes_, bytes, end_);
}

CloByteBuffer::CloByteBuffer(const char* bytes) {
	start_ = 0;
	end_   = strlen(bytes);
	size_  = end_;
	bytes_ = new char[size_];
	memcpy(bytes_, bytes, end_);
}

CloByteBuffer::~CloByteBuffer() {
	delete[] bytes_;
}

bool CloByteBuffer::ReadUInt8(uint8& val) {
	return ReadBytes(reinterpret_cast<char*>(&val), 1);
}

bool CloByteBuffer::ReadUInt16(uint16& v) {
	if (!ReadBytes(reinterpret_cast<char*>(&v), 2)) {
		return false;
	}
	return true;
}

bool CloByteBuffer::ReadUInt32(uint32& val) {
	if (!ReadBytes(reinterpret_cast<char*>(&val), 4)) {
		return false;
	}
	return true;
}

bool CloByteBuffer::ReadString(std::string& val, size_t len) {
	if (len > Length()) {
		return false;
	} else {
		val.append(bytes_ + start_, len);
		start_ += len;
		return true;
	}
}

bool CloByteBuffer::ReadBytes(char* val, size_t len) {
	if (len > Length()) {
		return false;
	} else {
		memcpy(val, bytes_ + start_, len);
		start_ += len;
		return true;
	}
}

void CloByteBuffer::WriteUInt8(uint8 val) {
	WriteBytes(reinterpret_cast<const char*>(&val), 1);
}

void CloByteBuffer::WriteUInt16(uint16 val) {
	WriteBytes(reinterpret_cast<const char*>(&val), 2);
}

void CloByteBuffer::WriteUInt32(uint32 val) {
	WriteBytes(reinterpret_cast<const char*>(&val), 4);
}

void CloByteBuffer::WriteString(const std::string& val) {
	WriteBytes(val.c_str(), val.size());
}

void CloByteBuffer::WriteBytes(const char* val, size_t len) {
	if (Length() + len > Capacity())
		Resize(Length() + len);

	memcpy(bytes_ + end_, val, len);
	end_ += len;
}

void CloByteBuffer::WriteBytes(size_t index,const char* val,size_t len)
{
	if( index >= end_ )
	{
		WriteBytes(val,len);
	}
	else if( index >= 0 )
	{
		if( (index + len) <= end_ )
		{
			memcpy(bytes_ + index, val, len);
		}
		else
		{
			size_t l = len - (end_- index );
			if (Length() + l > Capacity())
				Resize(Length() + l);

			memcpy(bytes_ + index, val, len);
			end_ += l;
		}
	}
}

void CloByteBuffer::Resize(size_t size) {
	if (size > size_)
		size = MAX_(size, 3 * size_ / 2);

	size_t len = MIN_(end_ - start_, size);
	char* new_bytes = new char[size];
	memcpy(new_bytes, bytes_ + start_, len);
	delete [] bytes_;

	start_ = 0;
	end_   = len;
	size_  = size;
	bytes_ = new_bytes;
}

void CloByteBuffer::Shift(size_t size) {
	if (size > Length())
		return;

	end_ = Length() - size;
	memmove(bytes_, bytes_ + start_ + size, end_);
	start_ = 0;
}
