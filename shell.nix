{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell.override { stdenv = gcc10Stdenv; } {
  hardeningDisable = [ "all" ];
  nativeBuildInputs = [ libmpc gmp mpfr e2fsprogs ninja qemu ];
}
