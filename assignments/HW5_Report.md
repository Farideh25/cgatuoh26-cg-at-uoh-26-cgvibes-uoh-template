# Homework 5 - Lighting, Materials, and Shading

**Name:** Farida Dabit
**ID:** 212693345
**Course:** Computer Graphics

---

## Part 1 - Light Sources and Material Properties

### Implementation

For this part, I added the basic light and material representation required for the lighting pipeline.

I created a `PointLight` struct containing:

- A 3D light position
- Ambient RGB color
- Diffuse RGB color
- Specular RGB color

I also created a `Material` struct containing corresponding Ambient, Diffuse, and Specular RGB properties. The color properties are stored as `glm::vec3` values in the range `0 ... 1`, which allows the RGB components to be used directly in lighting calculations.

A point light and a material were then initialized as part of the application state.

I added a new `Lighting` UI window that allows the light properties to be modified while the application is running. The window contains controls for:

- Light Position X, Y, and Z
- Ambient R, G, and B
- Diffuse R, G, and B
- Specular R, G, and B

At this stage, only the Ambient component is used by the renderer.

The Ambient lighting color is calculated by component-wise multiplication of the light's Ambient color and the material's Ambient color:

`ambient_color = point_light.ambient * material.ambient`

The resulting RGB values are clamped to the range `0 ... 1` and converted to the framebuffer's `0 ... 255` RGB representation before the triangle pixels are written.

The existing triangle rasterization and Z-buffer logic from Homework 4 remain unchanged. Only the solid triangle color was replaced by the calculated Ambient lighting color.

### Verification

I enabled `Filled Triangles` and disabled `Show Z-Buffer`.

For the screenshot, I used the following light settings:

- Light Position X = `2.0`
- Light Position Y = `2.0`
- Light Position Z = `7.0`
- Ambient R = `1.0`
- Ambient G = `0.0`
- Ambient B = `0.0`

The material Ambient color was initialized to:

- Material Ambient = `(0.7, 0.2, 0.2)`

With only the red Ambient light component enabled, the complete model is rendered with a uniform red color. There are no brightness differences between the individual faces, which is expected because no directional lighting calculation is performed in this part.

I also changed the Ambient RGB controls individually and verified that the rendered color responds to those UI changes.

To confirm that only Ambient lighting is active, I changed the Light Position as well as the Diffuse and Specular controls. These changes did not affect the rendered model, as expected at this stage.

![Ambient lighting controlled through the Lighting UI](./assets/HW5_image1.png)

### Result

The renderer now contains explicit Point Light and Material properties and supports interactive control of the light through the UI.

Ambient lighting is calculated from the light and material Ambient colors and is used as the solid model color. The result is intentionally flat and independent of the light position, providing the foundation for the directional Diffuse lighting that will be implemented in Part 2.

---
