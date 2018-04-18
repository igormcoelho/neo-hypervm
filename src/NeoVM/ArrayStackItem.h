#pragma once
#include "IStackItem.h"
#include <list>

class ArrayStackItem : public IStackItem
{
private:

	std::list<IStackItem*> List;

public:

	// Converters

	bool GetBoolean();
	void Reverse();
	BigInteger * GetBigInteger();
	bool GetInt32(int32 &ret);
	IStackItem* Clone();
	int32 ReadByteArray(byte * output, int32 sourceIndex, int32 count);
	int32 ReadByteArraySize();
	bool Equals(IStackItem * it);

	int32 Count();
	void Clear();
	IStackItem* Get(int32 index);
	void Add(IStackItem* item);
	void Set(int32 index, IStackItem* item);
	void Insert(int32 index, IStackItem* item);
	void RemoveAt(int32 index);
	int32 IndexOf(IStackItem* item);

	// Constructor

	ArrayStackItem(bool isStruct);
	ArrayStackItem(bool isStruct, int32 count);

	// Destructor

	~ArrayStackItem();

	// Serialize

	int32 Serialize(byte * data, int32 length);
	int32 GetSerializedSize();
};