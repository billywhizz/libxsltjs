rm -f lib/processor.node
g++ -O3 -DNDEBUG -fPIC -DPIC -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -I/media/storage/AJohnston/Code/node/git/node-v0.10.1/src -I/media/storage/AJohnston/Code/node/git/node-v0.10.1/deps/uv/include -I/media/storage/AJohnston/Code/node/git/node-v0.10.1/deps/v8/include -I/usr/include/libxml2 src/processor.cpp -c -o processor.o
g++ processor.o -o lib/processor.node -lxml2 -lxslt -shared -L/usr/local/lib
rm -f *.o
