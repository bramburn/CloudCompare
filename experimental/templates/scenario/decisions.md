# Decisions — scenario winner

> Filled in at the end of the scenario. Move long-form detail to
> `docs/decisions/<topic>.md`.

## Winner

- **Variant:** `XX-name/`
- **Status:** `selected`
- **Date:** <YYYY-MM-DD>

## Why

(3–7 bullets. Each bullet must be backed by a measurement, not an
intuition. Link to the row in `results.json` or `RESULTS.md`.)

- Correctness: matches the brute-force reference within ε (see `results.json`)
- Speed: 1.4× faster than the next-best at N=10k
- Memory: …
- Code complexity: …
- Dependency surface: …

## What we are NOT claiming

- "Faster in all cases" — only measured at N=2k/5k/10k
- "Production-ready" — graduation to `cc-rust/` is a separate step

## Source

- Per-variant `experiment.toml` files
- `results.json` (machine-readable)
- `RESULTS.md` (human narrative)
