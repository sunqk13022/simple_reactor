#include "codec.h"
#include "query.pb.h"

#include <stdio.h>
#include <iostream>

void print(const std::string& buf) {
  printf("encoded to %zd bytes\n", buf.size());
  for (size_t i = 0; i < buf.size(); ++i) {
    printf("%2zd:  0x%02x  %c\n", i, (unsigned char)buf[i], isgraph(buf[i]) ? buf[i]: ' ');
  }
}

void testQuery() {
  simple_reactor::Query query;
  query.set_id(1);
  query.set_questioner("MyBigBBB");
  query.add_question("Running?");

  std::string transport = encode(query);
  //print(transport);

  int32_t be32 = 0;
  std::copy(transport.begin(), transport.begin() + sizeof(be32), reinterpret_cast<char*>(&be32));
  int32_t len = ::ntohl(be32);
  assert(len == transport.size() - sizeof(be32));

  std::string buf = transport.substr(sizeof(int32_t));
  assert(len = buf.size());

  simple_reactor::Query* newQuery = dynamic_cast<simple_reactor::Query*>(decode(buf));
  assert(newQuery != NULL);
  newQuery->PrintDebugString();
  assert(newQuery->DebugString() == query.DebugString());
  delete newQuery;

  buf[buf.size() - 6]++;
  simple_reactor::Query* badQuery = dynamic_cast<simple_reactor::Query*>(decode(buf));
  assert(badQuery == NULL);
}

void testEmpty() {
  simple_reactor::Empty empty;
  std::string transport = encode(empty);

  print(transport);

  std::string buf = transport.substr(sizeof(int32_t));

  simple_reactor::Empty* newEmpty = dynamic_cast<simple_reactor::Empty*>(decode(buf));
  assert(newEmpty != NULL);
  newEmpty->PrintDebugString();
  assert(newEmpty->DebugString() == empty.DebugString());
  delete newEmpty;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  testQuery();
  std::cout << "\n\n";

  testEmpty();
  std::cout << "\n\n";

  google::protobuf::ShutdownProtobufLibrary();
}
