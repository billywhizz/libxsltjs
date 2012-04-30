import Options
from os import unlink, symlink, popen
from os.path import exists 

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")
  opt.add_option('--mode', action='store', default='release', help='Compile mode: release or debug')

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.env['LINKFLAGS'] = ['-lxml2', '-lxslt']
  if Options.options.mode == 'release':
    conf.env['CXXFLAGS'] = ['-O3', '-DNDEBUG', '-Wall', '-I/usr/include/libxml2']
  if Options.options.mode == 'debug':
    conf.env['CXXFLAGS'] = ['-g', -Wall, '-I/usr/include/libxml2']

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "processor"
  obj.source = "src/processor.cc"
  obj.uselib = "xml2,xslt"

def shutdown():
  if Options.commands['clean']:
    if exists('lib/processor.node'): unlink('lib/processor.node')
  else:
    if exists('build/Release/processor.node') and not exists('lib/processor.node'):
      symlink('../build/Release/processor.node', 'lib/processor.node')