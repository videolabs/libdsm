#include "config.h"

/* This file is just a placeholder so the library isn't empty */

/* This function does strictly nothing. It's just here to avoid
   libcompat.a to be empty, which is illegal */
int not_empty()
{
    return 42;
}

#if !defined(HAVE_PIPE) && defined(HAVE__PIPE)

#include <fcntl.h>

static inline int pipe(int fds[2])
{
    return _pipe(fds, 32768, O_NOINHERIT | O_BINARY);
}
#endif
