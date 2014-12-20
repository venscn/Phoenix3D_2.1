/*
*
* �ļ�����	��	PX2OutStream.inl
*
*/

//----------------------------------------------------------------------------
template <typename T>
bool OutStream::Write (T datum)
{
	return mTarget.Write(sizeof(T), &datum);
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WriteW (int numElements, const T* data)
{
	if (!mTarget.Write(sizeof(int), &numElements))
	{
		return false;
	}

	if (numElements > 0)
	{
		return mTarget.Write(sizeof(T), numElements, data);
	}
	return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WriteN (int numElements, const T* data)
{
	if (numElements > 0)
	{
		return mTarget.Write(sizeof(T), numElements, data);
	}
	return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WriteEnum (const T datum)
{
	int value = (int)datum;
	return mTarget.Write(sizeof(int), &value);
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WriteEnumW (int numElements, const T* data)
{
	if (!mTarget.Write(sizeof(T), &numElements))
	{
		return false;
	}

	if (numElements > 0)
	{
		for (int i = 0; i < numElements; ++i)
		{
			if (!WriteEnum(data[i]))
			{
				return false;
			}
		}
	}
	return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WriteEnumN (int numElements, const T* data)
{
	if (numElements > 0)
	{
		for (int i = 0; i < numElements; ++i)
		{
			if (!WriteEnum(data[i]))
			{
				return false;
			}
		}
	}
	return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WritePointer (const T* object)
{
	RegisterMap::iterator iter = mRegistered.find(object);
	if (iter != mRegistered.end())
	{
		unsigned int uniqueID = iter->second;
		mTarget.Write(sizeof(unsigned int), &uniqueID);
		return true;
	}
	return false;
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WritePointerW (int numElements, T* const* objects)
{
	if (!mTarget.Write(sizeof(int), &numElements))
	{
		return false;
	}

	if (numElements > 0)
	{
		for (int i = 0; i < numElements; ++i)
		{
			if (!WritePointer(objects[i]))
			{
				return false;
			}
		}
	}
	return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WritePointerN (int numElements, T* const* objects)
{
	if (numElements > 0)
	{
		for (int i = 0; i < numElements; ++i)
		{
			if (!WritePointer(objects[i]))
			{
				return false;
			}
		}
	}
	return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WritePointer (const Pointer0<T>& object)
{
	RegisterMap::iterator iter = mRegistered.find(object);
	if (iter != mRegistered.end())
	{
		unsigned int uniqueID = iter->second;
		mTarget.Write(sizeof(unsigned int), &uniqueID);
		return true;
	}
	return false;
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WritePointerW (int numElements, const Pointer0<T>* objects)
{
	if (!mTarget.Write(sizeof(int), &numElements))
	{
		return false;
	}

	if (numElements > 0)
	{
		for (int i = 0; i < numElements; ++i)
		{
			if (!WritePointer(objects[i]))
			{
				return false;
			}
		}
	}
	return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool OutStream::WritePointerN (int numElements, const Pointer0<T>* objects)
{
	if (numElements > 0)
	{
		for (int i = 0; i < numElements; ++i)
		{
			if (!WritePointer(objects[i]))
			{
				return false;
			}
		}
	}
	return true;
}
//----------------------------------------------------------------------------
template <typename T>
void OutStream::Register (const T* object, bool shareptr)
{
	if (object)
	{
		if(shareptr) RegisterRoot(object, true);
		else object->Register(*this);
	}
}
//----------------------------------------------------------------------------
template <typename T>
void OutStream::Register (int numElements, T* const* objects)
{
	for (int i = 0; i < numElements; ++i)
	{
		Register(objects[i]);
	}
}
//----------------------------------------------------------------------------
template <typename T>
void OutStream::Register (const Pointer0<T>& object, bool shareptr)
{
	if (object)
	{
		if(shareptr) RegisterRoot(object, true);
		else object->Register(*this);
	}
}
//----------------------------------------------------------------------------
template <typename T>
void OutStream::Register (int numElements, Pointer0<T> const* objects)
{
	for (int i = 0; i < numElements; ++i)
	{
		Register(objects[i]);
	}
}
//----------------------------------------------------------------------------