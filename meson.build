project(
  'tosh',
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
  'src/parser/ast/backslash.cpp',
  'src/parser/ast/base.cpp',
  'src/parser/ast/expr.cpp',
  'src/parser/ast/quote.cpp',
  # 'src/parser/ast/redirect.cpp',
  'src/parser/ast/root.cpp',
  'src/parser/ast/text.cpp',
  'src/parser/parser.cpp',
  'src/parser/query.cpp',
  'src/repl.cpp',
)
lib_inc = include_directories('include')

install_subdir('include/tosh', install_dir: 'include')

shlib = shared_library(
  'tosh',
  lib_src,
  include_directories: lib_inc,
  install: true,
)

dep_tosh_shared = declare_dependency(include_directories: lib_inc, link_with: shlib)

inc = include_directories('include')
src = files('src/main.cpp')
deps = [dep_tosh_shared]

exe = executable(
  'tosh',
  src,
  include_directories: inc,
  dependencies: deps,
  install: true,
)

test('tosh', exe)

libs = [shlib]

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name: 'tosh',
  filebase: 'tosh',
  description: 'A toy shell',
  libraries: libs,
  version: '0.1.0',
)