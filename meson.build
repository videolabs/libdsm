project('libdsm', ['c'],
    version: '0.4.3',
    license: ['LGPL2'],
    default_options: ['c_std=c11',
                      'warning_level=2',
                      'buildtype=release',
                      'b_ndebug=if-release'],
    meson_version: '>= 0.53.0')

#Soname
dsm_soname_version = '3.2.0'
if host_machine.system() == 'windows'
  dsm_soversion = ''
else
  dsm_soversion = dsm_soname_version.split('.')[0]
endif

conf_data = configuration_data()
conf_data.set_quoted('URANDOM', '/dev/urandom') #TODO
conf_data.set('_FORTIFY_SOURCE', 2,
              description: 'Define to 2 to get glibc warnings.')
conf_data.set('_FILE_OFFSET_BITS', 64,
              description: 'Define to 64 for large file support.')
conf_data.set('_DEFAULT_SOURCE', 1)

cc = meson.get_compiler('c')

# Add -fvisibility=hidden if compiler supports those
add_project_arguments(
    cc.get_supported_arguments('-fvisibility=hidden'),
    language : ['c'])


dep_tasn1 = dependency('libtasn1')
dep_thread = dependency('threads')
dep_rt = []

optional_arguments = [
  '-Wall',
  '-Wsign-compare',
  '-Wextra',
  '-Wstrict-aliasing',
  '-Wstrict-overflow' ]

add_project_arguments(
  cc.get_supported_arguments(optional_arguments),
  language: 'c')

if cc.has_argument('-fvisibility=hidden')
    add_project_arguments('-fvisibility=hidden', language: 'c')
else
    warning('Compiler does not support -fvisibility=hidden, all symbols will be public!')
endif

if get_option('default_library') != 'static'
    conf_data.set('DLL_EXPORT', true)
endif

test_args = ''

# Check headers
if cc.has_header('bsd/string.h')
  conf_data.set('HAVE_BSD_STRING_H', 1)
endif

if cc.has_header('alloca.h')
  conf_data.set('HAVE_ALLOCA_H', 1)
endif

if cc.has_header('sys/queue.h')
  conf_data.set('HAVE_SYS_QUEUE_H', 1)
endif

if cc.has_header('arpa/inet.h')
  conf_data.set('HAVE_ARPA_INET_H', 1)
endif

if cc.has_header('sys/socket.h')
  conf_data.set('HAVE_SYS_SOCKET_H', 1)
endif

if cc.has_header('sys/time.h')
  conf_data.set('HAVE_SYS_TIME_H', 1)
endif

if cc.has_header('ifaddrs.h')
  conf_data.set('HAVE_IFADDRS_H', 1)
endif


# Check functions
if host_machine.system() == 'linux'
    test_args += '-D_GNU_SOURCE'
    add_project_arguments('-D_GNU_SOURCE', language: 'c')
elif host_machine.system() == 'darwin'
    test_args += '-D_DARWIN_C_SOURCE'
    add_project_arguments('-D_DARWIN_C_SOURCE', language: 'c')
else
    test_args += '-D_POSIX_C_SOURCE=200809L'
    add_project_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')
endif

compat_sources = []

if cc.has_function('strlcpy', prefix: '#include <string.h>', args: test_args)
  conf_data.set('HAVE_STRLCPY', 1)
else
  compat_sources += 'compat/strlcpy.c'
endif

if cc.has_function('strndup', prefix: '#include <string.h>', args: test_args)
  conf_data.set('HAVE_STRNDUP', 1)
else
  compat_sources += 'compat/strndup.c'
endif

if cc.has_function('clock_gettime', prefix: '#include <time.h>',
                   dependencies: dep_thread)
  conf_data.set('HAVE_CLOCK_GETTIME', 1)
else
  dep_rt = cc.find_library('rt', required: false)
  if cc.has_function('clock_gettime', prefix: '#include <time.h>',
                     dependencies: [dep_rt, dep_thread])
    conf_data.set('HAVE_CLOCK_GETTIME', 1)
  else
    compat_sources += 'compat/clock_gettime.c'
  endif
endif

if cc.has_function('pipe')
  conf_data.set('HAVE_PIPE', 1)
endif

if cc.has_function('_pipe')
  conf_data.set('HAVE__PIPE', 1)
endif

if cc.has_function('getifaddrs')
  conf_data.set('HAVE_GETIFADDRS', 1)
