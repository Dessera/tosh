{
  stdenv,
  lib,

  # Build tools
  meson,
  ninja,
  pkg-config,
}:
stdenv.mkDerivation {
  name = "desh";
  src = lib.cleanSource ./.;

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
  ];
}
