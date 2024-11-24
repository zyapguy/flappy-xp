# flappy-xp | Flappy Bird for Windows XP
A flappy bird clone made using Windows XP.


## Why?
Mostly because I was bored and wanted to mess around with Windows XP.

## How?
To ensure compatibility, I couldn't use SDL, SFML or similar libraries. Instead, I used Windows GDI, which is built into the Windows API.

## Compatibility
This program should work in Windows 98, 2000, XP, Vista, 7, 8, 10, 11. It won't work on Windows 95, because of the matrix transformations, but you can modify the Bird::Draw function to remove that.

## Building
I wrote and built this using Dev CPP and G++, so those should work. However, since we're not doing any compiler-specific magic, it should probably work with any compiler.