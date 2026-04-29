{
  description = "Luminomagus development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, utils }:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            cmake
            clang-tools # for clangd
            ninja
            pkg-config
          ];

          buildInputs = with pkgs; [
            # Raylib dependencies
            mesa
            libGL
            xorg.libX11
            xorg.libXcursor
            xorg.libXi
            xorg.libXinerama
            xorg.libXrandr
            alsa-lib
          ];

          shellHook = ''
            echo "Luminomagus dev environment loaded"
          '';
        };
      });
}
