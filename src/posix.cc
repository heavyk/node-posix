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

#include <errno.h>

using namespace v8;
using namespace node;

NAN_METHOD(node_chroot) {
  if (args.Length() != 1) {
    return NanThrowError("chroot: takes exactly one argument");
  }

  if (!args[0]->IsString()) {
    return NanThrowError("chroot: first argument must be a string");
  }

  String::Utf8Value dir_path(args[0]->ToString());

  // proper order is to first chdir() and then chroot()
  if(chdir(*dir_path)) {
    return NanThrowError(node::ErrnoException(errno));
  }

  if(chroot(*dir_path)) {
    return NanThrowError(node::ErrnoException(errno));
  }

  NanReturnUndefined();
}

#define ADD_MASK_FLAG(name, flag) \
  obj->Set(NanNew<String>(name), NanNew<Integer>(flag)); \
  obj->Set(NanNew<String>("mask_" name), NanNew<Integer>(LOG_MASK(flag)));

NAN_METHOD(node_update_syslog_constants) {
  NanScope();

  if((args.Length() != 1) || (!args[0]->IsObject())) {
    return NanThrowError("invalid arguments");
  }

  Local<Object> obj = args[0]->ToObject();
  ADD_MASK_FLAG("emerg", LOG_EMERG);
  ADD_MASK_FLAG("alert", LOG_ALERT);
  ADD_MASK_FLAG("crit", LOG_CRIT);
  ADD_MASK_FLAG("err", LOG_ERR);
  ADD_MASK_FLAG("warning", LOG_WARNING);
  ADD_MASK_FLAG("notice", LOG_NOTICE);
  ADD_MASK_FLAG("info", LOG_INFO);
  ADD_MASK_FLAG("debug", LOG_DEBUG);

  // facility constants
  obj->Set(NanNew<String>("auth"), NanNew<Integer>(LOG_AUTH));
#ifdef LOG_AUTHPRIV
  obj->Set(NanNew<String>("authpriv"), NanNew<Integer>(LOG_AUTHPRIV));
#endif
  obj->Set(NanNew<String>("cron"), NanNew<Integer>(LOG_CRON));
  obj->Set(NanNew<String>("daemon"), NanNew<Integer>(LOG_DAEMON));
#ifdef LOG_FTP
    obj->Set(NanNew<String>("ftp"), NanNew<Integer>(LOG_FTP));
#endif
  obj->Set(NanNew<String>("kern"), NanNew<Integer>(LOG_KERN));
  obj->Set(NanNew<String>("lpr"), NanNew<Integer>(LOG_LPR));
  obj->Set(NanNew<String>("mail"), NanNew<Integer>(LOG_MAIL));
  obj->Set(NanNew<String>("news"), NanNew<Integer>(LOG_NEWS));
  obj->Set(NanNew<String>("syslog"), NanNew<Integer>(LOG_SYSLOG));
  obj->Set(NanNew<String>("user"), NanNew<Integer>(LOG_USER));
  obj->Set(NanNew<String>("uucp"), NanNew<Integer>(LOG_UUCP));
  obj->Set(NanNew<String>("local0"), NanNew<Integer>(LOG_LOCAL0));
  obj->Set(NanNew<String>("local1"), NanNew<Integer>(LOG_LOCAL1));
  obj->Set(NanNew<String>("local2"), NanNew<Integer>(LOG_LOCAL2));
  obj->Set(NanNew<String>("local3"), NanNew<Integer>(LOG_LOCAL3));
  obj->Set(NanNew<String>("local4"), NanNew<Integer>(LOG_LOCAL4));
  obj->Set(NanNew<String>("local5"), NanNew<Integer>(LOG_LOCAL5));
  obj->Set(NanNew<String>("local6"), NanNew<Integer>(LOG_LOCAL6));
  obj->Set(NanNew<String>("local7"), NanNew<Integer>(LOG_LOCAL7));

  // option constants
  obj->Set(NanNew<String>("pid"), NanNew<Integer>(LOG_PID));
  obj->Set(NanNew<String>("cons"), NanNew<Integer>(LOG_CONS));
  obj->Set(NanNew<String>("ndelay"), NanNew<Integer>(LOG_NDELAY));
  obj->Set(NanNew<String>("odelay"), NanNew<Integer>(LOG_ODELAY));
  obj->Set(NanNew<String>("nowait"), NanNew<Integer>(LOG_NOWAIT));

  NanReturnUndefined();
}


void init(Handle<Object> exports) {
  exports->Set(NanNew<String>("chroot"),
    NanNew<FunctionTemplate>(node_chroot)->GetFunction());
  exports->Set(NanNew<String>("update_syslog_constants"),
    NanNew<FunctionTemplate>(node_update_syslog_constants)->GetFunction());
}

NODE_MODULE(posix, init)
