# Minesweeper

This is the classic game of Minesweeper, written in C++ using SFML. 

## How to Play

To get into the game, simply download `minesweeper.exe` from the releases tab and will be ready to play out of the box. The game is primarily built for Windows and may or may not work on Mac or Linux (though I intend to add support later).

## Rules

Click on the smiley face to start a new game. The playing field starts as a grid of unopened tiles, left click on a tile to reveal it. If the tile shows a number, there are that many mines in the surrounding 8 tiles. You can right click a tile to flag it if you think there is a mine there. 

The goal of the game is to reveal all tiles without mines, or flag all the mines. Be careful, if you reveal a tile with a mine, it is game over!

## Menu

Press `esc` to open/close the menu. These are the options available:

- Zoom enabled - Enable/disable zooming (scroll to zoom, click and drag to pan)
- Chording enabled - Enable/disable chording with left-click (see [chording](https://en.wikipedia.org/wiki/Chording#Minesweeper_tactic))
- Game width/height* - Change the width/height of the playing field
- Number of bombs* - Change the number of mines on the field
- Automatic opening - Automatically open a zero tile when creating a new game

\**these changes go into effect at the start of the next game*