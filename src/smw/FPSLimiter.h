#ifndef FPSLIMITER_H
#define FPSLIMITER_H

class FPSLimiter
{
    public:
        void frameStart();
        void beforeFlip();
        void afterFlip();

        static FPSLimiter& instance();

    private:
        unsigned int    framestart, ticks;
        float           realfps, flipfps;

        // disabled constructors
        FPSLimiter();
        ~FPSLimiter() {}
        FPSLimiter(FPSLimiter const&);
        void operator=(FPSLimiter const&);
};

#endif // FPSLIMITER_H
