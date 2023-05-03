---
author: Vincent Marias
date: MMMM dd, YYYY
paging: "%d / %d"
---

# A Brief History of Real-Time Hard Shadowing Algorithms

<br/><br/>
<br/><br/>

##### Planar Projections
##### Shadow Textures
##### Depth Mapping
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

### Everything happens in your terminal
Create slides and present them without ever leaving your terminal.

---

#### Code execution
```bash
chafa ~Pictures/guy.png
```

You can execute code inside your slides by pressing `<C-e>`,
the output of your command will be displayed at the end of the current slide.

---

##### Pre-process slides
###### H6

You can add a code block with three tildes (`~`) and write a command to run *before* displaying
the slides, the text inside the code block will be passed as `stdin` to the command
and the code block will be replaced with the `stdout` of the command.

```
~~~graph-easy --as=boxart
[ A ] - to -> [ B ]
~~~
```

The above will be pre-processed to look like:

┌───┐  to   ┌───┐
│ A │ ────> │ B │
└───┘       └───┘

For security reasons, you must pass a file that has execution permissions
for the slides to be pre-processed. You can use `chmod` to add these permissions.

```bash
chmod +x file.md
```
