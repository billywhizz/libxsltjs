#include <node.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

using namespace v8;
using namespace node;
extern int xmlLoadExtDtdDefaultValue;

namespace libxsltjs {
  static Persistent<FunctionTemplate> constructor_template;
  class XslProcessor : public ObjectWrap {
    private:
    
    public:
    static void Initialize(Handle<Object> target) {
      HandleScope scope;
      Local<FunctionTemplate> t = FunctionTemplate::New(XslProcessor::New);
      constructor_template = Persistent<FunctionTemplate>::New(t);
      t->InstanceTemplate()->SetInternalFieldCount(1);
      NODE_SET_PROTOTYPE_METHOD(t, "parseString", XslProcessor::ParseString);
      NODE_SET_PROTOTYPE_METHOD(t, "processString", XslProcessor::ProcessString);
      NODE_SET_PROTOTYPE_METHOD(t, "process", XslProcessor::Process);
      NODE_SET_PROTOTYPE_METHOD(t, "free", XslProcessor::Free);
      NODE_SET_PROTOTYPE_METHOD(t, "lastError", XslProcessor::LastError);
      target->Set(String::NewSymbol("Processor"), t->GetFunction());
      xmlSubstituteEntitiesDefault(1);
    }

    static void PushToArray(void *errs, xmlError *error) {
      HandleScope scope;
      Persistent<Object> errors = reinterpret_cast<Object*>(errs);
      Handle<Function> push = Handle<Function>::Cast(errors->Get(String::NewSymbol("push")));
      Local<Object> err = Object::New();
      if(error->domain != NULL) err->Set(String::New("domain"), Integer::New(error->domain));
      if(error->code != NULL) err->Set(String::New("code"), Integer::New(error->code));
      if(error->message != NULL) err->Set(String::New("message"), String::New(error->message));
      if(error->level != NULL) err->Set(String::New("level"), Integer::New(error->level));
      if(error->file != NULL) err->Set(String::New("file"), String::New(error->file));
      if(error->line != NULL) err->Set(String::New("line"), Integer::New(error->line));
      if(error->str1 != NULL) err->Set(String::New("str1"), String::New(error->str1));
      if(error->str2 != NULL) err->Set(String::New("str2"), String::New(error->str2));
      if(error->str3 != NULL) err->Set(String::New("str3"), String::New(error->str3));
      if(error->int1 != NULL) err->Set(String::New("int1"), Integer::New(error->int1));
      if(error->int2 != NULL) err->Set(String::New("int2"), Integer::New(error->int2));
      Handle<Value> argv[1] = { err };
      push->Call(errors, 1, argv);
    }
    
    protected:
    static Handle<Value> ProcessString(const Arguments& args) {
      HandleScope scope;
      int nbparams = 0;
      const char *params[16 + 1];
      
      if (!(args[0]->IsString())) return ThrowException(Exception::Error(String::New("Must supply ProcessString with a string")));
      if (args[1]->IsObject()) {
        Handle<Object> attributes = args[1]->ToObject();
        Handle<Array> props = attributes->GetPropertyNames();
        for (unsigned int i = 0; i < props->Length(); i++) {
          String::Utf8Value name(props->Get(Number::New(i)));
          String::Utf8Value value(attributes->Get(props->Get(Number::New(i))));
          params[nbparams++] = strdup(*name);
          params[nbparams++] = strdup(*value);
        }
      }
      params[nbparams] = NULL;
      
      XslProcessor *processor = ObjectWrap::Unwrap<XslProcessor>(args.Holder());
      
      String::Utf8Value strxml(args[0]->ToString());
      if(processor->xml_doc) xmlFreeDoc(processor->xml_doc);

      Local<Array> errors = Array::New();
      //processor->errors = Persistent<Array>::New(errors);
      xmlResetLastError();
      xmlSetStructuredErrorFunc(reinterpret_cast<void *>(*errors), XslProcessor::PushToArray);

      processor->xml_doc = xmlReadMemory(*strxml, strxml.length(), NULL, "UTF-8", 0);
      xmlDoc *xmlres = xsltApplyStylesheet(processor->xsl_proc, processor->xml_doc, params);
      if(xmlres == NULL) {
        return scope.Close(Null());
      }
      else {
        xmlChar* buffer = NULL;
        int len = 0;
        xmlDocDumpFormatMemoryEnc(xmlres, &buffer, &len, "UTF-8", 0);
        Local<String> str = String::New((const char*)buffer, len);
        xmlFree(buffer);
        xmlFreeDoc(xmlres);
        return scope.Close(str);
      }
    }
    
