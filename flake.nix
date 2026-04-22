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
  }: let
    eachSystem = flake-utils.lib.eachDefaultSystem (
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

          meta = with pkgs.lib; {
            description = "Dress-up DDLC character composer";
            mainProgram = "losertron";
            platforms = platforms.linux;
          };
        };

        apps.default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/losertron";
          meta = self.packages.${system}.default.meta;
        };

        checks.default = self.packages.${system}.default;

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
  in
    eachSystem
    // {
      nixosModules.default = {
        config,
        lib,
        pkgs,
        ...
      }: let
        cfg = config.programs.losertron;
      in {
        options.programs.losertron = {
          enable = lib.mkEnableOption "losertron";
          package = lib.mkOption {
            type = lib.types.package;
            default = self.packages.${pkgs.system}.default;
            description = "LOSERTRON16 FREAKING MILLION PACKAGE";
          };
        };

        config = lib.mkIf cfg.enable {
          environment.systemPackages = [cfg.package];
        };
      };
    };
}
