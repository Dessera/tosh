#pragma once

#define TOSH_EXPORT __attribute__((visibility("default")))

#define TOSH_MAKE_MOVE_OPS(type, label)                                        \
  type(type&&) = label;                                                        \
  type& operator=(type&&) = label;

#define TOSH_MAKE_COPY_OPS(type, label)                                        \
  type(const type&) = label;                                                   \
  type& operator=(const type&) = label;

#define TOSH_DEFAULT_MOVE(type) TOSH_MAKE_MOVE_OPS(type, default)
#define TOSH_DEFAULT_COPY(type) TOSH_MAKE_COPY_OPS(type, default)

#define TOSH_DELETE_MOVE(type) TOSH_MAKE_MOVE_OPS(type, delete)
#define TOSH_DELETE_COPY(type) TOSH_MAKE_COPY_OPS(type, delete)