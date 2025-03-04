all: rust cpp

rust:
	cd rust && cargo rustc --release -- -C target-cpu=native -C opt-level=2 && cd ..	

cpp:
	sudo g++ -O2 cpp/simple.cpp -o cpp/fileout -lpapi

clean:
	rm -rf rust/target  cpp/fileout rust/src/fileout cpp/simple.csv rust/src/rust_output.csv

run:
	cd rust && cargo run --release
	./cpp/fileout

rebuild: clean all