endif

if cc.has_function('nl_langinfo')
  conf_data.set('HAVE_NL_LANGINFO', 1)
endif

# Check for struct timespec
if cc.has_type('struct timespec', prefix: '#include <time.h>')
  conf_data.set('HAVE_STRUCT_TIMESPEC', 1)
endif

if cc.has_type('clockid_t', prefix: '#include<time.h>\n#include<pthread.h>')
  conf_data.set('HAVE_CLOCKID_T', 1)
endif

conf_data.set_quoted('VERSION', meson.project_version())

have_iconv = cc.has_function('iconv_open', prefix: '#include <iconv.h>')
dep_iconv = []
if not have_iconv
  dep_iconv += [cc.find_library('iconv', required: false)]
  have_iconv = cc.has_function('iconv_open',
                               prefix: '#include <iconv.h>',
                               dependencies: dep_iconv)
endif

if not have_iconv
  error('iconv is not available')
endif

config_header = configure_file(
  input: 'config.h.meson',
  output: 'config.h',
  configuration: conf_data)

fs_module = import('fs')
package_generated = true
if fs_module.is_file('spnego_asn1.c')
  spnego_asn1_target = files('spnego_asn1.c')
  package_generated = false
else
  asn1parser_prog = find_program('asn1Parser')
  spnego_asn1_target = custom_target('spnego_asn1.c',
    input: 'contrib/spnego/spnego.asn1',
    output: 'spnego_asn1.c',
    command: [asn1parser_prog, '-o', '@OUTPUT@', '-n', 'spnego_asn1_conf',
              '@INPUT@'])
endif

libdsm_sources = [
  'contrib/mdx/md4.c',
  'contrib/mdx/md5.c',
  'contrib/rc4/rc4.c',
  spnego_asn1_target,
  'src/hmac_md5.c',
  'src/netbios_ns.c',
  'src/netbios_query.c',
  'src/netbios_session.c',
  'src/netbios_utils.c',
  'src/smb_buffer.c',
  'src/smb_dir.c',
  'src/smb_fd.c',
  'src/smb_file.c',
  'src/smb_spnego.c',
  'src/smb_message.c',
  'src/smb_ntlm.c',
  'src/smb_session.c',
  'src/smb_session_msg.c',
  'src/smb_share.c',
  'src/smb_stat.c',
  'src/smb_trans2.c',
  'src/smb_transport.c',
  'src/smb_utils.c' ]
libdsm_sources += compat_sources

libdsm_headers = [
  'include/bdsm/export.h',
  'include/bdsm/netbios_defs.h',
  'include/bdsm/netbios_ns.h',
  'include/bdsm/smb_defs.h',
  'include/bdsm/smb_dir.h',
  'include/bdsm/smb_file.h',
  'include/bdsm/smb_session.h',
  'include/bdsm/smb_share.h',
  'include/bdsm/smb_stat.h',
  'include/bdsm/smb_types.h',
  'include/bdsm.h'
]

install_headers(libdsm_headers, subdir: 'bdsm')

includes = include_directories('.', 'contrib', 'include', 'compat')

add_project_arguments('-DHAVE_CONFIG_H=1', language: ['c'])

# Platform specific logger library
dep_log = []

# Android use syslog
if host_machine.system() == 'android'
  dep_log = [cc.find_library('log', required: true)]
endif

# Windows use ws2_32
dep_ws2_32 = []
if host_machine.system() == 'windows'
  dep_ws2_32 = [cc.find_library('ws2_32', required : true)]
endif

libdsm = library('dsm',
  libdsm_sources,
  version: dsm_soname_version,
  soversion: dsm_soversion,
  include_directories: includes,
  dependencies: [dep_tasn1, dep_thread, dep_iconv, dep_log, dep_ws2_32],
  install: true
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  libraries: libdsm,
  version: meson.project_version(),
  name: 'libdsm',
  filebase: 'libdsm',
  description: 'Minimalist and read-only smb client library')

# We only add the doc target if doxygen is found
doxygen = find_program('doxygen', required: false)
if doxygen.found()
  subdir('doc')
else
  warning('Doxygen not found - continuing without Doxygen support')
endif

if get_option('binaries')
  subdir('bin')
endif

if package_generated
  meson.add_dist_script('dist_generated.py', spnego_asn1_target.full_path())
endif
