# opengl-voxel-scene
opengl-voxel-scene is a final project for the Computer Graphics course.

The project presents a model of an island (farm) based on the Skyblock concept from Minecraft. The scene consists of the following models: the island with windmills, the underside of the island, and two windmill propeller models. The user can move around the scene using the WASD keys and rotate the camera with the mouse.

## Models used in the project (created in Blender)

<img src="./readme_img/01_blender_blocks.png" width="600px">
<img src="./readme_img/02_blender_trees.png" width="600px">

### A single texture was used for texturing.

Textures from the [Faithful Texture Pack](https://faithfulpack.net) \
accessed: 16.03.2026

<img src="./textures/combined_minecraft_texture.png" width="600px">

## Scene visualization in Blender

<img src="./readme_img/03_blender_scene.png" width="600px">

## Expectations (Blender scene) vs Reality (OpenGL scene)

<table width="100%">
  <tr>
    <td width="50%">
      <img src="./readme_img/04_expectations.png" width="100%">
    </td>
    <td width="50%">
      <img src="./readme_img/05_reality.png" width="100%">
    </td>
  </tr>
</table>

## Applied activatable post-processing filters

a) no filter  
b) negative  
c) grayscale  
d) red, green, blue filters  
e) edge filter

<img src="./readme_img/06_filters_1.png" width="600px">
<img src="./readme_img/07_edge_filter.png" width="300px">

## Demo video

See demo video `./project-showcase.mkv`