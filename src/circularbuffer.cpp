#include "circularbuffer.h"

CircularBuffer::CircularBuffer(size_t size)
	: buffer(size, 0)
{
}

CircularBuffer::CircularBuffer(const CircularBuffer& copy)
	: buffer(copy.buffer)
{
}

CircularBuffer::CircularBuffer(const CircularBuffer&& move)
	: buffer(std::move(move.buffer))
{
}

CircularBuffer& CircularBuffer::operator=(const CircularBuffer& rhs)
{
	buffer = rhs.buffer;
	return *this;
}

CircularBuffer::CircularBuffer(const char* p, size_t size)
	: buffer(p, p+size)
{
}

void CircularBuffer::assign(const std::vector<char>& data)
{
	if (buffer.size() == data.size())
		buffer = data;
	else throw std::exception("Buffer must be resized in order for different sized data to be assigned to it.");
}

void CircularBuffer::assign(const char* data, size_t size)
{
	if (buffer.size() == size)
	{
		buffer = std::vector<char>(data, data + size);
	}
	else throw std::exception("Buffer must be resized in order for different sized data to be assigned to it.");
}

void CircularBuffer::push(char c)
{
	buffer.erase(buffer.begin());
	buffer.emplace_back(c);
}

void CircularBuffer::clear()
{
	size_t sz = buffer.size();
	buffer.clear();
	buffer.resize(sz, 0);
}

void CircularBuffer::resize(size_t newsize)
{
	if(newsize >= buffer.size())
		buffer.resize(newsize, 0);
	else {
		auto tmp = std::vector<char>(buffer.end() - newsize, buffer.end());
		buffer = tmp;
	}
}

const std::vector<char>& CircularBuffer::get() const
{
	return buffer;
}

const char* CircularBuffer::cdata() const
{
	return buffer.data();
}

char* CircularBuffer::data()
{
	return buffer.data();
}

bool CircularBuffer::equalTo(const std::vector<char> rhs) const
{
	return (buffer.size() == rhs.size()) && std::equal(buffer.begin(), buffer.end(), rhs.begin());
}

bool CircularBuffer::equalTo(const char* rhs) const
{
	/*
		======== WARNING =========
		Unsafe method, does not check the size of rhs array. 
		Use it at your own risk, only if you are sure the size of rhs is 
		equal to the size of the buffer (or bigger)
	*/
	return std::equal(buffer.begin(), buffer.end(), rhs);
}

bool CircularBuffer::operator==(const std::vector<char> rhs) const
{
	return equalTo(rhs);
}

bool CircularBuffer::operator==(const char* rhs) const
{
	/*
		======== WARNING =========
		Unsafe method, does not check the size of rhs array.
		Use it at your own risk, only if you are sure the size of rhs is
		equal to the size of the buffer (or bigger)
	*/
	return equalTo(rhs);
}
