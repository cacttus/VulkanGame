# Chemlabs Technical Design.md

## Purpose
This document is to aid in the programming of this game. It defines APIs and goals that will help direct the work
for this game.

## Scope
The scope of this document is the entire first round of development on this project including the prototype.

## Graphics, Art & Style
* The aesthetic mimics that of Pokemon Platinum mixed with "Gumball" hodgepodge of graphics.
  * Anime characters, or real-life. Pixelated or 3D.
![alt text](https://serebii.net/pokearth/maps/sinnoh-pt/64.png)
* Some elements are fixed to 3D or 2D.
  * 3D Elements
    * Ground - The entire game world is 3D (top-down perspective). See Pokemon Platinum images for examples. The world is textured with 2D Tiles however.
    * Buildings - Buildings are 3D.
  * 2D Elements
    * Most characters will be 2D sprites. Especially the main character. 
    * Tiles on the world are 2D.
 
## Data Format

* The world will be organized into a "sloped" voxel grid giving us the ability to quickly generate the worlds contents. 
* The grid is infinite with as many layers as is needed to represent the game world. 
* **Battles**
* Battles appear from within the game world itself.
 
## Tools

Tools that will aid in creation of the game.

### World Editor & Game Runner

The world editor will be built into the game itself. The editor must consist of a few key elements.

#### Runner Specs

* When the player presses F5 the game turns off debug mode and switches to the "run mode" of the game.

#### Editor Specs
  * 1. Right click mouse to build up the land.
  * 2. Left click to destroy the land.
  * 3. Click a sprite and click the land to place the sprite.
### Cinematics
* Cinematics are also built into the game world

## API(s)
* SDL 2 & OpenGL 4.6

## Platforms
* Windows & Linux
  * These will be portable using SDL.

