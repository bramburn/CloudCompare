---
title: Algorithm research
sidebar_label: Research
sidebar_position: 1
description: Reverse-engineering notes, algorithm studies, and post-processing analyses that produced reproducible evidence rather than guesses.
---

# Algorithm research

Long-form, evidence-first writeups where we try to pin down *what algorithm* a
piece of software is running by analysing only its inputs and outputs. No
decompiling, no licence circumvention, no scraped source code. We treat the
binary as a black box, design discriminators from the public literature, run
them on real data we own, and publish the failures alongside the wins.

Every study here follows the same shape:

1. **State the question** as concretely as possible.
2. **List the candidate algorithms** from the published literature.
3. **Predict the discriminators** each candidate would leave in the data.
4. **Run the experiments** on data we legally own. Log every parameter.
5. **Report the winner**, the runners-up, and the candidates that were
   definitively ruled out. The negative results are part of the deliverable.
6. **Document the residual gap** — what we still can't explain, and what
   would be needed to close it.

## Studies

| Study | What it covers | Page |
|---|---|---|
| **Mobile-SLAM post-processing feature reverse-engineering** | A 10 mm-resolution "thin-cloud" post-processing feature on a mobile SLAM scan: from forensics through 9 reproduction candidates to a publishable algorithm description | [Mobile-SLAM post-processing](/docs/research/mobile-slam-postprocessing) |
