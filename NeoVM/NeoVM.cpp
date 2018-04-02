#include "NeoVM.h"
#include "BoolStackItem.h"
#include "IntegerStackItem.h"
#include "ByteArrayStackItem.h"
#include "InteropStackItem.h"
#include "ArrayStackItem.h"
#include "MapStackItem.h"

#pragma region ExecutionContext

int32 ExecutionContext_GetScriptHash(ExecutionContext* context, byte* output, int32 index)
{
	return context->GetScriptHash(&output[index]);
}

EVMOpCode ExecutionContext_GetNextInstruction(ExecutionContext* context)
{
	return context->GetNextInstruction();
}

#pragma endregion

#pragma region ExecutionEngine

ExecutionEngine * ExecutionEngine_Create
(
	InvokeInteropCallback interopCallback, GetScriptCallback getScriptCallback, GetMessageCallback getMessageCallback,
	ExecutionContextStack* &invStack, StackItems* &evStack, StackItems* &altStack
)
{
	ExecutionEngine* engine = new ExecutionEngine(interopCallback, getScriptCallback, getMessageCallback);

	invStack = engine->GetInvocationStack();
	evStack = engine->GetEvaluationStack();
	altStack = engine->GetAltStack();

	return engine;
}

void ExecutionEngine_Free(ExecutionEngine * & engine)
{
	if (engine == NULL)
		return;

	delete(engine);
	engine = NULL;
}

void ExecutionEngine_LoadScript(ExecutionEngine* engine, byte * script, int32 scriptLength)
{
	engine->LoadScript(script, scriptLength);
}

void ExecutionEngine_LoadPushOnlyScript(ExecutionEngine* engine, byte * script, int32 scriptLength)
{
	engine->LoadPushOnlyScript(script, scriptLength);
}

byte ExecutionEngine_Execute(ExecutionEngine* engine)
{
	return (byte)engine->Execute();
}

void ExecutionEngine_StepInto(ExecutionEngine* engine)
{
	engine->StepInto();
}

void ExecutionEngine_StepOver(ExecutionEngine* engine)
{
	engine->StepOver();
}

void ExecutionEngine_StepOut(ExecutionEngine* engine)
{
	engine->StepOut();
}

int32 ExecutionEngine_GetState(ExecutionEngine* engine)
{
	return engine->GetState();
}

int32 StackItems_Drop(StackItems* stack, int32 count)
{
	int ret = stack->Count();
	ret = ret > count ? count : ret;

	for (int x = 0; x < ret; x++) stack->Drop();
	return ret;
}

#pragma endregion

#pragma region StackItems

IStackItem* StackItems_Pop(StackItems* stack)
{
	if (stack->Count() <= 0) return NULL;

	IStackItem * ret = stack->Pop();
	ret->Claims++;
	return ret;
}

void StackItems_Push(StackItems* stack, IStackItem * item)
{
	stack->Push(item);
}

IStackItem* StackItems_Peek(StackItems* stack, int32 index)
{
	IStackItem * ret = stack->Count() <= index ? NULL : stack->Peek(index);
	if (ret != NULL) ret->Claims++;
	return ret;
}

int32 StackItems_Count(StackItems* stack)
{
	return stack->Count();
}

#pragma endregion

#pragma region ExecutionContextStack

int32 ExecutionContextStack_Drop(ExecutionContextStack* stack, int32 count)
{
	int ret = stack->Count();
	ret = ret > count ? count : ret;

	for (int x = 0; x < ret; x++) stack->Drop();
	return ret;
}

ExecutionContext* ExecutionContextStack_Peek(ExecutionContextStack* stack, int32 index)
{
	return stack->Count() <= index ? NULL : stack->Peek(index);
}

int32 ExecutionContextStack_Count(ExecutionContextStack* stack)
{
	return stack->Count();
}

#pragma endregion

#pragma region StackItem

void StackItem_Free(IStackItem*& item)
{
	IStackItem::Free(item);
}

IStackItem* StackItem_Create(EStackItemType type, byte *data, int32 size)
{
	switch (type)
	{
	default:
	case EStackItemType::None: return NULL;
	case EStackItemType::Bool:
	{
		// https://github.com/neo-project/neo-vm/blob/master/src/neo-vm/StackItem.cs#L37
		// return GetByteArray().Any(p => p != 0);

		if (size == 1)
		{
			IStackItem* it = new BoolStackItem(data[0] != 0x00);
			it->Claims++;
			return it;
		}
		else
		{
			if (size <= 0) return NULL;

			bool ret = false;

			for (int x = 0; x < size; x++)
				if (data[x] != 0x00)
				{
					ret = true;
					break;
				}

			BoolStackItem * it = new BoolStackItem(ret);
			it->Claims++;
			return it;
		}
	}
	case EStackItemType::Integer:
	{
		IntegerStackItem *it = new IntegerStackItem(data, size);
		it->Claims++;
		return it;
	}
	case EStackItemType::ByteArray:
	{
		ByteArrayStackItem *it = new ByteArrayStackItem(data, size, false);
		it->Claims++;
		return it;
	}
	case EStackItemType::Interop:
	{
		InteropStackItem *it = new InteropStackItem(data, size);
		it->Claims++;
		return it;
	}
	case EStackItemType::Array:
	case EStackItemType::Struct:
	{
		ArrayStackItem *it = new ArrayStackItem(type == EStackItemType::Struct);
		it->Claims++;
		return it;
	}
	case EStackItemType::Map:
	{
		MapStackItem *it = new MapStackItem();
		it->Claims++;
		return it;
	}
	}
}

int32 StackItem_SerializeData(IStackItem* item, byte * output, int32 length)
{
	return item->Serialize(output, length);
}

EStackItemType StackItem_SerializeDetails(IStackItem* item, int32 &size)
{
	if (item == NULL)
	{
		size = 0;
		return EStackItemType::None;
	}

	size = item->GetSerializedSize();
	return item->Type;
}

#pragma endregion

#pragma region ArrayStackItem

int32 ArrayStackItem_Count(ArrayStackItem* array)
{
	return array->Count();
}

void ArrayStackItem_Clear(ArrayStackItem* array)
{
	array->Clear();
}

IStackItem* ArrayStackItem_Get(ArrayStackItem* array, int32 index)
{
	IStackItem* ret = array->Get(index);
	if (ret != NULL) ret->Claims++;
	return ret;
}

void ArrayStackItem_Add(ArrayStackItem* array, IStackItem* item)
{
	array->Add(item);
}

void ArrayStackItem_Set(ArrayStackItem* array, int32 index, IStackItem* item)
{
	array->Set(index, item, true);
}

int32 ArrayStackItem_IndexOf(ArrayStackItem* array, IStackItem* item)
{
	return array->IndexOf(item);
}

void ArrayStackItem_Insert(ArrayStackItem* array, int32 index, IStackItem* item)
{
	array->Insert(index, item);
}

void ArrayStackItem_RemoveAt(ArrayStackItem* array, int32 index, byte dispose)
{
	array->RemoveAt(index, dispose == 0x01);
}

#pragma endregion
