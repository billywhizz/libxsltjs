var fs = require("fs");
var Converter = require("../").JSONConverter;

var c = new Converter();
var doc = fs.readFileSync(process.argv[3] || "./test/test.xml", "utf8");
var run = 0;
var res;
var runs = process.argv[2] || 1;
var then = new Date().getTime();
for(var i=0; i<runs; i++) {
  res = c.process(doc);
}
var now = new Date().getTime();
var secs = (now - then)/1000;
console.log(runs + " in " + secs + " sec. " + (runs/secs) + " runs/sec.");
