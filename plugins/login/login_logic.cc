// Copyright (c) 2016 The login Authors. All rights reserved.
// login_logic.cc

#include "login/login_opcode.h"
#include "login/login_logic.h"

#include <assert.h>
#include <dlfcn.h>
#include <signal.h>

#include "core/common.h"
#include "core/network.h"
#include "glog/logging.h"
#include "base/logic/base_values.h"
#include "public/http/http_method.h"
#include "glog/logging.h"
#include "public/basic/basictypes.h"
#include "public/config/config.h"
#include "public/basic/native_library.h"
#include "base/logic/base_values.h"
#include "login/login_proto.h"

#define DEFAULT_CONFIG_PATH "./plugins/login/login_config.xml"

namespace login {
Loginlogic* Loginlogic::instance_ = NULL;

Loginlogic::Loginlogic() {
  login_mysql_ = NULL;

  if (!Init())
    assert(0);
}

void Loginlogic::InitLog() {
  //初始化日志名
  google::InitGoogleLogging("login");

  //初始化info级别日志存储位置以及日志文件开头 ./info/info_20160808-105851.4743
  google::SetLogDestination(google::INFO, "./log/info_");
  google::SetLogDestination(google::WARNING, "./log/waring_");
  google::SetLogDestination(google::ERROR, "./log/error_");

  //配置输出到标准输出的级别 INFO级别及以上输出到标准输出
  google::SetStderrLogging(google::INFO);

  //设置输出到屏幕的日志显示相应颜色
  FLAGS_colorlogtostderr = true;

  //实时输出日志
  FLAGS_logbufsecs = 0;

  //最大日志大小（MB）
  FLAGS_max_log_size = 100;

  //当磁盘被写满时，停止日志输出
  FLAGS_stop_logging_if_full_disk = true;

  //捕捉 core dumped
  google::InstallFailureSignalHandler();

  LOG(INFO)<< "glog has init finished";
}

Loginlogic::~Loginlogic() {
}
	
bool Loginlogic::Init() {
  bool r = false;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  std::string path = DEFAULT_CONFIG_PATH;
  if (config == NULL) {
    LOG(ERROR) << "Loginlogic config init error";
    return false;
  }
  r = config->LoadConfig(path);
  if (!r) {
    LOG(ERROR) << "login config load error";
    return false;
  }
  login_mysql_ = new LoginMysql(config);
  //InitShareData();
  return true;
}

	/*bool Loginlogic::InitShareData() {
  basic::libhandle  handle = NULL;
  handle = basic::load_native_library("./data.so");
  if (handle==NULL){
  	LOG(ERROR) << "Can't load path data.so\n";
  }
  LOG(INFO) << "load data.so success";
  share::DataShareMgr* (*pengine) (void);
  pengine = (share::DataShareMgr *(*)(void))basic::get_function_pointer(handle, "GetDataShareMgr");
  if(pengine==NULL){
  	LOG(ERROR) << "Can't find GetDataShareMgr\n";
  	return false;
  }
  share::DataShareMgr* data_engine_ = (*pengine)();
  UserInterface::GetInstance()->InitShareDataMgr(data_engine_);
  return false;
  }*/

Loginlogic* Loginlogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new Loginlogic();
  return instance_;
}

void Loginlogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

bool Loginlogic::OnLoginConnect(struct server *srv, const int socket) {
  LOG(INFO)<< "OnLoginConnect";
  return true;
}

bool Loginlogic::OnLoginMessage(struct server *srv, const int socket,
                                    const void *msg, const int len) {
  bool r = false;
  int32 err = 0;
  char* msg_c = new char[len + 1];
  memset(msg_c, 0, len+1);
  memcpy(msg_c, msg, len);
  LOG(INFO) << "OnLoginMessage:len-" << len;
  PacketHead packet(msg_c);
  delete[] msg_c;
  msg_c = NULL;
  if (packet.type() == LOGIN_TYPE) {
	switch (packet.operate_code()) {
      case REGISTER_ACCOUNT_REQ: {
        RegisterAccount(socket, &packet);
        break;
      }
      case USER_LOGIN_REQ: {
        UserLogin(socket, &packet);
        break;
      }
      case CHANGE_PASSWD_REQ: {
        ChangePasswd(socket, &packet);
        break;
      }
	}
    return true;
  }
  return false;
}
	
int32 Loginlogic::RegisterAccount(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    RegisterAccountRecv rev(*packet);
    err = rev.Deserialize();
    if (err < 0)
      break;
    /*if (time(NULL) - rev.timestamp() > 15 * 60) {
      err = VERIFY_CODE_OVERDUE;
      break;
    }
    std::stringstream ss;
    ss << SMS_KEY << rev.timestamp() << rev.verify_code() << rev.phone_num();
	base::MD5Sum md5(ss.str());
    if (md5.GetHash() != rev.token()) {
      err = VERIFY_CODE_ERR;
      break;
	  }*/
    DicValue dic;
    err = login_mysql_->RegisterInsertAndSelect(rev.phone_num(), rev.passwd(), &dic);
    if (err < 0)
      break;
    SendMsg(socket, packet, &dic, REGISTER_ACCOUNT_RLY);
  } while (0);
  if (err < 0) {
    SendError(socket, packet, err, REGISTER_ACCOUNT_RLY);
  }
  return err;
}

