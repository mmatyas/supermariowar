#include "Log.h"

#include <cstdio>
#include <ctime>
#include <cstdarg>

FILE* logfile;

bool log_init()
{
	logfile = fopen("serverlog.txt", "a");
    if (!logfile)
        return false;

    return true;
}

void log_close()
{
	fclose(logfile);
}

void write_log(bool show_output, const char* parsed_message)
{
    time_t rawtime;
    struct tm sTime;

    rawtime = time(0);
    sTime = *localtime(&rawtime);

    char timeBuffer[80];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%b-%d %X", &sTime);

    if (show_output)
        printf("%s\n", parsed_message);

    fprintf(logfile, "[%s] %s\n", timeBuffer, parsed_message);
    fflush(logfile);
}

void log(const char* message, ...)
{
    char messageBuffer[256];
    va_list vars;
    va_start(vars, message);
    vsprintf(messageBuffer, message, vars);
    va_end(vars);

    write_log(true, messageBuffer);
}

void log_silently(const char* message, ...)
{
	char messageBuffer[256];
    va_list vars;
    va_start(vars, message);
    vsprintf(messageBuffer, message, vars);
    va_end(vars);

    write_log(false, messageBuffer);
}
