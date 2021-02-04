# SDL Tetris

A very basic implementation of tetris in C. Probably very buggy and not very featureful.

## To Run

You need SDL2 and SDL_ttf2 installed (and in the system include path). The command I used to run was:

```
clang -Wall -O3 main.c -lsdl2 -lsdl2_ttf && ./a.out
```

Depending on your OS/method of installing the libraries, this may be different for you.

## Controls

Left and Right arrow keys to move left and right.
Up arrow key to rotate.
Down to speed up piece falling.

## Screenshots

![First screenshot](/media/screenshot-one.png)
![Second screenshot](/media/screenshot-two.png)
