# Homework 3 — Virtual Cameras and Projections

**Name:** Farida Dabit
**ID:** 212693345
**Course:** Computer Graphics

---

## Part 1 — Coordinate Frames and Bounding Boxes

### Implementation

For this part, I added visual debugging tools for the model's coordinate frames and bounding box.

The existing bounding-box calculation was reused to obtain the minimum and maximum coordinates of the loaded mesh. These values are kept available during rendering so they can also be used for the debug visualization.

I added three UI checkboxes:

- `Show World Axes`
- `Show Local Axes`
- `Show Bounding Box`

The coordinate axes use the standard colors: red for X, green for Y, and blue for Z.

The World axes are drawn from the world origin `(0, 0, 0)` without applying the model transformation. Therefore, they remain fixed when the model is transformed.

The Local axes originate at the model center. Their endpoints are transformed using the same `final_transform_matrix` as the mesh, so they move and rotate together with the model.

For the bounding box, I constructed its eight corners from `min_corner` and `max_corner`. Each corner is transformed with the same model transformation, and the twelve edges of the box are drawn using `draw_line`.

### Verification

First, I enabled only the World axes and translated the model along the world X axis. The model moved while the World axes remained at the original world origin.

![World coordinate axes remain fixed while the model is translated](./assets/HW3_image1.png)

Next, I enabled only the Local axes. I rotated the model around the local X and Y axes and translated it in the world X direction. The Local axes moved and rotated together with the model. With the model rotated in 3D, the red X, green Y, and blue Z directions can all be seen.

![Local coordinate axes transform together with the model](./assets/HW3_image2.png)

Finally, I enabled only the bounding-box visualization and rotated the model around the local X and Y axes. The yellow wireframe bounding box remained aligned with the transformed model.

![Transformed wireframe bounding box](./assets/HW3_image3.png)

### Result

The renderer can now independently display the fixed World coordinate frame, the model's Local coordinate frame, and its 3D wireframe bounding box. These debug visualizations make it easier to distinguish between transformations in world space and transformations relative to the model.

---

## Part 2 — The Virtual Camera (View Matrix)

### Implementation

For this part, I added a simple virtual camera with a position and rotation in world space.

The camera is represented by a `Camera` struct containing two `glm::vec3` values:

- `position`
- `rotation`

I also added UI controls for changing the camera position and rotation along the X, Y, and Z axes.

The camera transform is constructed from its translation and rotation matrices. The View matrix is then calculated as the inverse of the camera transform:

`View = inverse(Camera Transform)`

This inverse is necessary because moving the camera is represented by applying the opposite transformation to the world.

The existing model transformations are applied first. The resulting world-space points are then transformed by the View matrix before being converted to screen coordinates. Therefore, the rendering flow for this part is:

`Model → View → Screen`

This corresponds to the required graphics pipeline order `P * V * M * v`. Perspective projection is not added yet in this part, so the existing screen mapping is still used after the View transformation.

The same View transformation is also applied consistently to the debugging geometry. The transformed bounding box and Local axes first follow the model transformation and then pass through the camera View matrix. The World axes do not receive the model transformation, but they do pass through the View matrix because they are part of the world viewed by the camera.

### Verification

To verify camera translation, I set the camera position to `(-1, 0, 0)` while keeping the camera rotation at zero. Moving the camera to the left caused the model to appear shifted to the right on the screen, which confirms that the inverse camera transformation is being applied correctly.

![Camera translated left, causing the model to appear shifted right](./assets/HW3_image4.png)

I also tested camera rotation by resetting the camera position to `(0, 0, 0)` and setting the Z rotation to `30` degrees. The rendered scene rotated in the opposite direction of the camera rotation, as expected from the inverse View transformation.

![Scene viewed with a 30-degree camera rotation around the Z axis](./assets/HW3_image5.png)

### Result

The renderer now supports a virtual camera with controllable world-space position and rotation. The View matrix is constructed from the inverse camera transformation and is applied consistently to the model and the relevant debugging geometry. Camera translation and rotation were both verified visually.