    static Handle<Value> Process(const Arguments& args) {
      HandleScope scope;
      int nbparams = 0;
      const char *params[16 + 1];
      if (args[0]->IsObject()) {
        Handle<Object> attributes = args[0]->ToObject();
        Handle<Array> props = attributes->GetPropertyNames();
        for (unsigned int i = 0; i < props->Length(); i++) {
          String::Utf8Value name(props->Get(Number::New(i)));
          String::Utf8Value value(attributes->Get(props->Get(Number::New(i))));
          params[nbparams++] = strdup(*name);
          params[nbparams++] = strdup(*value);
        }
      }
      params[nbparams] = NULL;
      
      XslProcessor *processor = ObjectWrap::Unwrap<XslProcessor>(args.Holder());
      
      Local<Array> errors = Array::New();
      //processor->errors = Persistent<Array>::New(errors);
      xmlResetLastError();
      xmlSetStructuredErrorFunc(reinterpret_cast<void *>(*errors), XslProcessor::PushToArray);

      xmlDoc *xmlres = xsltApplyStylesheet(processor->xsl_proc, processor->xml_doc, params);
      
      if(xmlres == NULL) {
        return scope.Close(Null());
      }
      else {
        xmlChar* buffer = NULL;
        int len = 0;
        xmlDocDumpFormatMemoryEnc(xmlres, &buffer, &len, "UTF-8", 0);
        Local<String> str = String::New((const char*)buffer, len);
        xmlFree(buffer);
        xmlFreeDoc(xmlres);
        return scope.Close(str);
      }
    }
  
    static Handle<Value> ParseString(const Arguments& args) {
      HandleScope scope;
      
      if (!(args[0]->IsString()))
        return ThrowException(Exception::Error(String::New("Must supply ParseXslString with a String")));
      
      XslProcessor *processor = ObjectWrap::Unwrap<XslProcessor>(args.Holder());
      if(processor->xsl_proc) xsltFreeStylesheet(processor->xsl_proc);
      
      Local<Array> errors = Array::New();
      //processor->errors = Persistent<Array>::New(errors);
      xmlResetLastError();
      xmlSetStructuredErrorFunc(reinterpret_cast<void *>(*errors), XslProcessor::PushToArray);

      String::Utf8Value strxsl(args[0]->ToString());
      xmlDocPtr xsl_doc = xmlReadMemory(*strxsl, strxsl.length(), NULL, "UTF-8", 0);
      if(xsl_doc == NULL) {
        return scope.Close(Null());
      }
      else {
        processor->xsl_proc = xsltParseStylesheetDoc(xsl_doc);
        return scope.Close(args.This());
      }
    }
    
    static Handle<Value> Free(const Arguments& args) {
      HandleScope scope;
      XslProcessor *processor = ObjectWrap::Unwrap<XslProcessor>(args.Holder());
      if(processor->xml_doc != NULL) xmlFreeDoc(processor->xml_doc);
      if(processor->xsl_proc != NULL) xsltFreeStylesheet(processor->xsl_proc);
      //processor->Unref();
      return scope.Close(Int32::New(0));
    }

    static Handle<Value> LastError(const Arguments& args) {
      HandleScope scope;
      XslProcessor *processor = ObjectWrap::Unwrap<XslProcessor>(args.Holder());
      return scope.Close(processor->errors);
    }

    static Handle<Value> New(const Arguments& args) {
      HandleScope scope;
      XslProcessor *processor = new XslProcessor();
      processor->Wrap(args.Holder());
      processor->Ref();
      return args.This();
    }
    
    XslProcessor() : ObjectWrap() {
      xsl_proc = NULL;
      xml_doc = NULL;
    }
    
    ~XslProcessor() {
      if(xml_doc != NULL) xmlFreeDoc(xml_doc);
      if(xsl_proc != NULL) xsltFreeStylesheet(xsl_proc);
    }
  
    xsltStylesheetPtr xsl_proc;
    xmlDocPtr xml_doc;
    Persistent<Array> errors;
  };

}  // namespace libxsltjs

NODE_MODULE(processor, libxsltjs::XslProcessor::Initialize)
/*
extern "C" void
init (Handle<Object> target) 
{
  HandleScope scope;
  libxsltjs::XslProcessor::Initialize(target);
}
*/
