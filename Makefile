build:
	gcc -Wall -Werror -fsanitize=address -std=c99 ./src/*.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -lSDL2_TTF -o game

run:
	./game

clean:
	rm game
	