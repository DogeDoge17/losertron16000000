{
  description = "Losertron16000000";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    imgui-src = {
      url = "github:ocornut/imgui/v1.89.2";
      flake = false;
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    imgui-src,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {inherit system;};
      in {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "losertron";
          version = "0.1.0";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            ninja
            pkg-config
            xxd
          ];

          buildInputs = with pkgs; [
            libGL
            glfw
          ];

          cmakeFlags = [
            "-DFETCHCONTENT_SOURCE_DIR_IMGUI=${imgui-src}"
            "-DUSE_SYSTEM_GLFW=ON"
          ];

          installPhase = ''
            mkdir -p $out/bin
            cp losertron $out/bin/
            # Copy assets if they exist
            if [ -d "dokis" ]; then
              cp -r dokis $out/bin/
            fi
          '';
        };

        devShells.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            cmake
            ninja
            pkg-config
            xxd
            gdb
          ];

          buildInputs = with pkgs; [
            libGL
            glfw
          ];
        };
      }
    );
}
