#############################################################################
##
## Copyright (C) 2017 The Qt Company Ltd.
## Contact: http://www.qt.io/licensing/
##
## This file is part of the PySide examples of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:BSD$
## You may use this file under the terms of the BSD license as follows:
##
## "Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
##   * Redistributions of source code must retain the above copyright
##     notice, this list of conditions and the following disclaimer.
##   * Redistributions in binary form must reproduce the above copyright
##     notice, this list of conditions and the following disclaimer in
##     the documentation and/or other materials provided with the
##     distribution.
##   * Neither the name of The Qt Company Ltd nor the names of its
##     contributors may be used to endorse or promote products derived
##     from this software without specific prior written permission.
##
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
##
## $QT_END_LICENSE$
##
#############################################################################

import os, glob, re, sys, imp
from distutils import sysconfig
if sys.platform == 'win32':
    import winreg

usage = """
Utility to determine include/link options of PySide2 and Python for qmake

Usage: pyside2_config.py [option]
Options:
    --python-include            Print Python include path
    --python-link               Print Python link flags
    --pyside2                   Print PySide2 location
    --pyside2-include           Print PySide2 include paths
    --pyside2-link              Print PySide2 link flags
    --pyside2-shared-libraries  Print paths of PySide2 shared libraries (.so's, .dylib's, .dll's)
    --clang-bin-dir             Print path to the clang bin directory
    -a                          Print all
    --help/-h                   Print this help
"""

def cleanPath(path):
    return path if sys.platform != 'win32' else path.replace('\\', '/')

def sharedLibrarySuffix():
    if sys.platform == 'win32':
        return 'lib'
    elif sys.platform == 'darwin':
        return 'dylib'
    return 'so'

def sharedLibraryGlobPattern():
    glob = '*.' + sharedLibrarySuffix()
    return glob

def filterPySide2SharedLibraries(list):
    def predicate(item):
        basename = os.path.basename(item)
        if 'Qt' in basename and 'cpython' in basename:
            return True
        if 'shiboken' in basename or 'pyside2' in basename:
            return True
        return False
    result = [item for item in list if predicate(item)]
    return result

# Return qmake link option for a library file name
def linkOption(lib):
    baseName = os.path.splitext(os.path.basename(lib))[0]
    link = ' -l'
    if sys.platform in ['linux', 'linux2', 'darwin']: # Linux: 'libfoo.so' -> '-lfoo'
        link += baseName[3:]
    else:
        link += baseName
    return link

# Locate PySide2 via package path
def findPySide2():
    for p in sys.path:
        if 'site-' in p:
            pyside2 = os.path.join(p, 'PySide2')
            if os.path.exists(pyside2):
                return cleanPath(os.path.realpath(pyside2))
    return None

# Return version as "3.5"
def pythonVersion():
    return str(sys.version_info[0]) + '.' + str(sys.version_info[1])

def pythonInclude():
    return sysconfig.get_python_inc()

def pythonLinkQmake():
    flags = pythonLinkData()
    if sys.platform == 'win32' or sys.platform == 'darwin':
        return '-L{} -l{}'.format(flags['libdir'], flags['lib'])

    # Linux and anything else
    return '-l{}'.format(flags['lib'])

def pythonLinkCmake():
    flags = pythonLinkData()
    libdir = flags['libdir']
    lib = re.sub(r'.dll$', '.lib', flags['lib'])
    return '{} {}'.format(libdir, lib)

def pythonLinkData():
    # @TODO Fix to work with static builds of Python
    libdir = sysconfig.get_config_var('LIBDIR')
    version = pythonVersion()
    version_no_dots = version.replace('.', '')

    flags = {}
    flags['libdir'] = libdir
    if sys.platform == 'win32':
        suffix = '_d' if any([tup[0].endswith('_d.pyd') for tup in imp.get_suffixes()]) else ''
        flags['lib'] = 'python{}{}'.format(version_no_dots, suffix)

    elif sys.platform == 'darwin':
        flags['lib'] = 'python{}'.format(version)

    # Linux and anything else
    else:
        if sys.version_info[0] < 3:
            suffix = '_d' if any([tup[0].endswith('_d.so') for tup in imp.get_suffixes()]) else ''
            flags['lib'] = 'python{}{}'.format(version, suffix)
        else:
            flags['lib'] = 'python{}{}'.format(version, sys.abiflags)

    return flags

