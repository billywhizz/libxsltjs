#!/usr/bin/env node
var Builder = require('mnm');
var builder = new Builder();
builder.appendUnique('CXXFLAGS', '-Isrc/');
builder.appendUnique('CXXFLAGS', '-O3');
builder.appendUnique('CXXFLAGS', '-DNDEBUG');
builder.appendUnique('CXXFLAGS', '-Isrc/');
builder.appendUnique('CXXFLAGS', [ '-I/usr/include/libxml2' ]);
builder.failIfNotExists('/usr/include/libxslt/xslt.h', 'Could not find "%s".');
builder.appendLinkerLibrary("xml2");
builder.appendLinkerLibrary("xslt");
builder.target = "processor"
builder.appendSourceDir('./src');
builder.run();