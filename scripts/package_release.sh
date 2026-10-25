#!/usr/bin/env bash
set -euo pipefail

usage() {
	printf 'Usage: %s <tag-or-commit> [output-directory]\n' "$0" >&2
	exit 2
}

ref="${1:-}"
output_dir="${2:-dist}"

if [[ -z "$ref" ]]; then
	usage
fi

commit="$(git rev-parse --verify "${ref}^{commit}")"
tag_name="${ref#refs/tags/}"
tag_name="${tag_name//\//-}"
archive_prefix="pokemon-x-decomp-${tag_name}/"
base_name="PokemonX-${tag_name}-source"

mkdir -p "$output_dir"
rm -f \
	"$output_dir/${base_name}.tar.gz" \
	"$output_dir/${base_name}.zip" \
	"$output_dir/SHA256SUMS"

git archive \
	--format=tar.gz \
	--prefix="$archive_prefix" \
	"$commit" > "$output_dir/${base_name}.tar.gz"
git archive \
	--format=zip \
	--prefix="$archive_prefix" \
	"$commit" > "$output_dir/${base_name}.zip"

(
	cd "$output_dir"
	sha256sum "${base_name}.tar.gz" "${base_name}.zip" > SHA256SUMS
)

printf 'Packaged %s from %s into %s\n' "$tag_name" "$commit" "$output_dir"
