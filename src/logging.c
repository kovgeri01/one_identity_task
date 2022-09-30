#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <memory.h>
#include <errno.h>

#include "logging.h"

static int gIsDaemonProcess = 0;

void logging_init(int isDaemonProcess)
{
    gIsDaemonProcess = isDaemonProcess;
}

void logging_log_info(char *format, ...)
{
    if (!gIsDaemonProcess)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

void logging_log_error(char *format, ...)
{
    if (!gIsDaemonProcess)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

void logging_log_errno(char *messageFormat, ...)
{
    if (!gIsDaemonProcess)
    {
        static const char *logPrefix = "Errno #%d with message: ";
        size_t logPrefixLen = strlen(logPrefix);
        size_t messageFormatLen = strlen(messageFormat);
        char *errorMessageFormat = (char*)calloc(logPrefixLen + messageFormatLen + 1, sizeof(char));
        memcpy(errorMessageFormat, logPrefix, logPrefixLen);
        memcpy(errorMessageFormat + logPrefixLen, messageFormat, messageFormatLen + 1);

        char *errorMessage;
        va_list args;
        va_start(args, messageFormat);
        asprintf(&errorMessage, errorMessageFormat, errno, args);
        va_end(args);
        perror(errorMessage);
        free(errorMessage);
        free(errorMessageFormat);
        errorMessage = 0;
    }
}