int32 Loginlogic::UserLogin(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  LoginRecv rev(*packet);
  do {
    err = rev.Deserialize();
    if (err < 0)
      break;
    if (UserIsLogin(rev.phone_num())) {
      // todo
    }
    DicValue dic;
	if (rev.passwd() != "")
	  err = login_mysql_->UserLoginSelect(rev.phone_num(), rev.passwd(), &dic);
	else if (rev.token() != "")
	  err = login_mysql_->UserTokenLoginSelect(rev.phone_num(), rev.token(), &dic);
    LOG(INFO)<< "UserLogin err:" << err;
    if (err < 0)
      break;
    SendMsg(socket, packet, &dic, USER_LOGIN_RLY);
    //AddUser(socket, &dic, rev.user_type(), rev.passwd());
  } while (0);
  if (err < 0) {
    SendError(socket, packet, err, USER_LOGIN_RLY);
  }
  LOG(INFO)<< "UserLogin finish err:" << err;
  return err;
}

bool Loginlogic::UserIsLogin(std::string u) {
  return false;
}

int32 Loginlogic::AuthorUser(std::string phone, std::string passwd, DicValue* v) {
  int32 err = 0;
  do {
    err = login_mysql_->UserLoginSelect(phone, passwd, v);
    if (err < 0)
      break;
  } while (0);
  return err;
}
	
int32 Loginlogic::ChangePasswd(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
  	ChangePasswdRecv rev(*packet);
  	err = rev.Deserialize();
  	if (err < 0)
  		break;
  	/*UserInfo* p = data_share_mgr_->GetUser(rev.uid());
    if (p == NULL || !p->is_login()) {
      err = USER_NOT_IN_CACHE;
      break;
      }
  LOG(INFO) << "pwd:" << p->passwd();
    if (p->passwd() != rev.old_passwd()) {
      err = CHANGE_OLD_PWD_ERR;
      break;
      } else */{
  		err = login_mysql_->ChangePasswdUpdate(rev.phone_num(), rev.passwd());
  		if (err < 0)
  			break;
  		//p->set_passwd(rev.new_passwd());
  		SendMsg(socket, packet, NULL, CHANGE_PASSWD_RLY);
  	}
  } while (0);
  if (err < 0) {
  	SendError(socket, packet, err, CHANGE_PASSWD_RLY);
  }
  return err;
}



bool Loginlogic::OnLoginClose(struct server *srv, const int socket) {
  return true;
}

bool Loginlogic::OnBroadcastConnect(struct server *srv, const int socket,
                                      const void *msg, const int len) {
  server_fd_ = socket;
  
  LOG(INFO)<< "OnBroadcastConnect";
  return true;
}

bool Loginlogic::OnBroadcastMessage(struct server *srv, const int socket,
                                      const void *msg, const int len) {
  return true;
}

bool Loginlogic::OnBroadcastClose(struct server *srv, const int socket) {
  server_fd_ = -1;
  
  pthread_t tid;
  if (pthread_create(&tid, 0, Loginlogic::AutoReconnectToServer, (void*)srv) != 0)
	LOG(ERROR) << "can not create thread AutoReconnectToserver";
  pthread_detach(tid);
  
  return true;
}

bool Loginlogic::OnInitTimer(struct server *srv) {
  return true;
}

bool Loginlogic::OnTimeout(struct server *srv, char *id, int opcode,
                             int time) {
  return true;
}

void* Loginlogic::AutoReconnectToServer(void* arg) {
  struct server* srv = (struct server*)arg;
  int ret = 0;
  
  do {
	ret = srv->create_reconnects(srv);
	sleep(1);
  } while (ret < 0);
  LOG(INFO) << "try reconnect remote server:" << ret;
}

int Loginlogic::SendFull(int socket, const char *buffer, size_t nbytes) {
  ssize_t amt = 0;
  ssize_t total = 0;
  const char *buf = buffer;

  do {
    amt = nbytes;
    amt = send(socket, buf, amt, 0);
    buf = buf + amt;
    nbytes -= amt;
    total += amt;
  } while (amt != -1 && nbytes > 0);

  return (int) (amt == -1 ? amt : total);
}

void Loginlogic::SendPacket(const int socket, PacketHead* packet) {

  char* s = new char[packet->packet_length()];
  LOG(INFO)<< "packet body:" << packet->body_str();
  memset(s, 0, packet->packet_length());
  memcpy(s, &packet->head(), HEAD_LENGTH);
  memcpy(s + HEAD_LENGTH, packet->body_str().c_str(),
         packet->body_str().length());
  int total = SendFull(socket, s, packet->packet_length());
  delete[] s;
  s = NULL;
  LOG_IF(ERROR, total != packet->packet_length()) << "send packet wrong";
}

void Loginlogic::SendError(const int socket, PacketHead* packet, int32 err,
                             int16 opcode) {
  PacketErr p_err;
  p_err.set_head(packet->head());
  p_err.set_type(3);
  p_err.set_error(err);
  p_err.Serialize();
  p_err.AdapterLen();
  p_err.set_operate_code(opcode);
  SendPacket(socket, &p_err);
}

void Loginlogic::SendMsg(const int socket, PacketHead* packet, DicValue* dic,
                           int16 opcode) {
  PacketHead send;
  send.set_head(packet->head());
  send.Serialize(dic);
  send.AdapterLen();
  send.set_operate_code(opcode);
  SendPacket(socket, &send);
}

void Loginlogic::SendMsg(const int socket, PacketHead* packet, int16 opcode) {
  PacketHead send;
  send.set_head(packet->head());
  send.set_operate_code(opcode);
  SendPacket(socket, &send);
}
	
}  // namespace login

