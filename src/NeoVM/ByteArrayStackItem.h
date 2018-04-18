#pragma once
#include "IStackItem.h"

class ByteArrayStackItem : public IStackItem
{
private:

	int32 PayloadLength;
	byte* Payload;

public:

	// Converters

	bool GetBoolean();
	BigInteger * GetBigInteger();
	bool GetInt32(int32 &ret);
	int32 ReadByteArray(byte * output, int32 sourceIndex, int32 count);
	int32 ReadByteArraySize();
	bool Equals(IStackItem * it);

	// Constructor

	ByteArrayStackItem(byte* data, int32 length, bool copyPointer);

	// Destructor

	~ByteArrayStackItem();

	// Serialize

	int32 Serialize(byte * data, int32 length);
	int32 GetSerializedSize();
};