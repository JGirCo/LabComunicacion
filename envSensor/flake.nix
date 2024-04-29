{
  description = "Dev environment for ESP32 and python";

  inputs = { nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable"; };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      devShells.${system}.default = pkgs.mkShell {
        nativeBuildInputs = [
          pkgs.python3
          pkgs.python311Packages.pyserial
          pkgs.arduino-core
          pkgs.arduino-language-server
          pkgs.arduino-cli
          pkgs.arduino-ide
        ];

        shellHook = ''
          zsh
          echo "Entered Arduino development shell"
        '';
      };
    };
}
