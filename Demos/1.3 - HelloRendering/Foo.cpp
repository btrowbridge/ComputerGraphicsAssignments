#include "pch.h"
#include "Foo.h"

Foo::Foo(int data, int value) :
	mData(data), mValue(new int(value))
{
}

Foo::Foo(const Foo & rhs) :
	mData(rhs.mData), mValue(new int(*(rhs.mValue)))
{
}

Foo& Foo::operator=(const Foo& rhs)
{
	if (this != &rhs)
	{
		delete mValue;

		mData = rhs.mData;
		mValue = new int(*(rhs.mValue));
	}

	return *this;
}

Foo::~Foo()
{
	delete mValue;
}