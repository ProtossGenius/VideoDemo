##Tail
prebuild:
	smdcatalog
debug:

qrun:
	rm -rf /tmp/file_info /tmp/result_info /tmp/reducer.cache
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(nproc) && ./unit_test  && ./main 
test:
	rm -rf /tmp/file_info /tmp/result_info /tmp/reducer.cache
	cd build && cmake .. && make -j$(nproc)	&& ctest

install:
clean:

