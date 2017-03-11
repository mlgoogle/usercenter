// Copyright (c) 2016 The tourism Authors. All rights reserved.
// login_proto.cc

#include "login/login_proto.h"

#include <typeinfo>

#include "glog/logging.h"
#include "base/logic/base_values.h"
#include "public/basic/md5sum.h"

#include "pub/comm/comm_head.h"
//#include "pub/util/util.h"

namespace login {
RegisterAccountRecv::RegisterAccountRecv(PacketHead packet) {
  head_ = packet.head();
  body_str_ = packet.body_str();
  timestamp_ = 0;
  verify_code_ = 0;
  user_type_ = 0;
}

int32 RegisterAccountRecv::Deserialize() {
  int32 err = 0;
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  DicValue* dic = (DicValue*) serializer->Deserialize(&err, &err_str);
  do {
    if (dic != NULL) {
      r = dic->GetBigInteger(L"timestamp_", &timestamp_);
      LOG_IF(ERROR, !r) << "RegisterAccountRecv::timestamp_ parse error";
      r = dic->GetBigInteger(L"verify_code_", &verify_code_);
      LOG_IF(ERROR, !r) << "RegisterAccountRecv::verify_code_ parse error";
      r = dic->GetBigInteger(L"user_type_", &user_type_);
      LOG_IF(ERROR, !r) << "RegisterAccountRecv::user_type_ parse error";
      r = dic->GetString(L"phone", &phone_num_);
      LOG_IF(ERROR, !r) << "RegisterAccountRecv::phone parse error";
      r = dic->GetString(L"pwd", &passwd_);
      LOG_IF(ERROR, !r) << "RegisterAccountRecv::pwd parse error";
      r = dic->GetString(L"token_", &token_);
      LOG_IF(ERROR, !r) << "RegisterAccount::token_ parse error";
    } else {
      LOG(ERROR)<< "RegisterAccountRecv Deserialize error";
      err = JSON_FORMAT_ERR;
      break;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
}

LoginRecv::LoginRecv(PacketHead packet) {
  head_ = packet.head();
  body_str_ = packet.body_str();
}

int32 LoginRecv::Deserialize() {
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  int32 err = 0;
  Value* v = serializer->Deserialize(&err, &err_str);
  DicValue* dic;
  do {
    if (v != NULL) {
      if (typeid(*v) == typeid(ListValue)) {
        LOG(INFO)<< "listvalue:";
        ((ListValue*)v)->GetDictionary(0, &dic);
      } else if (typeid(*v) == typeid(DicValue)) {
        LOG(INFO) << "dicvalue:";
        dic = (DicValue*) v;
      } else {
        err = JSON_FORMAT_ERR;
        break;
      }
      r = dic->GetString(L"pwd", &passwd_);
      LOG_IF(ERROR, !r) << "LoginRecv::passwd parse error";
      r = dic->GetString(L"phone", &phone_num_);
      LOG_IF(ERROR, !r) << "LoginRecv::phone_num_ parse error";
      if (phone_num_.length() < 11) {
        LOG(ERROR) << "phone is wrong";
        err = PHONE_NUM_ERR;
        break;
      }
      r = dic->GetString(L"token", &token_);
      LOG_IF(ERROR, !r) << "LoginRecv::token_ parse error";
    } else {
      LOG(ERROR) << "LoginRecv Deserialize error";
      err = JSON_FORMAT_ERR;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
}

	/*SMSCodeLoginRecv::SMSCodeLoginRecv(PacketHead packet) {
  head_ = packet.head();
  body_str_ = packet.body_str();
  timestamp_ = 0;
  verify_code_ = 0;
  user_type_ = 0;
}

int32 SMSCodeLoginRecv::Deserialize() {
  int32 err = 0;
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  DicValue* dic = (DicValue*) serializer->Deserialize(&err, &err_str);
  do {
    if (dic != NULL) {
      r = dic->GetBigInteger(L"timestamp_", &timestamp_);
      LOG_IF(ERROR, !r) << "SMSCodeLoginRecv::timestamp_ parse error";
      r = dic->GetBigInteger(L"verify_code_", &verify_code_);
      LOG_IF(ERROR, !r) << "SMSCodeLoginRecv::verify_code_ parse error";
      r = dic->GetBigInteger(L"user_type_", &user_type_);
      LOG_IF(ERROR, !r) << "SMSCodeLoginRecv::user_type_ parse error";
      r = dic->GetString(L"phone_num_", &phone_num_);
      LOG_IF(ERROR, !r) << "SMSCodeLoginRecv::phone_num_ parse error";
      r = dic->GetString(L"token_", &token_);
      LOG_IF(ERROR, !r) << "SMSCodeLoginRecv::token_ parse error";
    } else {
      LOG(ERROR)<< "SMSCodeLoginRecv Deserialize error";
      err = SMS_CODE_LOGIN_JSON_ERR;
      break;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
  }*/


Heartbeat::Heartbeat(PacketHead packet) {
  head_ = packet.head();
  body_str_ = packet.body_str();
  uid_ = -1;
}

int32 Heartbeat::Deserialize() {
  int32 err = 0;
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  DicValue* dic = (DicValue*) serializer->Deserialize(&err, &err_str);
  do {
    if (dic != NULL) {
      r = dic->GetBigInteger(L"uid_", &uid_);
      LOG_IF(ERROR, !r) << "Heartbeat::uid_ parse error";
    } else {
      LOG(ERROR)<< "Heartbeat Deserialize error";
      err = JSON_FORMAT_ERR;
      break;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
}

ChangePasswdRecv::ChangePasswdRecv(PacketHead packet) {
  head_ = packet.head();
  body_str_ = packet.body_str();
}

int32 ChangePasswdRecv::Deserialize() {
  int32 err = 0;
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  DicValue* dic = (DicValue*) serializer->Deserialize(&err, &err_str);
  do {
    if (dic != NULL) {
      r = dic->GetString(L"phone", &phone_num_);
      LOG_IF(ERROR, !r) << "ChangePasswdRecv::phone parse error";
      r = dic->GetString(L"pwd", &passwd_);
      LOG_IF(ERROR, !r) << "ChangePasswdRecv::pwd parse error";
    } else {
      LOG(ERROR)<< "ChangePasswdRecv Deserialize error";
      err = JSON_FORMAT_ERR;
      break;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
}

ObtainVerifyCodeRecv::ObtainVerifyCodeRecv(PacketHead packet) {
  head_ = packet.head();
  body_str_ = packet.body_str();
  verify_type_ = 0;
}

int32 ObtainVerifyCodeRecv::Deserialize() {
  int32 err = 0;
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  DicValue* dic = (DicValue*) serializer->Deserialize(&err, &err_str);
  do {
    if (dic != NULL) {
      r = dic->GetBigInteger(L"verify_type_", &verify_type_);
      LOG_IF(ERROR, !r) << "ObtainVerifyCodeRecv::verify_type_ parse error";
      r = dic->GetString(L"phone_num_", &phone_num_);
      LOG_IF(ERROR, !r) << "ObtainVerifyCodeRecv::phone_num_ parse error";
    } else {
      LOG(ERROR)<< "ObtainVerifyCodeRecv Deserialize error";
      err = JSON_FORMAT_ERR;
      break;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
}

ImproveDataRecv::ImproveDataRecv(PacketHead packet) {
  head_ = packet.head();
  body_str_ = packet.body_str();
  uid_ = 0;
  longitude_ = 0;
  latitude_ = 0;
  gender_ = 0;
}

int32 ImproveDataRecv::Deserialize() {
  int32 err = 0;
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  DicValue* dic = (DicValue*) serializer->Deserialize(&err, &err_str);
  do {
    if (dic != NULL) {
      r = dic->GetBigInteger(L"uid_", &uid_);
      LOG_IF(ERROR, !r) << "ImproveDataRecv::uid_ parse error";
      r = dic->GetBigInteger(L"gender_", &gender_);
      LOG_IF(ERROR, !r) << "ImproveDataRecv::gender_ parse error";
      r = dic->GetString(L"nickname_", &nickname_);
      LOG_IF(ERROR, !r) << "ImproveDataRecv::nickname_ parse error";
      r = dic->GetString(L"head_url_", &head_url_);
      LOG_IF(ERROR, !r) << "ImproveDataRecv::head_url_ parse error";
      r = dic->GetString(L"address_", &address_);
      LOG_IF(ERROR, !r) << "ImproveDataRecv::address_ parse error";
      r = dic->GetReal(L"longitude_", &longitude_);
      LOG_IF(ERROR, !r) << "ImproveDataRecv::longitude_ parse error";
      r = dic->GetReal(L"latitude_", &latitude_);
      LOG_IF(ERROR, !r) << "ImproveDataRecv::latitude_ parse error";
    } else {
      LOG(ERROR)<< "ImproveDataRecv Deserialize error";
      err = JSON_FORMAT_ERR;
      break;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
}

UserDetailRecv::UserDetailRecv(PacketHead packet) {
  head_ = packet.head();
  body_str_ = packet.body_str();
}

int32 UserDetailRecv::Deserialize() {
  int32 err = 0;
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  DicValue* dic = (DicValue*) serializer->Deserialize(&err, &err_str);
  do {
    if (dic != NULL) {
      r = dic->GetString(L"uid_str_", &uid_str_);
      LOG_IF(ERROR, !r) << "UserDetailRecv::uid_str_ parse error";

    } else {
      LOG(ERROR)<< "UserDetailRecv Deserialize error";
      err = JSON_FORMAT_ERR;
      break;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
}


	/*int32 VerifyPasswdRecv::Deserialize() {
  int32 err = 0;
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  DicValue* dic = (DicValue*) serializer->Deserialize(&err, &err_str);
  do {
    if (dic != NULL) {
      r = dic->GetBigInteger(L"uid_", &uid_);
      LOG_IF(ERROR, !r) << "VerifyPasswdRecv::uid_ parse error";
      r = dic->GetBigInteger(L"passwd_type_", &passwd_type_);
      LOG_IF(ERROR, !r) << "VerifyPasswdRecv::passwd_type_ parse error";
      r = dic->GetString(L"passwd_", &passwd_);
      if (r) {
        base::MD5Sum md5(passwd_);
        passwd_ = md5.GetHash();
      }
      LOG_IF(ERROR, !r) << "VerifyPasswdRecv::passwd_ parse error";
    } else {
      LOG(ERROR)<< "VerifyPasswdRecv Deserialize error";
      err = REQUEST_JSON_ERR;
      break;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
  }*/

	/*ChangePayPasswdRecv::ChangePayPasswdRecv(PacketHead packet) {
  head_ = packet.head();
  body_str_ = packet.body_str();
  uid_ = 0;
  change_type_ = 0;
  passwd_type_ = 0;
}

int32 ChangePayPasswdRecv::Deserialize() {
  int32 err = 0;
  bool r = false;
  base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &body_str_, false);
  std::string err_str;
  DicValue* dic = (DicValue*) serializer->Deserialize(&err, &err_str);
  do {
    if (dic != NULL) {
      r = dic->GetBigInteger(L"uid_", &uid_);
      LOG_IF(ERROR, !r) << "ChangePayPasswdRecv::uid_ parse error";
      r = dic->GetBigInteger(L"change_type_", &change_type_);
      LOG_IF(ERROR, !r) << "ChangePayPasswdRecv::change_type_ parse error";
      r = dic->GetBigInteger(L"passwd_type_", &passwd_type_);
      LOG_IF(ERROR, !r) << "ChangePayPasswdRecv::passwd_type_ parse error";
      r = dic->GetString(L"new_passwd_", &new_passwd_);
      if (r) {
        base::MD5Sum md5(new_passwd_);
        new_passwd_ = md5.GetHash();
      }
      LOG_IF(ERROR, !r) << "ChangePayPasswdRecv::new_passwd_ parse error";
      r = dic->GetString(L"old_passwd_", &old_passwd_);
      if (r) {
        base::MD5Sum md5(old_passwd_);
        old_passwd_ = md5.GetHash();
      }
      LOG_IF(ERROR, !r) << "ChangePayPasswdRecv::old_passwd_ parse error";
    } else {
      LOG(ERROR)<< "ChangePayPasswdRecv Deserialize error";
      err = REQUEST_JSON_ERR;
      break;
    }
  }while (0);
  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON,
                                                serializer);
  return err;
  }*/

}  // namespace login

