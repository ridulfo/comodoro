all: comodoro

libraylib.a:
	cd raylib/src && make -j`nproc` && mv libraylib.a ../..

raygui/src/raygui.c:
	cp raygui/src/raygui.h raygui/src/raygui.c

comodoro: main.c libraylib.a raygui/src/raygui.c
	cc -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -Iraylib/src -Iraygui/src -o comodoro main.c raygui/src/raygui.c libraylib.a
