# What's up?
This a program for sketching and laying out your thoughts.
It excels at scribbling garbage on an infinite canvas.

# Limits
You draw pixels/particles and eventually you will draw to much in which case
the program will slow down.

# Features
- Color picker
- Resizing brush (scroll wheel)
- Undo (CTRL+Z), redo (CTRL+R)
- Save and load drawings (CTRL+SHIFT+S, CTRL+L)
- Export png (CTRL+E)

# Building the project (only on windows with vcvars64.bat)
Running build.bat will compile the project.
This only works if you have vcvars64.bat in your environment variables.

In build.batch you can change the following line: call vcvars64.bat > nul
to something like the line below:
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" > nul

Once compiled you will find the executable in the bin folder.