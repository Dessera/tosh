project(
  'desh',
  'cpp',
  version: '0.1.0',
  default_options: ['warning_level=3', 'cpp_std=gnu++26'],
)

if get_option('buildtype') == 'debug'
  # add_project_arguments('-D_FORTIFY_SOURCE=0', language: 'cpp')
endif

lib_src = files(
  'src/builtins/cd.cpp',
  'src/builtins/check_args.cpp',
  'src/builtins/echo.cpp',
  'src/builtins/exec.cpp',
  'src/builtins/exit.cpp',
  'src/builtins/pwd.cpp',
  'src/builtins/type.cpp',
  'src/desh.cpp',
  'src/parser.cpp',
  'src/parser/token/backslash.cpp',
  'src/parser/token/base.cpp',
  'src/parser/token/expr.cpp',
  'src/parser/token/normal.cpp',
  'src/parser/token/quote.cpp',
  'src/parser/token/root.cpp',
  'src/repl.cpp',
)
lib_inc = include_directories('include')

install_subdir('include/desh', install_dir: 'include')

shlib = shared_library(
  'desh',
  lib_src,
  include_directories: lib_inc,
  install: true,
)

dep_desh_shared = declare_dependency(include_directories: lib_inc, link_with: shlib)

inc = include_directories('include')
src = files('src/main.cpp')
deps = [dep_desh_shared]

exe = executable(
  'desh',
  src,
  include_directories: inc,
  dependencies: deps,
  install: true,
)

test('desh', exe)

libs = [shlib]

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name: 'desh',
  filebase: 'desh',
  description: 'A toy shell',
  libraries: libs,
  version: '0.1.0',
)