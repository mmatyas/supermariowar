// return a random integer in range 0...max
int	GetRandMax(int max);
bool GetRandBool();

// make a decision using 'scaleMax' steps and 'positiveThreshold' as threshold
// returns true only when random value is above the threshold
bool GetRandBool(int scaleMax, int positiveThreshold);

// will return true only if 1/scaleMax is verified
bool GetRandBool(int scaleMax);
