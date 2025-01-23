all: serv client

serv:
	cd server; 	cargo build

client:
	cd SDL; cmake -Bbuild
	cd SDL; cmake --build ./build
tui:
	@make -C core/alchemist-tui/
	
