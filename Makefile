all: serv client

serv:
	cd server; 	cargo build

client:
	cd SDL; ./configure
	@make -C SDL/build

tui:
	@make -C core/alchemist-tui/
	
clean:
	cd server; cargo clean
	rm -rf SDL/build

