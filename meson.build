project(
  'tosh',
  'cpp',
  version: '0.1.0',
  default_options: ['warning_level=3', 'cpp_std=gnu++26'],
)

libmagic_enum_proj = subproject('magic_enum')
libmagic_enum_dep = libmagic_enum_proj.get_variable('magic_enum_dep')

subdir('src')

lib_inc = include_directories('include')

install_subdir('include/tosh', install_dir: 'include')

shlib = shared_library(
  'tosh',
  lib_src,
  dependencies: [libmagic_enum_dep],
  include_directories: lib_inc,
  install: true,
)

dep_tosh_shared = declare_dependency(include_directories: lib_inc, link_with: shlib)

inc = include_directories('include')
src = files('src/main.cpp')
deps = [dep_tosh_shared, libmagic_enum_dep]

executable(
  'tosh',
  src,
  include_directories: inc,
  dependencies: deps,
  install: true,
)

# subdir tests
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