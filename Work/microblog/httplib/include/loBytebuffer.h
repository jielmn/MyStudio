#ifndef __lo_BASE_BYTEBUFFER_H__
#define __lo_BASE_BYTEBUFFER_H__

#include <string>
#include "lo/loctypes.h"
#include "lo/locommon.h"

DEFINE_NAMESPACE(locom)

/**
 * 一个字符为单位的简单缓冲区存取对象
 *
*/
class loCOMMON_API CloByteBuffer
{
public:
	CloByteBuffer();
	CloByteBuffer(const char* bytes, size_t len);
	CloByteBuffer(const char* bytes); // uses strlen
	~CloByteBuffer();

	const char* Data() const { return bytes_ + start_; }
	size_t Length() { return end_ - start_; }
	size_t Capacity() { return size_ - start_; }

	bool ReadUInt8(uint8& val);
	bool ReadUInt16(uint16& val);
	bool ReadUInt32(uint32& val);
	bool ReadString(std::string& val, size_t len); // append to val
	bool ReadBytes(char* val, size_t len);

	void WriteUInt8(uint8 val);
	void WriteUInt16(uint16 val);
	void WriteUInt32(uint32 val);
	void WriteString(const std::string& val);
	void WriteBytes(const char* val, size_t len);

	/** add by loach,进行修改*/
	void WriteBytes(size_t index,const char* val,size_t len);

	void Resize(size_t size);
	void Shift(size_t size);
private:
	char* bytes_;
	size_t size_;
	size_t start_;
	size_t end_;
};

END_NAMESPACE(locom)

#endif // __lo_BASE_BYTEBUFFER_H__
