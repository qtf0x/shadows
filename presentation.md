---
author: Vincent Marias
date: MMMM dd, YYYY
paging: "%d / %d"
---

# A Brief History of Rasterized Hard Shadowing Algorithms

<br/><br/>
<br/><br/>

##### Planar Projections
##### Shadow Textures
##### Shadow (Depth) Mapping
##### Percentage-Closer Filtering
##### Shadow Volumes (not today lol)
##### ~~Stencil Volumes~~

---

## Parts of a shadow

Scene points can be:

- **Umbra**: all light samples hidden from view (no direct lighting)
- **Penumbra**: only some light samples hidden from view (some direct lighting)
- **Lit**: no light samples hidden from view (all direct lighting)
	- not part of the shadow

### Hard shadows

- Only the umbra
- Produced by point lights
- Less physically accurate, easier to render

### Soft Shadows

- Umbra + Penumbra
- Produced by area lights
- More physically accurate, harder to render

---

## Planar Projections

- Create special projection matrix (similar to perspective transform)
- Squishes occluder geometry onto a specific plane
- Re-render squished geometry all black

#### The Good

- Relatively cheap for simple geometry
- Sharp shadows w/out aliasing or sampling issues

#### The Bad

- Can only shadow planes
- Must separate occluders and receivers
- Need to draw all occluder geometry twice

---

## Shadow Textures

- Place camera at the light source
- Render occluders into binary framebuffer texture
- Render receivers normally, sampling from texture to determine shadow

#### The Good

- Shadows on curved surfaces!

#### The Bad

- Still need to separate occluders and receivers
- Textures need high resolution to avoid aliasing

---

## Shadow (Depth) Mapping

- Similar to shadow textures, but render out fragment depths to framebuffer
- Compare depth of view samples in light space to recorded depth

#### The Good

- Per-fragment -> Occluders can be receivers (self-shadowing!)
- Basically the same performance as shadow textures

#### The Bad

- Performing signal reconstruction -> LOTS of artifacts
- Fixing one artifact usually means creating another

---

## Percentage-Closer Filtering

Can we reduce aliasing + shadow acne without increasing texture resolution? What about approximating soft shadows?

- Basically need to sample multiple texels from shadow map and combine them
- Use as much surrounding information as possible w/out creating more artifacts
- How do we combine the texels?
	- Many statistical methods
	- Simple average is often good enough

### There are still problems (because of course there are)

- No one sampling distribution ideal for every scene
- Sampling bias can lead to inconsistent results
- Edges of the shadow are fuzzier ...  this is NOT soft shadowing, not remotely physically accurate

---

## References

Real-Time Rendering, 4th Edition (Tomas Akenine-Mo¨ller, Eric Haines, Naty Hoffman)

OpenGL Programming Guide, 9th Edition (John Kessenich et al.)

Real-Time Shadows (Elmar Eisemann et al.)

