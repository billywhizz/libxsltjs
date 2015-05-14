{
  "targets": [
    {
      "target_name": "processor",
      'product_extension': 'node',
      'type': 'shared_library',
      "include_dirs": [
        "/usr/include/libxml2",
        "/usr/include/libxslt"
      ],
      "libraries": [ "-lxml2", "-lxslt" ],
      "sources": [ "src/processor.cc" ]
    }
  ]
}

