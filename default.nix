{
  stdenv,
  lib,

  # Build tools
  meson,
  ninja,
  pkg-config,

  # dependencies
  libevent,
}:
stdenv.mkDerivation {
  name = "tosh";
  src = lib.cleanSource ./.;

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
  ];

  buildInputs = [ libevent ];
}
