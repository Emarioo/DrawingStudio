
# API for customizing shader?
You can do rainbow stuff with each pixel.

# Other design
- Undecorated window. Looks cooler.
- Transparent window. Drawing ontop of a text editor is cool.

# Features
- Help information
- Multiple saved brushes so you can quickly switch colors and sizes.
- Line tool which uses the brush functionality.
- Eraser, 1.5 times bigger than brush?
- Change background color
- Save as png. You get to define a rectangle where particles inside are saved to the image. You can also define the precision (matters if you zoomed out and drew stuff but don't want to get a 30000x30000 image). You may also define pixel width and height. With this you can "squish" the image into odd aspect ratio. You can also use a fixed ratio where either the rectangle height or image height changes. ALSO, when saving, run compression algorithms.
- When loading, you should get a list of choices. Very quick to load a drawing. You can also manually type in a path.
- Start color themes. Black/White, White/Dark Blue...
- Layers, it should be easy to switch between them. like a popup menu beside your cursor.
- Keybindings
- Redo key (CTRL+Z). Instead of removing entries in history you decrement an index. Redo would increment the index. If you make a change then entries after the index would be deleted.
- Autosave and auto load last drawing? autosave.drw
- [x] Ctrl+Z, remove particles from the back of the buffer.
- [x] Change brush size
- [x] Change brush color.

# Special features
- Grid generation. As a programmer you usually use grids when explaining certain things. Not having to draw the grid every time would be convenient.

# Optimizations
- Prune algorithm which deletes duplicate particles. This can run on a few threads and on a part of the buffer. When pruning holes in the buffer will be left from the removed particles. They should be filled but how? Moving particles from the back would invalidate Ctrl+Z.
- Compress algorithm when saving data to disk. Each particle has position and color. Instead you can save positions for each color. White may have 1023 positions. Blue has 99. This saves 16 bytes for each particle (4x floats for each color). You can compress a block of particles.
- Quad tree for the particles?
- Set a maximum zoom. A drag draw zoomed out draws many more particles then zoomed in. Too far out would generate kilobytes of particles per frame.
- Enable GL_PROGRAM_POINT_SIZE and give each particle a size. Draw one large particle instead of many small ones. Enable GL_POINT_SMOOTH for rounded points (smoothing doesn't work, maybe i have old PC?).