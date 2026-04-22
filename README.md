# ImLOSERTRON16000000
 make the okdi doki literature club people like a gacha life game (they likle it when you playh dressup with them!)\
 made in DearImGUI so it can eat your gpu resources with immediate rendering
![finished app](https://github.com/user-attachments/assets/d36f3d94-ae83-4aa4-8b1a-c85ba89fb53d)

## Nix / NixOS

- Run from flake app: `nix run .#default`
- Build package: `nix build .#default`

Use as a NixOS module input:

```nix
{
	inputs.losertron.url = "github:DogeDoge17/losertron16000000";

	outputs = { self, nixpkgs, losertron, ... }: {
		nixosConfigurations.my-host = nixpkgs.lib.nixosSystem {
			system = "x86_64-linux";
			modules = [
				losertron.nixosModules.default
				{
					programs.losertron.enable = true;
				}
			];
		};
	};
}
```

Optional override:

```nix
programs.losertron.package = losertron.packages.${pkgs.system}.default;
```

