// Copyright (c) 2016 The tourism Authors. All rights reserved.
// login_proto.h
// Created on: 2016年8月12日.
// Author: Paco.
#ifndef PLUGINS_LOGIN_LOGIN_PROTO_H_
#define PLUGINS_LOGIN_LOGIN_PROTO_H_

#include "pub/net/proto_buf.h"
namespace login {

class RegisterAccountRecv : public PacketHead {
 public:
  RegisterAccountRecv(PacketHead packet);
  int32 Deserialize();
  inline int64 timestamp() { return timestamp_; }
  inline int64 verify_code() { return verify_code_; }
  inline std::string phone_num() { return phone_num_; }
  inline std::string passwd() { return passwd_; }
  inline std::string token() { return token_; }
  inline int64 user_type() { return user_type_; }
 private:
  int64 timestamp_;
  int64 verify_code_;
  int64 user_type_;
  std::string phone_num_;
  std::string passwd_;
  std::string token_;
};
	
//recv
class LoginRecv:public PacketHead {
 public:
  LoginRecv(PacketHead packet);
  int32 Deserialize();

  inline std::string phone_num() { return phone_num_; }
  inline std::string passwd() { return passwd_; }
  inline std::string token() { return token_; }
 private:
  std::string phone_num_;
  std::string passwd_;
  std::string token_; 
};

class Heartbeat:public PacketHead {
 public:
  Heartbeat(PacketHead packet);
  int32 Deserialize();

  inline int64 uid() { return uid_; }
 private:
  int64 uid_;
};

class ChangePasswdRecv:public PacketHead {
 public:
  ChangePasswdRecv(PacketHead packet);
  int32 Deserialize();

  inline std::string phone_num() { return phone_num_; };
  inline std::string passwd() { return passwd_; }
 private:
  std::string phone_num_;
  std::string passwd_;
};

class ObtainVerifyCodeRecv : public PacketHead {
 public:
  ObtainVerifyCodeRecv(PacketHead packet);
  int32 Deserialize();
  inline int64 verify_type() { return verify_type_; }
  inline std::string phone_num() { return phone_num_; }
 private:
  int64 verify_type_;
  std::string phone_num_;
};


class ImproveDataRecv : public PacketHead {
 public:
  ImproveDataRecv(PacketHead packet);
  int32 Deserialize();

  inline int64 uid() { return uid_; }
  inline int64 gender() { return gender_; }
  inline std::string nickname() { return nickname_; }
  inline std::string head_url() { return head_url_; }
  inline std::string address() { return address_; }
  inline double longitude() { return longitude_; }
  inline double latitude() { return latitude_; }

 private:
  int64 uid_;
  int64 gender_;
  std::string nickname_;
  std::string head_url_;
  std::string address_;
  double longitude_;
  double latitude_;
};

class UserDetailRecv : public PacketHead {
public:
 UserDetailRecv(PacketHead packet);
 int32 Deserialize();

 inline std::string uid_str() { return uid_str_; }
private:
 std::string uid_str_; // "1,2,3,4"
};
 
}  // namespace login

#endif  // PLUGINS_LOGIN_LOGIN_PROTO_H_
