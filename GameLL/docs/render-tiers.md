# What gets post processed, and what does not

There are three tiers on screen. Getting a thing into the wrong one is the most
common way to make the post processing look broken, so this is the first thing
to know before adding anything that draws.

```
  scene target ──► x light map ──► bright pass ──► blur ──► composite ──► window
                                                                            │
  overlay states ───────────────────────────────────────────────────────────►│
                                                                            │
  GUI ──────────────────────────────────────────────────────────────────────►│
```

| tier | what is in it | post processed |
|---|---|---|
| **scene** | the world, entities, particles, lighting, the intro splash | yes |
| **overlay** | the pause screen | no |
| **GUI** | menus, the editor panels | no |

## Which tier should my thing be in?

**Is it part of the world, or a picture of it?** Then it is scene content, and
it should go through the chain. Draw it into `Window::GetSceneTexture()` from
your state's `Draw()`. The intro splash is in this tier: it is a picture, so it
should carry the same grain and vignette as everything else.

**Is it furniture laid over the world — a label, a dim, a HUD?** Then it is an
overlay. Override `BaseState::DrawOverlay()` and draw to
`Window::GetRenderWindow()` with the UI view. It runs after the scene is
finished, so nothing touches it.

The pause screen is the worked example of why this matters. It started out in
the scene, and the result was that the lighting multiply darkened the word
PAUSED depending on where the player happened to be standing, and the film
grain crawled over the label. It is furniture, not world, so it moved to the
overlay tier and the label is now flat and legible.

## The two mistakes that are easy to make

**Drawing to the window from `Draw()`.** `Window::DisplayScene` blits the
finished scene over the window afterwards, so anything drawn straight to the
window in the scene phase is painted over and vanishes. If your thing
disappears, this is why. Either draw into the scene target, or move it to
`DrawOverlay()`.

**Assuming `getDefaultView()` means the current window.** It does not. SFML
fixes the default view at the size the window was *created* with and never
updates it on resize. A full screen pass wants `Window::GetUIView()`, which is
reset to the live size on every resize.

## The knobs

All on `Window`, all safe to call at runtime.

| setter | default | what it does |
|---|---|---|
| `SetVignetteStrength` | 0.6 | darkening towards the corners |
| `SetGrainStrength` | 0.10 | per pixel noise, animated |
| `SetAberrationStrength` | 0.009 | colour fringing towards the edges |
| `SetScanlineStrength` | 0.5 | horizontal scanlines |
| `SetColorGrade` | identity | lift / gamma / gain, per channel |
| `SetBloomThreshold` | 0.1 | how bright a pixel must be to glow |
| `SetBloomIntensity` | 0.2 | how much glow is added back |
| `SetLightingEnabled` | true | the light map multiply |

`SetBloomThreshold` is the one to reach for first. At 0.1 almost everything
glows a little; raising it towards 0.5 restricts the glow to genuinely bright
things like lights and particles.

## Everything degrades

No pass is mandatory:

- **no GLSL support at all** — `loadFromFile` fails for every shader (SFML
  checks `sf::Shader::isAvailable()` internally), the pointers stay null, and
  `DisplayScene` blits the scene plainly. Lighting still works, because it is a
  blend rather than a shader.
- **only some shaders load** — bloom drops out on its own if its two shaders
  are missing while the composite still runs.
- **a state that builds no light map** — the map is cleared to white each frame
  and multiplying by white changes nothing.

So a missing shader costs an effect, never the frame.

One rough edge: on a machine with no shader support this reports itself as
three separate `! Failed to load shader` lines on stderr rather than one
statement that the driver cannot do shaders. An explicit
`sf::Shader::isAvailable()` check in `ShaderManager::Load` would make that
diagnosis a lot faster.

## Adding a new pass

Full screen passes live in `PostProcessor`. A new one needs a `.frag` in
`Assets/Shaders/`, an id in `nav/shaders.cfg`, a `RequireResource` in
`PostProcessor::LoadShaders`, and a working target if it cannot write in place.
Size the target in `PostProcessor::OnResize`, which is already called from
`Window::OnResize` at the same point the scene target is sized.

Order matters more than anything else. Lighting sits before the bright pass on
purpose; putting it after made every surface glow equally regardless of whether
it was lit.

See also `lighting.md`, `particles.md` and `GameLL_VFX_Pipeline.pdf`.
