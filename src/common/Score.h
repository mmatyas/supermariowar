#ifndef SCORE_H
#define SCORE_H

class CScore
{
	public:
		CScore(short iPlace) {
			place = iPlace;
			displayorder = iPlace;

			score = 0;

			for (short iSubScore = 0; iSubScore < 3; iSubScore++)
				subscore[iSubScore] = 0;

			x = 0;
			y = 0;
			destx = 0;
			desty = 0;
			order = 0;
			fromx = 0;
			fromy = 0;
			iDigitRight = 0;
			iDigitMiddle = 0;
			iDigitLeft = 0;
		}

		~CScore() {}

		void AdjustScore(short iValue);

		void SetScore(short iValue) {
			score = iValue;
			SetDigitCounters();
		}

		//keeps track of what the actual score value is
		short score;

		//keeps track of other scoring elements for some games (health, collected cards, etc)
		short subscore[3];

		//Where to display score
		short x;
		short y;

		short destx;
		short desty;

		short place;
		short displayorder;
		short order;  //the order in which the team died

		short fromx;
		short fromy;

		//One less array dereference doing vars like this
		short iDigitRight;
		short iDigitMiddle;
		short iDigitLeft;

	private:

		void SetDigitCounters() {
			short iDigits = score;
			while (iDigits > 999)
				iDigits -= 1000;

			iDigitLeft = iDigits / 100 * 16;
			iDigitMiddle = iDigits % 100 / 10 * 16;
			iDigitRight = iDigits % 10 * 16;
		}

	//friend class CGM_Star;
};

#endif // SCORE_H
