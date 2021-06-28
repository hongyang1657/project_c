#ifndef FD_LOG_H
#define FD_LOG_H

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <exception>

#define RESET "\033[0m"
#define GREEN "\033[32m" /*Green*/
#define RED "\033[31m" /*Red*/

#define LOG_FILE_PATH "/data/found/log/nlu.log"
#define ALL_LOG_PATH "/data/found/log/log.txt"
#define CACHE_LOG_PATH "/data/found/log/cache_log.txt"

std::string log_time();

void LOG_FILE(std::string title, std::string log);

int checkCleanLogFile(char *path);

void traverseLogFile();

#define FD_LOG(msg) std::cout << GREEN << "✋   " << __FUNCTION__ <<"_" << __LINE__ << " (" << log_time() << "): " << msg << RESET

#define FD_DEBUG(msg) std::cout << RED << "🐛   " << __FUNCTION__ <<"_" << __LINE__ << " (" << log_time() << "): " << msg << RESET

#define FD_LOG_L(msg) std::cout << GREEN << "✋   " << __FUNCTION__ <<"_" << __LINE__ << " (" << log_time() << "): " << msg << RESET << std::endl

#define FD_DEBUG_L(msg) std::cout << RED << "🐛   " << __FUNCTION__ <<"_" << __LINE__ << " (" << log_time() << "): " << msg << RESET << std::endl

#endif
