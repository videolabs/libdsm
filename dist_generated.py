#!/usr/bin/python3

import shutil
import sys
import os

if __name__ == '__main__':
    if (len(sys.argv) != 2):
        print('usage: ', sys.argv[0], ' </path/to/file/to/package>')
        sys.exit(1)
    shutil.copy(sys.argv[1], os.getenv('MESON_DIST_ROOT'))
    
