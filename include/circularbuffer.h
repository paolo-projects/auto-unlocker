#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <vector>

class CircularBuffer
{
public:
	CircularBuffer(size_t size);
	CircularBuffer(const CircularBuffer& copy);
	CircularBuffer(const CircularBuffer&& move);
	CircularBuffer& operator= (const CircularBuffer& rhs);
	CircularBuffer(const char* p, size_t size);
	void assign(const std::vector<char>& data);
	void assign(const char* data, size_t size);
	void push(char c);
	void clear();
	void resize(size_t newsize);
	const std::vector<char>& get() const;
	const char* cdata() const;
	char* data();
	bool equalTo(const std::vector<char> rhs) const;
	bool equalTo(const char* rhs) const;
	bool operator== (const std::vector<char> rhs) const;
	bool operator== (const char* rhs) const;
private:
	std::vector<char> buffer;
};

#endif // CIRCULARBUFFER_H