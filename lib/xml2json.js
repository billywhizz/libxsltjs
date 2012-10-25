var Processor = require("../build/Release/processor").Processor;
var fs = require("fs");

var xslt = fs.readFileSync("./lib/libxsltjs/lib/xml2json.xsl", "utf8");
var xmlrx = new RegExp("<\\?xml.*\\?>\\s*");

exports.Converter = function(xslfile) {
  var _self = this;
  _self.lasterror = null;
  var proc = new Processor();
  if(!xslfile) {
    proc.parseString(xslt);
  }
  else {
    proc.parseString(fs.readFileSync(xslfile, "utf8"));
  }
  this.process = function(body, params) {
    try {
      if(body) {
        return JSON.parse(proc.processString(body, params).replace(xmlrx, ""));
      }
      else {
        return JSON.parse(proc.process(params).replace(xmlrx, ""));
      }
    }
    catch(ex) {
      _self.lasterror = ex;
      return null
    }
  }
}
