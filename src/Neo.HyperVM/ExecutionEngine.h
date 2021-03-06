#pragma once

#include <list>
#include "Types.h"
#include "Limits.h"
#include "StackItems.h"
#include "ExecutionContextStack.h"
#include "EVMState.h"
#include "IStackItemCounter.h"
#include "MapStackItem.h"
#include "ArrayStackItem.h"
#include "BoolStackItem.h"
#include "IntegerStackItem.h"
#include "ByteArrayStackItem.h"
#include "InteropStackItem.h"

class ExecutionEngine : public IStackItemCounter
{
private:

	// Used for MessageCallback

	uint32 _iteration;
	uint32 _consumedGas;
	uint32 _maxGas;

	// Save the state of the execution

	EVMState _state;

	// Callback Interoperability

	OnStepIntoCallback Log;
	GetMessageCallback OnGetMessage;
	LoadScriptCallback OnLoadScript;
	InvokeInteropCallback OnInvokeInterop;

	// Stacks

	std::list<ExecutionScript*> Scripts;

	inline void SetHalt()
	{
		this->_state = EVMState::HALT;
	}

	inline void SetFault()
	{
		this->_state = EVMState::FAULT;
	}

public:

	// Stacks

	StackItems ResultStack;
	ExecutionContextStack InvocationStack;

	// Load scripts

	ExecutionContext* LoadScript(ExecutionScript* script, int32 rvcount);
	int32 LoadScript(byte* script, int32 scriptLength, int32 rvcount);
	bool LoadScript(byte scriptIndex, int32 rvcount);

	inline bool AddGasCost()
	{
		if ((this->_consumedGas += 1) > this->_maxGas)
		{
			this->_state = EVMState::FAULT_BY_GAS;
			return false;
		}

		return true;
	}

	inline bool AddGasCost(uint32 cost)
	{
		if ((this->_consumedGas += cost) > this->_maxGas)
		{
			this->_state = EVMState::FAULT_BY_GAS;
			return false;
		}

		return true;
	}

	// Getters

	inline byte GetState()
	{
		return this->_state;
	}

	inline ExecutionContext* GetCurrentContext()
	{
		return this->InvocationStack.Peek(0);
	}

	inline ExecutionContext* GetCallingContext()
	{
		return this->InvocationStack.Peek(1);
	}

	inline ExecutionContext* GetEntryContext()
	{
		return this->InvocationStack.Peek(-1);
	}

	inline uint32 GetConsumedGas()
	{
		return this->_consumedGas;
	}

	// Setters

	inline void SetLogCallback(OnStepIntoCallback logCallback)
	{
		this->Log = logCallback;
	}

	void Clean(uint32 iteration);

	// Run

	void StepInto();
	void StepOut();
	void StepOver();

	EVMState Execute(uint32 gas);

	// Creators

	inline MapStackItem* CreateMap()
	{
		if (!this->ItemCounterInc())
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		return new MapStackItem(this);
	}

	inline IntegerStackItem* CreateInteger(int32 value)
	{
		if (!this->ItemCounterInc())
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		return new IntegerStackItem(this, value);
	}

	inline IntegerStackItem* CreateInteger(BigInteger *value)
	{
		if (!this->ItemCounterInc())
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		return new IntegerStackItem(this, value);
	}

	inline IntegerStackItem* CreateInteger(byte* data, int32 length)
	{
		if (!this->ItemCounterInc())
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		return new IntegerStackItem(this, data, length);
	}

	inline InteropStackItem* CreateInterop(byte* data, int32 length)
	{
		if (!this->ItemCounterInc())
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		return new InteropStackItem(this, data, length);
	}

	inline ByteArrayStackItem* CreateByteArray(byte* data, int32 length, bool copyPointer)
	{
		if (!this->ItemCounterInc())
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		return new ByteArrayStackItem(this, data, length, copyPointer);
	}

	inline BoolStackItem* CreateBool(bool value)
	{
		if (!this->ItemCounterInc())
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		return new BoolStackItem(this, value);
	}

	inline ArrayStackItem* CreateArray()
	{
		if (!this->ItemCounterInc())
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		return new ArrayStackItem(this);
	}

	inline ArrayStackItem* CreateStruct()
	{
		if (!this->ItemCounterInc())
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		return new ArrayStackItem(this, true);
	}

	inline ArrayStackItem* CreateArray(int32 count)
	{
		if (!this->ItemCounterInc(count + 1))
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		ArrayStackItem * ret = new ArrayStackItem(this, false);

		for (int32 i = 0; i < count; ++i)
		{
			ret->Add(new BoolStackItem(this, false));
		}

		return ret;
	}

	inline ArrayStackItem* CreateStruct(int32 count)
	{
		if (!this->ItemCounterInc(count + 1))
		{
			this->_state = EVMState::FAULT;
			return NULL;
		}

		ArrayStackItem * ret = new ArrayStackItem(this, true);

		for (int32 i = 0; i < count; ++i)
		{
			ret->Add(new BoolStackItem(this, false));
		}

		return ret;
	}

	// Constructor

	ExecutionEngine(InvokeInteropCallback invokeInterop, LoadScriptCallback loadScript, GetMessageCallback getMessage);

	// Destructor

	~ExecutionEngine();
};