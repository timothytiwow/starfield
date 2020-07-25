# starfield
software rendered starfield

# building
this project uses `SDL2` and requires `libsdl2-dev`

#### compiling

```
gcc -o starfield main.c `pkg-config --cflags --libs sdl2` -lm
```
#### running

```
./starfield
```

**note :** use a very small value for the stars speed (0.001 or such)