var Processor = require("./processor").Processor;
var fs = require("fs");

var xslt = '<?xml version="1.0"?><xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:output method="text"\n\t\t\t\tmedia-type="application/json"\n\t\t\t\tencoding="UTF-8"\n\t\t\t\tindent="no"\n\t\t\t\tomit-xml-declaration="yes" /><xsl:strip-space elements="*"/><xsl:template name="escape-string"><xsl:param name="s"/><xsl:text>"</xsl:text><xsl:call-template name="escape-bs-string"><xsl:with-param name="s" select="$s"/></xsl:call-template><xsl:text>"</xsl:text></xsl:template><xsl:template name="escape-bs-string"><xsl:param name="s"/><xsl:choose><xsl:when test="contains($s,\'\\\')"><xsl:call-template name="escape-quot-string"><xsl:with-param name="s" select="concat(substring-before($s,\'\\\'),\'\\\\\')"/></xsl:call-template><xsl:call-template name="escape-bs-string"><xsl:with-param name="s" select="substring-after($s,\'\\\')"/></xsl:call-template></xsl:when><xsl:otherwise><xsl:call-template name="escape-quot-string"><xsl:with-param name="s" select="$s"/></xsl:call-template></xsl:otherwise></xsl:choose></xsl:template><xsl:template name="escape-quot-string"><xsl:param name="s"/><xsl:choose><xsl:when test="contains($s,\'&quot;\')"><xsl:call-template name="encode-string"><xsl:with-param name="s" select="concat(substring-before($s,\'&quot;\'),\'\\&quot;\')"/></xsl:call-template><xsl:call-template name="escape-quot-string"><xsl:with-param name="s" select="substring-after($s,\'&quot;\')"/></xsl:call-template></xsl:when><xsl:otherwise><xsl:call-template name="encode-string"><xsl:with-param name="s" select="$s"/></xsl:call-template></xsl:otherwise></xsl:choose></xsl:template><xsl:template name="encode-string"><xsl:param name="s"/><xsl:choose><xsl:when test="contains($s,\'&#x9;\')"><xsl:call-template name="encode-string"><xsl:with-param name="s" select="concat(substring-before($s,\'&#x9;\'),\'\\t\',substring-after($s,\'&#x9;\'))"/></xsl:call-template></xsl:when><xsl:when test="contains($s,\'&#xA;\')"><xsl:call-template name="encode-string"><xsl:with-param name="s" select="concat(substring-before($s,\'&#xA;\'),\'\\n\',substring-after($s,\'&#xA;\'))"/></xsl:call-template></xsl:when><xsl:when test="contains($s,\'&#xD;\')"><xsl:call-template name="encode-string"><xsl:with-param name="s" select="concat(substring-before($s,\'&#xD;\'),\'\\r\',substring-after($s,\'&#xD;\'))"/></xsl:call-template></xsl:when><xsl:otherwise><xsl:value-of select="$s"/></xsl:otherwise></xsl:choose></xsl:template><xsl:template match="/"><xsl:text>{</xsl:text><xsl:apply-templates select="*"/><xsl:text>}</xsl:text></xsl:template><xsl:template match="*"><xsl:variable name="childName" select="name()"/><xsl:choose><xsl:when test="count(../*[name()=$childName]) > 1 and preceding-sibling::*[name()=$childName]"></xsl:when><xsl:otherwise><xsl:text>"</xsl:text><xsl:value-of select="name()"/><xsl:text>":</xsl:text></xsl:otherwise></xsl:choose><xsl:call-template name="Properties"/></xsl:template><xsl:template match="*" mode="ArrayElement"><xsl:call-template name="Properties"/></xsl:template><xsl:template name="Properties"><xsl:variable name="childName" select="name()"/><xsl:choose><xsl:when test="not(*|@*)"><xsl:choose><xsl:when test="not(string(number())=\'NaN\' or (starts-with(.,\'0\' ) and . != \'0\'))"><xsl:value-of select="."/></xsl:when><xsl:otherwise><xsl:call-template name="escape-string"><xsl:with-param name="s" select="."/></xsl:call-template></xsl:otherwise></xsl:choose></xsl:when><xsl:otherwise><xsl:if test="count(../*[name()=$childName]) > 1 and not(preceding-sibling::*[name()=$childName])"><xsl:text>[</xsl:text></xsl:if><xsl:text>{</xsl:text><xsl:for-each select="@*"><xsl:text>"</xsl:text><xsl:value-of select="name(.)"/><xsl:text>":</xsl:text><xsl:choose><xsl:when test="not(string(number())=\'NaN\' or (starts-with(.,\'0\' ) and . != \'0\'))"><xsl:value-of select="."/></xsl:when><xsl:otherwise><xsl:call-template name="escape-string"><xsl:with-param name="s" select="."/></xsl:call-template></xsl:otherwise></xsl:choose><xsl:if test="position() != last()"><xsl:text>,</xsl:text></xsl:if></xsl:for-each><xsl:if test="count(*) > 0 and count(@*) > 0"><xsl:text>,</xsl:text></xsl:if><xsl:apply-templates select="*"/><xsl:text>}</xsl:text><xsl:if test="count(../*[name()=$childName]) > 1 and not(following-sibling::*[name()=$childName])"><xsl:text>]</xsl:text></xsl:if></xsl:otherwise></xsl:choose><xsl:if test="following-sibling::*"><xsl:text>,</xsl:text></xsl:if></xsl:template></xsl:stylesheet>';
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