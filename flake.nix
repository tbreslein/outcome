{
  description = "Small lib for a type similar to Rust's Result";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = inputs@{ self, ... }:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" "aarch64-linux" "aarch64-darwin" ];

      perSystem = { system, pkgs, ... }: {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            cmake
            ninja
            pkg-config
            gcc12
            clang_14
            just

            cppcheck
            clang-tools
            ccls
          ];
        };
      };
    };
}
