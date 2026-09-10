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
