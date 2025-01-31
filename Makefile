all: serv client

serv:
	cd server; 	cargo build

client:
	cd SDL; ./configure
	@make -C SDL/build -j $(nproc)

tui:
	@make -C core/alchemist-tui/ -j $(nproc)
	
clean:
	cd server; cargo clean
	rm -rf SDL/build

format:
	find . -name \*.h  -exec clang-format -i {}  \;
	find . -name \*.cpp  -exec clang-format -i {}  \;
