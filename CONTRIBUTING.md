# Contributing

Start with the [README](README.md) and [roadmap](ROADMAP.md). Small, focused
pull requests are easiest to review. For a larger reconstruction, open a
research task first with the function or subsystem and the evidence available.

## Useful contributions

- Document a function's arguments, memory layout, callers, or service boundaries.
- Reconstruct a bounded routine in portable C with meaningful behavior tests.
- Add reference comparisons or edge cases that expose an actual uncertainty.
- Improve analysis tools, reproducibility instructions, or broken public links.

## Verify a change

From the repository root:

```sh
make -C reconstructed test
python3 -m unittest discover -s tests -v
```

A C11 compiler, Make, and Python 3 cover the core checks. Extraction integration
tests require optional local tools and skip when those are absent. Mention
skipped checks and any checks requiring private local inputs in your PR.

For reconstructed logic, record the baseline address, reviewed evidence,
assumptions, test scope, and unresolved dependencies alongside the subsystem.
Distinguish synthetic test results from comparisons against a running game.
Keep provisional field names and stand-ins explicit. Analysis function counts
do not establish source completion or behavioral equivalence.

Do not commit ROMs, extracted game assets, credentials, generated Ghidra
projects, or local analysis bundles. Share reproduction steps and non-sensitive
synthetic fixtures instead. The project has no declared license; do not assume
a license grant or add third-party code without recording its license and origin.
