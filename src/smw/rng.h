
#include <cassert>
#include <climits>

#define	RNGMAX(rMaxInt)	smw->rng->GetRandMax(rMaxInt)

class CRandomNumberGenerator {
public:

	virtual int	GetRand(int min, int max) {
		// this should never be called
		return RAND_MAX;
	}

	void ReSeed(unsigned int seed) {
		srand(seed);
	}

	// generate uniformly distributed random number
	// rMax is not part of the set
	int	GetRandMax(int rMax)
	{
		const int rMin = 0;

		return GetRand(rMin, rMax);
	}

	bool GetRandBool()
	{
		return GetRandBool(2);
	}

	// will return true only if 1/scaleMax is verified
	bool GetRandBool(int scaleMax)
	{
		// maybe we shall match center of range, like (scaleMax/2 - 1) ?
		return 0 == GetRandMax(scaleMax);
	}

	// make a decision using 'scaleMax' steps and 'positiveThreshold' as threshold
	// returns true only when random value is above the threshold
	bool GetRandBool(int scaleMax, int positiveThreshold)
	{
		assert(positiveThreshold < scaleMax && positiveThreshold >= 0);
		return GetRandMax(scaleMax) > positiveThreshold;
	}

};

// default random number generator, using system
class SystemRandomNumberGenerator : public CRandomNumberGenerator {
public:
	int GetRand(int rMin, int rMax)
	{
		assert(rMax > rMin);

		int rVal = ((double) rand() / (((float)RAND_MAX)+1)) * (rMax-rMin) + rMin;

		assert(rVal < rMax && rVal >= rMin);

		return rVal;
	}
};


// default random number generator, using system
class Well512RandomNumberGenerator : public CRandomNumberGenerator {
private:
		/* initialize state to random bits */
		unsigned int state[16];
		/* init should also reset this to 0 */
		unsigned int index;

		/* return 32 bit random number */
		unsigned int	getNext() {
		   unsigned int a, b, c, d;
		   a = state[index];
		   c = state[(index+13)&15];
		   b = a^c^(a<<16)^(c<<15);
		   c = state[(index+9)&15];
		   c ^= (c>>11);
		   a = state[index] = b^c;
		   d = a^((a<<5)&0xDA442D20UL);
		   index = (index + 15)&15;
		   a = state[index];
		   state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
		   return state[index];
		}

		void initialize() {
			assert(sizeof(unsigned int) == 4);

			index = 0;

			for(int i =0; i < 16; i++) {
				state[i] = rand();
			}
		}

public:

	// we initialize the state with (supposedly) true random numbers coming from the system
	// this constitutes a good enough seed
	Well512RandomNumberGenerator() {
		initialize();
	}

	void ReSeed(unsigned int seed) {
		CRandomNumberGenerator::ReSeed(seed);
		initialize();
	}

	int GetRand(int rMin, int rMax)
	{
		assert(rMax > rMin);

		int rVal = ((double) getNext() / (((float)UINT_MAX)+1)) * (rMax-rMin) + rMin;

		assert(rVal < rMax && rVal >= rMin);

		return rVal;
	}
};
