#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#endif

#include "defs.h"

int GetTimeMs()
{
#ifdef WIN32
    return GetTickCount();
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
#endif
}

int InputWaiting()
{
#ifdef WIN32
    static int init = 0, pipe;
    static HANDLE inh;
    DWORD dw;

    if (!init)
    {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
    }

    if (pipe)
    {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
            return 1;
        return dw;
    }

    if (!GetNumberOfConsoleInputEvents(inh, &dw))
        return 1;

    return dw <= 0 ? 0 : 1;
#else
    struct timeval tv;
    fd_set readfds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(fileno(stdin), &readfds);
    select(fileno(stdin) + 1, &readfds, NULL, NULL, &tv);
    return (FD_ISSET(fileno(stdin), &readfds));
#endif
}

void ReadInput(S_SEARCHINFO *info)
{
    int bytes;
    char input[256] = "", *endc;
    if (InputWaiting())
    {
        info->stopped = true;
        do
        {
            bytes = read(fileno(stdin), input, 256);
        } while (bytes < 0);
        endc = strchr(input, '\n');
        if (endc)
            *endc = 0;

        if (strlen(input) > 0)
        {
            if (!strncmp(input, "quit", 4))
            {
                info->quit = true;
            }
        }
        return;
    }
}