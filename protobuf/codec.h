#ifndef PROTOBUF_TEST_CODEC_H_
#define PROTOBUF_TEST_CODEC_H_

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <zlib.h>
#include <string>

#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>

const int kHeaderLen = sizeof(int32_t);
void print_debug(const std::string& buf) {
  printf("encoded to %zd bytes\n", buf.size());
  for (size_t i = 0; i < buf.size(); ++i) {
    printf("%2zd:  0x%02x  %c\n", i, (unsigned char)buf[i], isgraph(buf[i]) ? buf[i]: ' ');
  }
}

inline std::string encode(const google::protobuf::Message& message) {
  std::string result;
  result.resize(kHeaderLen);

  const std::string& typeName = message.GetTypeName();
  int32_t nameLen = static_cast<int32_t>(typeName.size() + 1);
  int32_t be32 = ::htonl(nameLen);
  result.append(reinterpret_cast<char*>(&be32), sizeof(be32));
  result.append(typeName.c_str(), nameLen);
  bool succeed = message.AppendToString(&result);
  if (succeed) {
    const char* begin = result.c_str() + kHeaderLen;
    int checkSum = adler32(1, reinterpret_cast<const Bytef*>(begin), result.size() - kHeaderLen);
    int32_t check_be32 = ::htonl(checkSum);
    printf("encode len=%d, n1=%d,n2=%d\n", result.size() - kHeaderLen, checkSum, check_be32);
    //print_debug(std::string(begin, result.size() - kHeaderLen));
    //print_debug(result);
    result.append(reinterpret_cast<char*>(&check_be32), sizeof(check_be32));
    //print_debug(result);

    int32_t len = ::htonl(result.size() - kHeaderLen);
    std::copy(reinterpret_cast<char*>(&len),
              reinterpret_cast<char*>(&len) + sizeof(len),
              result.begin());
    //print_debug(result);
  } else {
    result.clear();
  }
  return result;
}

inline google::protobuf::Message* createMessage(const std::string& type_name) {
  google::protobuf::Message* message = NULL;
  const google::protobuf::Descriptor* descriptor =
    google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
  if (descriptor) {
    const google::protobuf::Message* prototype =
      google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (prototype) {
      message = prototype->New();
    }
  }
  return message;
}

inline int32_t asInt32(const char* buf) {
  int32_t be32 = 0;
  ::memcpy(&be32, buf, sizeof(be32));
  return ::ntohl(be32);
}

inline google::protobuf::Message* decode(const std::string& buf) {
  //print_debug(buf);
  google::protobuf::Message* result = NULL;

  int32_t len = static_cast<int32_t>(buf.size());
  if (len < 5) return result;
  int32_t expectedCheckSum = asInt32(buf.c_str() + buf.size() - kHeaderLen);
  const char* begin = buf.c_str();
  int32_t checkSum = adler32(1, reinterpret_cast<const Bytef*>(begin), len - kHeaderLen);
  printf("decode len=%d, n1=%d,n2=%d\n", len - kHeaderLen, checkSum, expectedCheckSum);
  //printf("decode str=%s\n", begin);
  //print_debug(std::string(begin, len - kHeaderLen));
  if (checkSum != expectedCheckSum) return result;

  int nameLen = asInt32(buf.c_str());
  if (nameLen <2 || nameLen > len - 2*kHeaderLen) return result;

  std::string typeName(buf.begin() + kHeaderLen, buf.begin() + kHeaderLen+ nameLen - 1);
  google::protobuf::Message* message = createMessage(typeName);
  if (!message) return result;

  const char* data = buf.c_str() + nameLen + kHeaderLen;
  int32_t dataLen = len - nameLen - 2*kHeaderLen;
  if (message->ParseFromArray(data, dataLen)) {
    result = message;
  } else {
    delete message;
  }
  return result;
}

#endif // PROTOBUF_TEST_CODEC_H_
