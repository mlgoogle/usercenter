// Copyright (c) 2016 The tourism Authors. All rights reserved.
// typedef.h
// Created on: 2016年8月10日.
// Author: Paco.
#ifndef PUB_NET_TYPEDEF_H_
#define PUB_NET_TYPEDEF_H_

#include <map>

#include "base/logic/base_values.h"



typedef base_logic::DictionaryValue DicValue;
typedef base_logic::ListValue ListValue;
typedef base_logic::Value Value;

#define USER_TYPE 1

#define ALIPAY_SEVER_REQ USER_TYPE*1000 + 15 //支付宝服务端支付通知
#define ALIPAY_SEVER_RLY USER_TYPE*1000 + 16

#define WXPAY_SERVER_REQ USER_TYPE*1000 + 53 //微信服务端支付通知
#define WXPAY_SERVER_RLY USER_TYPE*1000 + 54 //微信服务端支付成功




#endif  // PUB_NET_TYPEDEF_H_
