# GitHub discovery setup

These are proposed settings for `chanadinh/pokemon-x-decomp`. Repository file
changes take effect after they are committed and pushed; this document does not
apply remote settings automatically.

## About

On the repository home page, click the gear beside **About**.

Description:

> Pokémon X reverse engineering and portable C reconstruction. Six connected subsystems, synthetic tests, and a roadmap toward a native PC port. Early research; not playable.

Topics:

```text
pokemon pokemon-x nintendo-3ds decompilation reverse-engineering
c game-preservation ghidra arm
```

Leave Website empty until there is a maintained project site or demo. Show the
README and releases in the About section if the corresponding options appear.

With an authenticated GitHub CLI, the equivalent configuration is:

```sh
gh repo edit chanadinh/pokemon-x-decomp \
  --description 'Pokémon X reverse engineering and portable C reconstruction. Six connected subsystems, synthetic tests, and a roadmap toward a native PC port. Early research; not playable.' \
  --add-topic pokemon,pokemon-x,nintendo-3ds,decompilation,reverse-engineering,c,game-preservation,ghidra,arm
```

## A useful launch

1. Publish the README and contribution files; confirm the Host tests workflow is green.
2. Add a real terminal recording or screenshot of the six-module synthetic test
   run, labeled clearly as host tests. Add it near the top of the README.
3. Create a small release when a reproducible source milestone is ready. Include
   commands, verified behavior, remaining stand-ins, and test results. Do not
   advertise local ZIP names as downloadable release assets unless uploaded.
4. Open a few bounded research issues with evidence and completion criteria.
   Use `good first issue` only for tasks with enough context for a newcomer.
5. Pin the repository on your profile. Share a factual progress update in
   relevant communities where project promotion is welcome.

Suggested launch text, to use after checks pass:

> I'm reconstructing Pokémon X game logic in portable C. The project currently
> connects six subsystems, from the wild-battle script wrapper to Pokémon data
> storage, with synthetic tests that need no game files. It's early research,
> with no playable PC build yet. Looking for help with ARM analysis, reference
> comparisons, and resource loading: https://github.com/chanadinh/pokemon-x-decomp

Report new verified behavior with each update. Track whether visitors can run
the tests and make useful contributions, alongside stars. No setup can guarantee
virality.
