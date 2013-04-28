#pragma once

#ifndef	__GAME_H
#define	__GAME_H

#include <cassert>

class CRandomNumberGenerator {
public:
	
	virtual int	GetRand(int min, int max) {
		return RAND_MAX;
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
class DefaultRandomNumberGenerator : public CRandomNumberGenerator {
public:
	int GetRand(int rMin, int rMax)
	{
		assert(rMax > rMin);
    
		int rVal = ((double) rand() / (((float)RAND_MAX)+1)) * (rMax-rMin) + rMin;

		assert(rVal < rMax && rVal >= rMin);

		return rVal;
	}
};

//----------------- game options all parts of the game need -----------
enum gs {GS_MENU, GS_START_GAME, GS_START_WORLD, GS_END_GAME, GS_GAME, GS_QUIT};

class CGame
{
public:
    CGame(char *);
    ~CGame(void);

    // run all the game logic
    void Go();

	int ScreenWidth;
	int ScreenHeight;
	CRandomNumberGenerator	*rng;


//private:

};



#endif // __GAME_H
