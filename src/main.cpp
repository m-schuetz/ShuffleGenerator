
// license: MIT (https://opensource.org/licenses/MIT)
// author: Markus Schütz

#include <iostream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <algorithm>

using std::vector;
using std::cout;
using std::endl;
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

static long long start_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();

double now() {
	auto now = std::chrono::high_resolution_clock::now();
	long long nanosSinceStart = now.time_since_epoch().count() - start_time;

	double secondsSinceStart = double(nanosSinceStart) / 1'000'000'000;

	return secondsSinceStart;
}


///
/// * Incrementally generates a shuffled array in a given range.
/// * Shuffled values are generated on demand using the Fisher-Yates algorithm
///
class ShuffleGenerator {

	vector<uint32_t> indices;

	// max value of uint32_t
	static const uint32_t dval = -1;

	uint32_t current = 0;

	uint32_t n = 0;

public:

	ShuffleGenerator(uint32_t size) {
		n = size;
		indices = vector<uint32_t>(n, dval);
	}

	/// get the next value
	uint32_t getNextValue() {

		if (current >= n) {
			return dval;
		}

		uint32_t index = xorshf96() % (n - current) + current;

		uint32_t a = indices[current];
		uint32_t b = indices[index];

		a = a == dval ? current : a;
		b = b == dval ? index : b;

		indices[current] = b;
		indices[index] = a;

		current++;

		return b;
	}

	/// get the next few values
	vector<uint32_t> getNextValues(int chunkSize) {

		int start = current;
		int end = std::min(current + chunkSize, n);
		int size = end - start;

		vector<uint32_t> values(size);

		for (int i = start; i < end; i++) {
			values[i - start] = getNextValue();
		}

		return std::move(values);
	}

	/// see 
	/// * https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
	/// * https://github.com/raylee/xorshf96
	///
	/// not recommended according to the latter but will use for now until issues arise
	static uint32_t xorshf96(void) {

		static uint32_t x = 123456789, y = 362436069, z = 521288629;

		uint32_t t;
		x ^= x << 16;
		x ^= x >> 5;
		x ^= x << 1;

		t = x;
		x = y;
		y = z;
		z = t ^ x ^ y;

		return z;
	}

};


int main() {

	cout << std::setprecision(3) << std::fixed;

	{
		cout << "===== 01 ====" << endl;
		cout << "Generate 123 values, print them" << endl;

		int n = 123;
		ShuffleGenerator gen(n);

		cout << gen.getNextValue() << endl;
		cout << gen.getNextValue() << endl;

		vector<uint32_t> values = gen.getNextValues(123);

		for (auto value : values) {
			cout << value << ", ";
		}

		// The result will have 121 elements, because we've already 
		// retrieved 2 elements before and there are only 121 elements left.

		cout << endl << endl;
	}


	{
		// Generate shuffle of 7 elements, try retrieve 10 elements. 
		// last 3 will be ShuffleGenerator::dval

		cout << "===== 02 ====" << endl;
		cout << "Generate 7 values, retrieve 10" << endl;

		int n = 7;
		ShuffleGenerator gen(n);

		for (int i = 0; i < 10; i++) {

			auto val = gen.getNextValue();

			cout << i << ": " << val << endl;
		}

		cout << endl;
	}

	{
		// checking performance of generating and retrieving 20 million elements
		cout << "===== 03 ====" << endl;
		cout << "Generate 20M values, shuffle, print first 10" << endl;

		auto start = now();

		int n = 20'000'000;
		ShuffleGenerator gen(n);

		vector<uint32_t> values = gen.getNextValues(n);

		for (int i = 0; i < 10; i++) {
			cout << i << ": " << values[i] << endl;
		}

		auto end = now();
		auto duration = end - start;

		cout << "duration: " << duration << "s" << endl;
		cout << endl;
	}

	{
		// checking performance of generating 20M elements, retrieving 5
		cout << "===== 04 ====" << endl;
		cout << "Generate 20M values, shuffle, retrieve 1M values, print first 10" << endl;

		auto start = now();

		int n = 20'000'000;
		ShuffleGenerator gen(n);

		vector<uint32_t> values = gen.getNextValues(1'000'000);

		for (int i = 0; i < 10; i++) {
			cout << i << ": " << values[i] << endl;
		}

		auto end = now();
		auto duration = end - start;

		cout << "duration: " << duration << "s" << endl;
		cout << endl;
	}

	return 0;
}
