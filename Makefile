CXX=g++

build: ltr_c.cpp
	$(CXX) -std=c++17 -O3 ltr_c.cpp -o ltr_c

clean:
	rm ltr_c

install: build
	mv ltr_c /usr/local/bin/ltr_c
	mkdir -p /usr/local/share/man/man1
	cp ltr_c.1 /usr/local/share/man/man1/ltr_c.1
	mandb

uninstall:
	rm ltr_c /usr/local/bin/ltr_c
	rm ltr_c.1 /usr/local/share/man/man1/ltr_c.1
	mandb