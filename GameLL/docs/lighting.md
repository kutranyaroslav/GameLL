# Lighting

A light is a component on an entity. Give an entity a `C_LightSource` and it
lights the world around its position; there is no asset to ship and nothing to
construct in code.

Anywhere no light reaches keeps only the **ambient** fraction of its brightness,
so adding the first light to a map makes everything else darker. That is the
point of it, but it is worth knowing before you wonder why the level went dim.

## Adding a light to an entity

Two edits in the `.entity` file, and the second one is the one people forget.

```
Name Player
Attributes 1023                                    <-- 1
...
|LightSource|radius|r|g|b|intensity|
Component 8 120 255 226 170 1.0                    <-- 2
```

**1. Widen `Attributes`.** It is a bitmask of which components to create, and
a light is bit 8, so the mask must include 256. If you forget, the
`Component 8` line is *silently ignored*: `EntityManagerNew::AddEntity` looks
the component up, gets nothing back and skips the line. No error, no light.

| components wanted | mask |
|---|---|
| 0–7, the original set | 255 |
| plus a light (bit 8) | 511 |
| plus particles (bit 9) | 1023 |

**2. Add the `Component 8` line.** The `|...|` line above it is a comment; the
loader skips anything starting with `|`.

## The parameters

`Component 8 <radius> <r> <g> <b> <intensity>`

| | meaning | units | sensible range |
|---|---|---|---|
| `radius` | how far the light reaches | world units | 60 – 200 |
| `r g b` | colour of the light, 0–255 each | | |
| `intensity` | brightness at the centre | 0–1, clamped | 0.4 – 1.0 |

Scale matters. The game view is 480x270 world units and a tile is 32, so a
`radius` of 120 is about four tiles — a pool you can stand in. At 260 the pool
covers most of the screen and stops reading as a light at all; that was the
first value tried here and it looked like the ambient had simply been raised.

Brightness falls off smoothly from the centre to nothing at `radius`. Lights
are **additive**, so two overlapping lights build up rather than one replacing
the other.

## Ambient

The ambient colour is the floor: the brightness everything keeps where no light
falls. It lives in `S_Lighting` as `m_ambient`, defaulting to
`sf::Color(90, 90, 105)` — roughly 35%, slightly blue so unlit areas read as
cold against warm lights.

There is `SetAmbient` on the system if you want to drive it per level. Pure
black ambient means unlit areas are genuinely invisible, which is a design
choice rather than a bug.

## How it fits the chain

Per frame:

1. `Window::BeginDraw` clears the light map to **white**
2. `State_Game::Draw` calls `SystemManager::DrawLighting`, which clears the map
   to the ambient colour and adds one fan per light, in world space
3. `Window::ApplyLightmap` multiplies the map into the scene
4. the bloom pass and the composite shader run on the result

Two things follow from that order:

- **A state that never builds a light map is unaffected.** The map is cleared
  to white and multiplying by white changes nothing, which is exactly why the
  menu and the editor look the way they always did.
- **Bloom comes from the lit image**, because lighting is applied before the
  bright pass. A surface under a lamp glows; the same surface in the dark does
  not. If lighting ran last, everything would glow equally, which is what it
  did in the first draft.

Lighting is a `sf::BlendMultiply` draw, **not a shader**, so it still works on
a driver with no GLSL support at all. `Window::SetLightingEnabled(false)` turns
it off wholesale.

## Tuning

```
# a warm handheld lamp, close in
Component 8 120 255 226 170 1.0

# a cold shaft from above, reaching further, weaker
Component 8 190 170 200 255 0.6
```

- **the level is too dark** → raise the ambient before you raise `intensity`;
  intensity is clamped at 1 and pushing it does nothing past that
- **the light does not read as a light** → shrink `radius`. A pool needs an
  edge inside the screen to be legible as one
- **the pool has visible facets** → each light is a 24 segment triangle fan.
  It is smooth at these radii; a much larger radius would need more segments
  (`LIGHT_SEGMENTS` in `S_Lighting.h`)
- **nothing is lit at all** → check the `Attributes` mask first, then that the
  entity has a `C_Position`; `S_Lighting` requires both components

## Limits, if you need more

Deliberately left out, and roughly where each would go:

- **no shadows or occlusion** — light passes through walls. Real shadow casting
  would mean building geometry from the solid tiles per light
- **no flicker or animation** — the values are static. `S_Lighting::Update` is
  empty and is where a flicker would live
- **no per-light falloff curve** — linear from centre to rim, set by the vertex
  colours of the fan
- **lights are round only** — no cones or directional lights

See also `particles.md` and `GameLL_VFX_Pipeline.pdf`.