def pyside2Include():
    pySide2 = findPySide2()
    if pySide2 is None:
        return None
    return "/usr/include/PySide2 /usr/include/shiboken2"

def pyside2Link():
    pySide2 = findPySide2()
    if pySide2 is None:
        return None
    link = "-L{}".format(pySide2)
    glob_result = glob.glob(os.path.join(pySide2, sharedLibraryGlobPattern()))
    for lib in filterPySide2SharedLibraries(glob_result):
        link += ' '
        link += linkOption(lib)
    return link

def pyside2SharedLibrariesData():
    pySide2 = findPySide2()
    if pySide2 is None:
        return None

    glob_result = glob.glob(os.path.join(pySide2, sharedLibraryGlobPattern()))
    filtered_libs = filterPySide2SharedLibraries(glob_result)
    libs = []
    if sys.platform == 'win32':
        for lib in filtered_libs:
            libs.append(os.path.realpath(lib))
    else:
        for lib in filtered_libs:
            libs.append(lib)
    return libs

def pyside2SharedLibraries():
    libs = pyside2SharedLibrariesData()
    if libs is None:
        return None

    if sys.platform == 'win32':
        if not libs:
            return ''
        dlls = ''
        for lib in libs:
            dll = os.path.splitext(lib)[0] + '.dll'
            dlls += dll + ' '

        return dlls
    else:
        libs_string = ''
        print(libs)
        for lib in libs:
            libs_string += ' ' + lib
        return libs_string

def pyside2SharedLibrariesCmake():
    libs = pyside2SharedLibrariesData()
    result = ' '.join(libs)
    result += ' /usr/lib/libpyside2.cpython-36m-x86_64-linux-gnu.so.2.0'
    result += ' /usr/lib/libshiboken2.cpython-36m-x86_64-linux-gnu.so.2.0'
    return result

def clangBinPath():
    source = 'LLVM_INSTALL_DIR'
    clangDir = os.environ.get(source, None)
    if not clangDir:
        source = 'CLANG_INSTALL_DIR'
        clangDir = os.environ.get(source, None)
        if not clangDir:
            source = 'llvm-config'
            try:
                output = run_process_output([source, '--prefix'])
                if output:
                    clangDir = output[0]
            except OSError:
                pass
    if clangDir:
        return os.path.realpath(clangDir + os.path.sep + 'bin')
    return ''

option = sys.argv[1] if len(sys.argv) == 2 else '-a'
if option == '-h' or option == '--help':
    print(usage)
    sys.exit(0)

if option == '--pyside2' or option == '-a':
    pySide2 = findPySide2()
    if pySide2 is None:
        sys.exit('Unable to locate PySide2')
    print(pySide2)

if option == '--pyside2-link' or option == '-a':
    l = pyside2Link()
    if l is None:
        sys.exit('Unable to locate PySide2')
    print(l)

if option == '--pyside2-include' or option == '-a':
    i = pyside2Include()
    if i is None:
        sys.exit('Unable to locate PySide2')
    print(i)

if option == '--python-include' or option == '-a':
    i = pythonInclude()
    if i is None:
        sys.exit('Unable to locate Python')
    print(i)

if option == '--python-link' or option == '-a':
    l = pythonLinkQmake()
    if l is None:
        sys.exit('Unable to locate Python')
    print(l)

if option == '--python-link-cmake' or option == '-a':
    l = pythonLinkCmake()
    if l is None:
        sys.exit('Unable to locate Python')
    print(l)

if option == '--pyside2-shared-libraries' or option == '-a':
    l = pyside2SharedLibraries()
    if l is None:
        sys.exit('Unable to locate the PySide2 shared libraries')
    print(l)

if option == '--pyside2-shared-libraries-cmake' or option == '-a':
    l = pyside2SharedLibrariesCmake()
    if l is None:
        sys.exit('Unable to locate the PySide2 shared libraries')
    print(l)

if option == '--clang-bin-dir' or option == '-a':
    l = clangBinPath()
    if l is None:
        sys.exit('Unable to locate Clang')
    print(l)

