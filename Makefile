##Tail
prebuild:
	smdcatalog
debug:

qrun:
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j$(nproc) && ./unit_test  && ./easy_video
test:
	rm -rf /tmp/file_info /tmp/result_info /tmp/reducer.cache
	cd build && cmake .. && make -j$(nproc)	&& ctest

install:
clean:
	rm -rf /tmp/file_info /tmp/result_info /tmp/reducer.cache

