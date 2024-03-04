{pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  nativeBuildInputs = [
    pkgs.python3
    pkgs.python311Packages.pyserial
  ];
}
