clip:
	g++ ./cliptst.cpp ./clip/clip.cpp ./clip/clip_x11.cpp ./clip/image.cpp -o clp -lxcb -lxcb-randr -lpthread -I. -DCLIP_ENABLE_IMAGE=1 -DHAVE_XCB_XLIB_H
	./clp

run:
	g++ ./app.cpp ./clip/clip.cpp ./clip/clip_x11.cpp ./clip/image.cpp -o app -lXi -lX11 -lXtst -lxcb -lxcb-randr -lpthread -I. -DCLIP_ENABLE_IMAGE=1 -DHAVE_XCB_XLIB_H
	./app

