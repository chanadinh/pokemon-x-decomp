#!/usr/bin/env bash
set -euo pipefail

usage() {
	printf 'Usage: %s <version> <output-directory>\n' "$0" >&2
	exit 2
}

version="${1:-}"
output_dir="${2:-}"

if [[ -z "$version" || -z "$output_dir" ]]; then
	usage
fi

package_root="PokemonX-${version}-linux"
package_dir="$output_dir/$package_root"
archive="$output_dir/$package_root.tar.gz"

rm -rf "$package_dir" "$archive"
mkdir -p "$package_dir/bin"

for executable in \
	pokemon_asset_viewer \
	pokemon_model_viewer \
	pokemon_field_viewer \
	pokemon_slice \
	pica_probe \
	shader_check; do
	if [[ ! -x "native/build/$executable" ]]; then
		printf 'Missing executable: native/build/%s\n' "$executable" >&2
		exit 1
	fi
	cp "native/build/$executable" "$package_dir/bin/$executable"
done

cat > "$package_dir/README.txt" <<'EOF'
Pokémon X native viewer demo

Requirements:
  - 64-bit Linux
  - SDL2 runtime libraries

Run the self-contained synthetic demo:
  ./bin/pokemon_asset_viewer --demo
  ./bin/pokemon_model_viewer --demo
  ./bin/pokemon_field_viewer --demo
  ./bin/pokemon_slice --demo

To inspect locally extracted game data, pass --romfs /path/to/romfs to the
viewer described in the project documentation. This release does not include
Nintendo game files or extracted game assets.
EOF

tar -C "$output_dir" -czf "$archive" "$package_root"
rm -rf "$package_dir"

printf 'Packaged native Linux viewers into %s\n' "$archive"
