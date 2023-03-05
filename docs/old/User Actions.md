## Camera movement
Zoom, drag, smooth when resizising window

## Keybindings

## Camera pen
Se camera pen for more information -> [CameraPen]

## Graphics Overlay
A header with ui things.

option to utilize gpu when drawing. Using frame buffers and whatnot. Fill and big brushes would be smoother and not take as much performance on the cpu.
## The studio
Load draw actions to a buffer/array and in the game update loop run those actions.
### Image
A texture which has a width, height and possibly multiple layers. 

### Layout
Here you can add, move and resize images. You can combine them and snap them to a grid. This is done by special tools and by selecting other "drawing" tools you edit the image itself.

### Animation
Allow swithing between layers and movement of images.

### Session data
History data which hasn't been touched for a while can be written to disk and if the user goes back in history and needs the data it will be loaded.

### Color palette/picker
You can change color and create a color palette. Can be opened with a shortcut. Can also select a color from the color palette with number keys.

### Tools
Can be selected through a circle menu or shortcuts. Circle menu can be customized.
Often used tools like brush, wand, eraser will probably be shortcut while fill, moving selection will be circle menu.

Improve bound check. Right now it is checked for ever drawn pixel but you cut the line if you draw one so the pixels outside won't be drawn.

#### Pencil
Painting one pixel.

#### Brush
Same as pencil but you can select a size. The brush can be sharp which will replace the area with the color or it can be smooth which will create transparency towards the edge of the brush.

#### Eraser
Same as brush but erase.

#### Fill
When pressing the process starts by filling the color you pressed and continues to areas depending on the tolerance. The filling is the selected color. If you have a selection the selection area will be the only one to be filled.

#### Selection
With this you can select an area. It can be on or more selections which are rectangular, lasso, magic wand. Any drawing outside the selection will be ignored.
