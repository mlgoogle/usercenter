// Copyright (c) 2016 The login Authors. All rights reserved.
// login_logic.h

#ifndef PLUGINS_LOGIN_LOGIN_LOGIC_H_
#define PLUGINS_LOGIN_LOGIN_LOGIC_H_

#include <list> 
#include <pthread.h>

#include "core/common.h"
#include "login/typedef.h"
#include "login/proto_buf.h"
#include "login/login_mysql.h"

namespace login {

class Loginlogic {
 public:
  Loginlogic();
  virtual ~Loginlogic();

 private:
  static Loginlogic  *instance_;

 public:
  static Loginlogic *GetInstance();
  static void FreeInstance();

 public:
  bool OnLoginConnect(struct server *srv, const int socket);
  bool OnLoginMessage(struct server *srv, const int socket,
                        const void *msg, const int len);
  bool OnLoginClose(struct server *srv, const int socket);
  bool OnBroadcastConnect(struct server *srv, const int socket,
                          const void *data, const int len);
  bool OnBroadcastMessage(struct server *srv, const int socket,
                          const void *msg, const int len);
  bool OnBroadcastClose(struct server *srv, const int socket);
  bool OnInitTimer(struct server *srv);
  bool OnTimeout(struct server *srv, char* id, int opcode, int time);
  
  static void* AutoReconnectToServer(void* arg);

 private:
  bool Init();
  bool InitShareData();
  void InitLog();
  
  int32 RegisterAccount(const int32 socket, PacketHead* packet);
  int32 UserLogin(const int32 socket, PacketHead* packet);
  bool UserIsLogin(std::string u);
  int32 AuthorUser(std::string phone, std::string passwd, DicValue* v);
  int32 ChangePasswd(const int32 socket, PacketHead* packet);
  
  int SendFull(int socket, const char *buffer, size_t nbytes);
  void SendMsg(const int socket, PacketHead* packet, DicValue* dic,
               int16 opcode);
  void SendMsg(const int socket, PacketHead* packet, int16 opcode);
  void SendError(const int socket, PacketHead* packet, int32 err, int16 opcode);
  void SendPacket(const int socket, PacketHead* packet);
  
  LoginMysql* login_mysql_;
  int32 server_fd_;
};

}  // namespace login

#endif  // PLUGINS_LOGIN_LOGIN_LOGIC_H_

