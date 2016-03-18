#include "codec.h"
#include "query.pb.h"

#include <stdio.h>
#include <iostream>
#include <typeinfo>

#include <assert.h>

template<typename T>
void testDescriptor() {
  std::string type_name = T::descriptor()->full_name();
  std::cout << type_name << std::endl;

  const google::protobuf::Descriptor* descriptor =
    google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);

  assert(descriptor == T::descriptor());
  std::cout << "FindMessageTypeByName() = " << descriptor << std::endl;
  std::cout << "T::descriptor()         = " << T::descriptor() << std::endl;
  std::cout << std::endl;

  const google::protobuf::Message* prototype =
    google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
  assert(prototype == &T::default_instance());
  std::cout << "GetPrototype()      =" << prototype << std::endl;
  std::cout << "T::default_instance() = " << &T::default_instance() << std::endl;
  std::cout << std::endl;

  T* new_obj = dynamic_cast<T*>(prototype->New());
  assert(new_obj != NULL);
  assert(new_obj != prototype);
  assert(typeid(*new_obj) == typeid(T::default_instance()));
  std::cout << "prototype->New() = " << new_obj << std::endl << std::endl;
  delete new_obj;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  testDescriptor<simple_reactor::Query>();
  testDescriptor<simple_reactor::Answer>();

  google::protobuf::Message* newQuery = createMessage("simple_reactor.Query");
  assert(newQuery != NULL);
  assert(typeid(*newQuery) == typeid(simple_reactor::Query::default_instance()));
  std::cout << "createMessage(Query) ="<< newQuery->DebugString() << std::endl;

  delete newQuery;

  google::protobuf::ShutdownProtobufLibrary();
}
