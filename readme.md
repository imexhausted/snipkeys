Simple app for text snippets for Linux (X11). You can define them in ```settings.config```

Snippet example:
```/key=value```

Here you can see a ```/key``` which you need to type to get ```value``` in any text field of any app which supports pasting.

Also output file accepts CLI arguments ```config``` and ```/path/to/config```
If config is in the same foler you can just not pass the argument

to run you can execute ```./snipclip```
or ```make run``` (recompiles application)

to compile
```g++ ./app.cpp ./clip/clip.cpp ./clip/clip_x11.cpp ./clip/image.cpp -o app -lXi -lX11 -lXtst -lxcb -lxcb-randr -lpthread -I. -DCLIP_ENABLE_IMAGE=1 -DHAVE_XCB_XLIB_H``` 
or ```make cmp```

*if you're unable to compile try to install all dependencies*
```sudo apt install libxcb-randr0-dev libxcb1-dev xcb libpthread-stubs0-dev```

