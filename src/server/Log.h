#ifndef LOG_H
#define LOG_H

bool log_init();
void log_close();
void log_silently(const char* message, ...);
void log(const char* message, ...);

#endif // LOG_H