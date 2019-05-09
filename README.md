
# ShuffleGenerator

* Incrementally generate a shuffled version of a range array, e.g. shuffle([0, 1, 2, ..., n-1]).
* Resulting numbers can be used as indices to shuffle huge data sets, one batch at a time.
* In-core, requires roughly numberOfElements * 4 bytes of memory.

# Usage

```cpp
// Tell ShuffleGenerator we want to shuffle array [0, 1, 2, ..., n - 1]
// The values are not yet generated, but memory will be reserved.
int n = 20'000'000;
ShuffleGenerator gen(n);

// Generate the first 1 million values of the shuffled array.
// Shuffling is done incrementally using Fisher-Yates[1]
vector<uint32_t> values = gen.getNextValues(1'000'000);

// Generate the next 500k values after the first million.
// Shuffling is done incrementally using Fisher-Yates[1]
vector<uint32_t> values2 = gen.getNextValues(500'000);

// Generate the next two values after the first 1.5 million.
uint32_t a = gen.getNextValue();
uint32_t b = gen.getNextValue();
```

# Potential Issues

* Uses xorshf96, which may not be the best random number generator [2][3]. It's super fast, though, and hopefully sufficiently random.
* May suffer from [Modulo Bias](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#Potential_sources_of_bias). Probably not much of an issue for large arrays and this is intented for large arrays.

# References

[1] [Fisher-Yates](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle)
[2] https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
[3] https://github.com/raylee/xorshf96


