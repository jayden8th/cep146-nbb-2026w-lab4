CC = clang
CFLAGS = -I/opt/homebrew/opt/raylib/include
LDFLAGS = -L/opt/homebrew/opt/raylib/lib -lraylib

jlist2: jlist2.c
	$(CC) jlist2.c -o jlist2 $(CFLAGS) $(LDFLAGS)

clean:
	rm -f jlist2
