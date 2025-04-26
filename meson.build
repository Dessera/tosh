project(
  'tosh',
  'cpp',
  version: '0.1.0',
  default_options: ['warning_level=3', 'cpp_std=gnu++26'],
)

libevent = dependency('libevent')
libevent_pthreads = dependency('libevent_pthreads')

magic_enum_proj = subproject('magic_enum')
magic_enum_dep = magic_enum_proj.get_variable('magic_enum_dep')

catch2_proj = subproject('catch2')
catch2_with_main_dep = catch2_proj.get_variable('catch2_with_main_dep')

subdir('src')

lib_inc = include_directories('include')

shlib = shared_library(
  'tosh',
  lib_src,
  dependencies: [magic_enum_dep, libevent, libevent_pthreads],
  include_directories: lib_inc,
  gnu_symbol_visibility: 'hidden',
  cpp_args: ['-Wall', '-Wno-pedantic'],
  install: true,
)

install_subdir('include/tosh', install_dir: 'include')

tosh_dep = declare_dependency(include_directories: lib_inc, link_with: shlib)

inc = include_directories('include')
src = files('src/main.cpp')
deps = [tosh_dep, magic_enum_dep, libevent, libevent_pthreads]

executable(
  'tosh',
  src,
  include_directories: inc,
  dependencies: deps,
  install: true,
)

# test('tosh', exe)
subdir('tests')

libs = [shlib]

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name: 'tosh',
  filebase: 'tosh',
  description: 'A toy shell',
  libraries: libs,
  version: '0.1.0',
)