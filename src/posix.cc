#include <node.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h> // setrlimit, getrlimit
#include <limits.h> // PATH_MAX
#include <pwd.h> // getpwnam, passwd
#include <grp.h> // getgrnam, group
#include <syslog.h> // openlog, closelog, syslog, setlogmask
#include <nan.h>

using namespace v8;
using namespace node;

NAN_METHOD(node_chroot) {
  if (info.Length() != 1) {
    return Nan::ThrowError("chroot: takes exactly one argument");
  }

  if (!info[0]->IsString()) {
    return Nan::ThrowError("chroot: first argument must be a string");
  }

  String::Utf8Value dir_path(info[0]->ToString());

  // proper order is to first chdir() and then chroot()
  if(chdir(*dir_path)) {
    return Nan::ThrowError(node::ErrnoException(errno));
  }

  if(chroot(*dir_path)) {
    return Nan::ThrowError(node::ErrnoException(errno));
  }

  return;
}

#define ADD_MASK_FLAG(name, flag) \
  obj->Set(Nan::New<String>(name).ToLocalChecked(), Nan::New<Integer>(flag)); \
  obj->Set(Nan::New<String>("mask_" name).ToLocalChecked(), Nan::New<Integer>(LOG_MASK(flag)));

NAN_METHOD(node_update_syslog_constants) {
  Nan::HandleScope scope;

  if((info.Length() != 1) || (!info[0]->IsObject())) {
    return Nan::ThrowError("invalid arguments");
  }

  Local<Object> obj = info[0]->ToObject();
  ADD_MASK_FLAG("emerg", LOG_EMERG);
  ADD_MASK_FLAG("alert", LOG_ALERT);
  ADD_MASK_FLAG("crit", LOG_CRIT);
  ADD_MASK_FLAG("err", LOG_ERR);
  ADD_MASK_FLAG("warning", LOG_WARNING);
  ADD_MASK_FLAG("notice", LOG_NOTICE);
  ADD_MASK_FLAG("info", LOG_INFO);
  ADD_MASK_FLAG("debug", LOG_DEBUG);

  // facility constants
  obj->Set(Nan::New<String>("auth").ToLocalChecked(), Nan::New<Integer>(LOG_AUTH));
#ifdef LOG_AUTHPRIV
  obj->Set(Nan::New<String>("authpriv").ToLocalChecked(), Nan::New<Integer>(LOG_AUTHPRIV));
#endif
  obj->Set(Nan::New<String>("cron").ToLocalChecked(), Nan::New<Integer>(LOG_CRON));
  obj->Set(Nan::New<String>("daemon").ToLocalChecked(), Nan::New<Integer>(LOG_DAEMON));
#ifdef LOG_FTP
  obj->Set(Nan::New<String>("ftp").ToLocalChecked(), Nan::New<Integer>(LOG_FTP));
#endif
  obj->Set(Nan::New<String>("kern").ToLocalChecked(), Nan::New<Integer>(LOG_KERN));
  obj->Set(Nan::New<String>("lpr").ToLocalChecked(), Nan::New<Integer>(LOG_LPR));
  obj->Set(Nan::New<String>("mail").ToLocalChecked(), Nan::New<Integer>(LOG_MAIL));
  obj->Set(Nan::New<String>("news").ToLocalChecked(), Nan::New<Integer>(LOG_NEWS));
  obj->Set(Nan::New<String>("syslog").ToLocalChecked(), Nan::New<Integer>(LOG_SYSLOG));
  obj->Set(Nan::New<String>("user").ToLocalChecked(), Nan::New<Integer>(LOG_USER));
  obj->Set(Nan::New<String>("uucp").ToLocalChecked(), Nan::New<Integer>(LOG_UUCP));
  obj->Set(Nan::New<String>("local0").ToLocalChecked(), Nan::New<Integer>(LOG_LOCAL0));
  obj->Set(Nan::New<String>("local1").ToLocalChecked(), Nan::New<Integer>(LOG_LOCAL1));
  obj->Set(Nan::New<String>("local2").ToLocalChecked(), Nan::New<Integer>(LOG_LOCAL2));
  obj->Set(Nan::New<String>("local3").ToLocalChecked(), Nan::New<Integer>(LOG_LOCAL3));
  obj->Set(Nan::New<String>("local4").ToLocalChecked(), Nan::New<Integer>(LOG_LOCAL4));
  obj->Set(Nan::New<String>("local5").ToLocalChecked(), Nan::New<Integer>(LOG_LOCAL5));
  obj->Set(Nan::New<String>("local6").ToLocalChecked(), Nan::New<Integer>(LOG_LOCAL6));
  obj->Set(Nan::New<String>("local7").ToLocalChecked(), Nan::New<Integer>(LOG_LOCAL7));

  // option constants
  obj->Set(Nan::New<String>("pid").ToLocalChecked(), Nan::New<Integer>(LOG_PID));
  obj->Set(Nan::New<String>("cons").ToLocalChecked(), Nan::New<Integer>(LOG_CONS));
  obj->Set(Nan::New<String>("ndelay").ToLocalChecked(), Nan::New<Integer>(LOG_NDELAY));
  obj->Set(Nan::New<String>("odelay").ToLocalChecked(), Nan::New<Integer>(LOG_ODELAY));
  obj->Set(Nan::New<String>("nowait").ToLocalChecked(), Nan::New<Integer>(LOG_NOWAIT));

  return;
}


void init(Local<Object> exports) {
  exports->Set(Nan::New<String>("chroot").ToLocalChecked(),
    Nan::New<FunctionTemplate>(node_chroot)->GetFunction());
  exports->Set(Nan::New<String>("update_syslog_constants").ToLocalChecked(),
    Nan::New<FunctionTemplate>(node_update_syslog_constants)->GetFunction());
}

NODE_MODULE(posix, init)
