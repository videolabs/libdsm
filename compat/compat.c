#include "config.h"

#if !defined(HAVE_PIPE) && defined(HAVE__PIPE)

#include <fcntl.h>

int pipe(int fds[2])
{
    return _pipe(fds, 32768, O_NOINHERIT | O_BINARY);
}
#endif
