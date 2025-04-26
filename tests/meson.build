# To make all symbols public in tests
shlib_internal = shared_library(
  'tosh_internal',
  lib_src,
  dependencies: lib_deps,
  include_directories: lib_inc,
  cpp_args: ['-Wall', '-Wno-pedantic'],
  install: false,
)

tosh_internal_dep = declare_dependency(include_directories: lib_inc, link_with: shlib_internal)

test_deps = lib_deps
test_deps += tosh_internal_dep
test_deps += catch2_with_main_dep

test_terminal_event_reader = executable(
  'test_terminal_event_reader',
  'event_reader.cpp',
  include_directories: inc,
  dependencies: test_deps,
)

test('terminal_event_reader', test_terminal_event_reader)

test_parser_ast = executable(
  'test_parser_ast',
  'parser_ast.cpp',
  include_directories: inc,
  dependencies: test_deps,
)

test('parser_ast', test_parser_ast)