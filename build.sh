rm -f lib/processor.node
g++ -O3 -DNDEBUG -fPIC -DPIC -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -I/usr/local/include/node -I/usr/include/libxml2 src/processor.cpp -c -o processor.o
g++ -flat_namespace processor.o -o build/Release/processor.node -lxml2 -lxslt -shared -L/usr/local/lib
rm -f *.o