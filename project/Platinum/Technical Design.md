# Chemlabs Technical Design.md

## Purpose

Define APIs and goals to direct work.

## Scope

Prototype requirements.

## Art & Style

* "Gumball" hodgepodge. Animated, real-life, pixilated, 3D.

![Poke](https://serebii.net/pokearth/maps/sinnoh-pt/64.png)

![Poke](https://cdn.bulbagarden.net/upload/4/48/Twinleaf_Town_Pt.png)

* Fixed style elements
  * 3D Elements
    * Land 
    * Buildings
  * 2D Elements
    * Characters (pixilated, 2D)
    * Ground texture tiles

## Graphics

* Isometric
* Front views of buildings only.
* Enemy behind building (can't see?)
  * Render building transparent.
 
## Data Format

* Sloped voxel grid 
* Infinite grid.
* Monsters & Items 
  * Stored in text file JSON
 
## Tools

### World Editor & Game Runner

Built into the game itself

#### Runner Specs

* F5 - Test game.

#### Editor Specs

  * Right click - build up the land.
  * Left click - destroy the land.
  * Click a sprite and click the land to place the sprite.
  
### Cinematics & Enemy moves & Definitions & User Interface

* Scripted in LUA

## API(s)

* SDL 2 & OpenGL 4.6

## Platforms

* Windows & Linux










