# Homework 4 - Triangle Rasterization and Depth Buffering

**Name:** Farida Dabit
**ID:** 212693345
**Course:** Computer Graphics

---

## Part 1 - Bounding Box Rasterization

### Implementation

For this part, I added a debugging rasterization mode that displays the screen-space bounding rectangle of every projected triangle.

After applying the existing model transformation and converting the three triangle vertices to screen coordinates, I calculate the minimum and maximum X and Y values of the projected vertices.

The minimum coordinates are rounded down using `floor`, while the maximum coordinates are rounded up using `ceil`, so the complete projected triangle is contained inside its rectangle.

The resulting rectangle is also clamped to the framebuffer boundaries:

- X coordinates are limited to `0 ... WIDTH - 1`
- Y coordinates are limited to `0 ... HEIGHT - 1`

Each mesh face is assigned a random solid color when the OBJ model is loaded. The colors are stored in `mesh.face_colors`, so the same face keeps the same color while the application is running instead of receiving a new random color every frame.

I added a new UI checkbox:

- `Triangle Bounding Boxes`

When this option is enabled, the renderer loops over every pixel in each triangle's screen-space bounding rectangle and writes the face color directly to `g_buffer`.

When the option is disabled, the original wireframe rendering remains available.

### Verification

I first verified that disabling `Triangle Bounding Boxes` preserves the existing wireframe rendering.

I then enabled the bounding-box visualization and rotated the model using:

- World Rotation X = `20`
- World Rotation Y = `30`
- World Rotation Z = `0`

The resulting image consists of overlapping solid-colored rectangles. Their positions and dimensions change according to the projected triangle vertices as the model rotates, while the rectangles themselves remain aligned with the screen axes, as expected for screen-space bounding rectangles.

![Triangle bounding-box rasterization with the model rotated](./assets/HW4_image1.png)

### Result

The renderer can now calculate and visualize the 2D screen-space bounding rectangle of every triangle in the mesh. This provides the pixel region that will be used in the next part for triangle inclusion testing with Barycentric Coordinates.

---

## Part 2 - Triangle Rasterization with Barycentric Coordinates

### Implementation

In this part, I replaced the bounding-box-only fill from Part 1 with an actual triangle rasterization test based on Barycentric Coordinates.

I first added a helper function named `compute_barycentric(...)` that receives the three triangle vertices in screen space together with a pixel position `(x, y)` and computes the three barycentric weights:

- `alpha`
- `beta`
- `gamma`

The function uses the standard 2D barycentric-coordinate formula in screen space.
I also added a small safety check for degenerate triangles: if the denominator is close to zero, the function returns invalid values instead of dividing by zero.

The rasterization process still begins by computing the screen-space bounding rectangle of each projected triangle, exactly as in Part 1. Then, instead of coloring the entire rectangle, the renderer iterates over all pixels inside the rectangle and computes the barycentric coordinates of each pixel relative to the triangle.

A pixel is filled only if all three barycentric weights satisfy:

- `0 <= alpha <= 1`
- `0 <= beta <= 1`
- `0 <= gamma <= 1`

If these conditions hold, the pixel lies inside the triangle and is colored using the face color stored in `mesh.face_colors`.

To reflect the new behavior, I also renamed the UI checkbox from `Triangle Bounding Boxes` to `Filled Triangles`.

When the option is disabled, the original white wireframe rendering is still shown.

### Verification

I enabled `Filled Triangles` and rotated the model using:

- World Rotation X = `20`
- World Rotation Y = `30`
- World Rotation Z = `0`

The resulting image shows solid-colored triangles instead of the colored axis-aligned rectangles from Part 1. This confirms that pixels are no longer filled only according to the bounding rectangle, but according to actual triangle membership computed with barycentric coordinates.

I also verified that disabling `Filled Triangles` still restores the original wireframe rendering.

At this stage, some triangles may visually overlap in an incorrect front-to-back order. This is expected, because depth testing has not been implemented yet.

![Filled triangle rasterization using barycentric coordinates](./assets/HW4_image2.png)

### Result

The renderer can now rasterize each triangle itself, rather than only its bounding rectangle. Barycentric coordinates are used to determine whether a screen pixel lies inside the triangle, which provides the correct foundation for the next part: interpolating depth values and implementing a Z-buffer.

---
