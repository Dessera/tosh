{
  stdenv,
  lib,

  # Build tools
  meson,
  ninja,
  pkg-config,
}:
stdenv.mkDerivation {
  name = "tosh";
  src = lib.cleanSource ./.;

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
  ];
}
