# In focus
- [ ] Loading a png and converting to particles however that would be done.
# Project things
- [ ] Setup a configuration for release build. **Note:** Somewhat done but the libraries also needs to be compiled with release configs.
- [ ] Fix icon for executable.
- [ ] Undecorated window. Looks cooler.
- [ ] Transparent window. Drawing ontop of a text editor is cool. Or a game.
- [ ] Organize todos before they get out of hand. Organize by importance or category? Maybe both?
# Quality of life
- [ ] Exponential undo, redo speed when holding key.
- [ ] Help information
- [ ] Origin point or cross showing the center of the canvas.
- [ ] A quick smaller color panel that shows up beside the mouse instead of bottom left. Other quick menus with brushes, rectangles?
- [ ] Multiple saved brushes so you can quickly switch colors and sizes. When switching brushes, the same way they maintain size and colorEach, they also remember which layer they were on. All this info should be visible when changing brush. Brush is changed by number keys.
- [ ] Saving and loading images suck. A list of choices recent choices or a prompt of navigating to your folder using the native file explorer would be nice. In any case, a quicker and more pleasant way of saving and loading images is required.
- [ ] Some limit or standardization for zoom and size. If you balance them incorrectly your canvas will drop performance. This is not something you should have to think about. Perhaps size can be 1/10 of the zoom at minimum.
- [ ] Option to automatically create an image when saving to the `.drw` format.
# Special features
- [ ] Area select and move. Move pixels/particles. Rectangle selection? Implementation could specify ranges of particles to select and move instead of a list of all particles within the area.
- [ ] Grid generation. As a programmer you usually use grids when explaining certain things. Not having to draw the grid every time would be convenient.
- [ ] Drawing rectangles
- [ ] Drawing text
- [ ] Line tool which uses the brush functionality.
- [ ] Eraser, 1.5 times bigger than brush? Instead of removing particles from the list you could add particles with the same color as the background. These particles can be rendered with the same color as the background. If a particle has negative size then it will be rendered with the same color as the background. 
- [ ] Import images and being able to resize and move them.
- [ ] Layers, it should be easy to switch between them. like a popup menu beside your cursor.
- [ ] Keybindings
- [ ] Plugin support?
# Other
- Some what random starting color themes. They are chosen from a set of good looking themes.
# Optimizations
- [ ] Pause program and don't render if minimized or out of focus.
- [ ] Prune algorithm which deletes duplicate particles. This can run on a few threads and on a part of the buffer. When pruning holes in the buffer will be left from the removed particles. They should be filled but how? Moving particles from the back would invalidate Ctrl+Z.
- [ ] Compress algorithm when saving data to disk. Each particle has position and color. Instead you can save positions for each color. White may have 1023 positions. Blue has 99. This saves 16 bytes for each particle (4x floats for each color). You can compress a block of particles.
- [ ] Quad tree for the particles?
- [ ] Set a maximum zoom. A drag draw zoomed out draws many more particles then zoomed in. Too far out would generate kilobytes of particles per frame.
- [ ] Enable GL_PROGRAM_POINT_SIZE and give each particle a size. Draw one large particle instead of many small ones. Enable GL_POINT_SMOOTH for rounded points (smoothing doesn't work, maybe i have old PC?).
- [ ] Performance problems with high zoom and small brush size. Can be fixed a little bit by having multiple shaders buffers, multiple allocations instead of one large. Some multithreading on some parts? I don't think this will solve the real issue though. There will still be a limit. What you should do is set a maximum zoom. Maybe an option when launching the program do disable all limits?
# Completed
- [x] Bake text font and marker.png into the executable. **Note:** Done with MSVC Resource compiler.
- [x] Fix fps counter
- [x] Save/Load text should change size depending on window size.
- [x] Increment/Decrement brush size faster. Same expontial as zooming?
- [x] Can't move when exporting png/saving image and such.
- [x] Save background color with .drw.
- [x] Notification text move to right side.
- [x] Save as png. You get to define a rectangle where particles inside are saved to the image. You can also define the precision (matters if you zoomed out and drew stuff but don't want to get a 30000x30000 image). You may also define pixel width and height. With this you can "squish" the image into odd aspect ratio. You can also use a fixed ratio where either the rectangle height or image height changes.
- [x] Saving png is super slow with many particles. Instead of rasterizing particles with CPU use GPU to draw particles to an image like normal then read and output that image.
- [x] Change background color
- [x] Redo key (CTRL+Z). Instead of removing entries in history you decrement an index. Redo would increment the index. If you make a change then entries after the index would be deleted.
- [x] Autosave and auto load last drawing? autosave.drw
- [x] Ctrl+Z, remove particles from the back of the buffer.
- [x] Change brush size
- [x] Change brush color.