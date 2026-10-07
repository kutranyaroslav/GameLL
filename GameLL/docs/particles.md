# Particles

An emitter is a component on an entity. Give an entity a `C_Particles` and it
continuously throws particles from its position; there is nothing to construct
in code and no asset to ship.

## Adding an emitter to an entity

Two edits in the `.entity` file, and the second one is the one people forget.

```
Name Player
Attributes 1023                                             <-- 1
...
|Particles|rate|lifetime|speed|spreadDegrees|size|r|g|b|
Component 9 18 1.6 34 70 3 255 170 90                       <-- 2
```

**1. Widen `Attributes`.** It is a bitmask of which components to create, and
particles are bit 9, so the mask must include 512. If you forget this, the
`Component 9` line is *silently ignored* — `EntityManagerNew::AddEntity` looks
the component up, gets nothing back, and skips the line without complaining.
No error, no particles.

| components wanted | mask |
|---|---|
| 0–7, the original set | 255 |
| plus a light (bit 8) | 511 |
| plus particles (bit 9) | 1023 |

**2. Add the `Component 9` line.** The `|...|` line above it is a comment; the
loader skips anything starting with `|`.

## The parameters

`Component 9 <rate> <lifetime> <speed> <spreadDegrees> <size> <r> <g> <b>`

| | meaning | units | sensible range |
|---|---|---|---|
| `rate` | particles emitted per second | 1/s | 5 (a trickle) – 60 (a jet) |
| `lifetime` | how long one lives | seconds | 0.4 – 3.0 |
| `speed` | how fast it travels | world units/s | 10 (drifting) – 120 (spraying) |
| `spreadDegrees` | width of the emission cone | degrees | 0 (a beam) – 360 (a burst) |
| `size` | side of the square particle | world units | 1 – 8 |
| `r g b` | colour, 0–255 each | | |

Scale matters: the game view is 480x270 world units, and a tile is 32. So
`size 3` is about a tenth of a tile, and `speed 34` crosses a tile a second.

Three things are randomised per particle so the effect does not look mechanical:

- direction, anywhere in the cone
- speed, 60–100% of `speed`
- lifetime, 70–100% of `lifetime`

The cone is centred **straight up**. There is no gravity and no drag: a particle
travels in a straight line at constant speed and fades linearly to invisible
over its life.

## How it is drawn

Every particle of one emitter goes into a single `sf::VertexArray` of quads and
is drawn **additively into the scene**, which has two consequences worth
knowing:

- Overlapping particles **build up** rather than covering each other, so a
  dense emitter gets brighter in the middle. Turn `rate` down before you turn
  `size` up.
- They are inside the post processing chain, so they are lit, they pick up the
  vignette and grain, and **they bloom**. With the bloom threshold at its
  default of `0.1` a bright particle colour will glow noticeably.

Each emitter is capped at 400 live particles. `rate * lifetime` is roughly how
many will be alive at once, so keep that product under 400 or the emitter
quietly stops keeping up.

## Tuning

Two recipes to work from:

```
# warm embers drifting up from a torch: slow, sparse, long lived
Component 9 18 1.6 34 70 3 255 170 90

# cold steam venting: fast, dense, short lived, narrow
Component 9 45 0.6 90 25 2 200 220 255
```

- **too sparse** → raise `rate`, or raise `lifetime` so they linger
- **too blobby** → lower `rate` first, then `size`; additive blending means
  density reads as brightness
- **too directional** → raise `spreadDegrees`; 360 turns the cone into a burst
- **not visible in the dark** → the particles are lit like everything else. If
  they should read as self-lit, give the entity a `C_LightSource` too rather
  than brightening the particle colour past what the scene can carry.

The random sequence is seeded with a fixed value, so a given emitter looks the
same run to run. That is deliberate: it makes visual changes reproducible.

## Limits, if you need more

Deliberately left out, and roughly where each would go:

- **no gravity or drag** — `S_Particles::Update` integrates position from a
  constant velocity; an acceleration field would go there
- **no burst mode** — the emitter is continuous. A one shot would need a count
  and an enable flag on the component
- **no textured particles** — they are flat coloured quads, so no asset is
  needed. A texture would mean binding one per emitter and giving up the single
  batched draw
- **no per-particle rotation or size curve** — size is constant, only alpha
  animates

See also `GameLL_VFX_Pipeline.pdf` for the chain the particles are drawn into.
