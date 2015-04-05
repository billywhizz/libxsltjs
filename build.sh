#NODE_HOME=/media/storage/AJohnston/Code/node/git/node-v0.10.1
NODE_HOME=/media/edge1/storage4/source/js/node-v0.10.26
XML_HOME=/usr/include/libxml2
rm -f lib/processor.node
g++ -O3 -DNDEBUG -fPIC -DPIC -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -I$NODE_HOME/src -I$NODE_HOME/deps/uv/include -I$NODE_HOME/deps/v8/include -I$XML_HOME src/processor.cpp -c -o processor.o
g++ processor.o -o lib/processor.node -lxml2 -lxslt -shared -L/usr/local/lib
rm -f *.o
