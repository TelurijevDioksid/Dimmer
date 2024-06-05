build:
	@gcc dimmer.c -o ./bin/dimmer.exe -Wall -lgdi32

run:
	@bin/dimmer.exe

debug:
	@gcc dimmer.c -o ./bin/dimmer.exe -Wall -lgdi32 -g
	@gdb ./bin/dimmer.exe

