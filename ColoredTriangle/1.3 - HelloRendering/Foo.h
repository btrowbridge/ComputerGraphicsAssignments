#pragma once
class Foo
{
public:
	Foo(int data = 0, int value = 0);
	Foo(const Foo& rhs);
	Foo& operator=(const Foo& rhs);
	~Foo();

private:
	int mData;
	int* mValue;
};

