// Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <cstdbool>

#include "rosidl_runtime_c/message_type_support_struct.h"

#include <rosidl_typesupport_microxrcedds_c/identifier.h>
#include <rosidl_typesupport_fastrtps_c/identifier.h>
#include <rosidl_typesupport_introspection_c/identifier.h>

// Workaround for having two coexistent message_type_support_callbacks_t structs
#define message_type_support_callbacks_t message_type_support_callbacks_fast_t
#include <rosidl_typesupport_fastrtps_cpp/message_type_support.h>
#undef message_type_support_callbacks_t
#define message_type_support_callbacks_t message_type_support_callbacks_xrce_t
#include <rosidl_typesupport_microxrcedds_c/message_type_support.h>
#undef message_type_support_callbacks_t

#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosidl_typesupport_introspection_c/field_types.h"

// Used types
#include "rosidl_typesupport_microxrcedds_test_msg/msg/regression10.h"

template <typename T>
class CrossSerialization {
  public:

  #define QUICK_RETURN(x) if(!(x)) return x;

  CrossSerialization(const rosidl_message_type_support_t * type_support_c) :
    xrce_buffer(nullptr),
    xrce_size(0),
    fast_buffer(nullptr),
    fast_size(0),
    xrce_typesupport(get_message_typesupport_handle(type_support_c, rosidl_typesupport_microxrcedds_c__identifier)),
    fast_typesupport(get_message_typesupport_handle(type_support_c, rosidl_typesupport_fastrtps_c__identifier)),
    introspection_typesupport(get_message_typesupport_handle(type_support_c, rosidl_typesupport_introspection_c__identifier)),
    xrce_callbacks(static_cast<const message_type_support_callbacks_xrce_t *>(xrce_typesupport->data)),
    fast_callbacks(static_cast<const message_type_support_callbacks_fast_t *>(fast_typesupport->data)),
    introspection((rosidl_typesupport_introspection_c__MessageMembers*) introspection_typesupport->data) {};

  bool check() {
    bool ret = true;
    ret &= xrce_typesupport != nullptr;
    ret &= fast_typesupport != nullptr;
    ret &= introspection_typesupport != nullptr;
    ret &= xrce_callbacks != nullptr;
    ret &= fast_callbacks != nullptr;
    ret &= introspection != nullptr;
    return ret;
  }

  ~CrossSerialization() {
    if(xrce_buffer) free(xrce_buffer);
    if(fast_buffer) free(fast_buffer);
  }

  bool serialize_and_compare_buffers(T & msg) {
    xrce_size = xrce_callbacks->get_serialized_size(&msg);
    fast_size = fast_callbacks->get_serialized_size(&msg);
    QUICK_RETURN(xrce_size == fast_size);

    // size_t xrce_max_size = xrce_callbacks->max_serialized_size();
    // bool full_bounded;
    // size_t fast_max_size = fast_callbacks->max_serialized_size(full_bounded);
    // QUICK_RETURN(xrce_max_size == fast_max_size);

    xrce_buffer = (uint8_t *) calloc(xrce_size, sizeof(uint8_t));
    ucdrBuffer xrce_cdr;
    ucdr_init_buffer(&xrce_cdr, xrce_buffer, xrce_size);
    QUICK_RETURN(xrce_callbacks->cdr_serialize(&msg, &xrce_cdr));

    fast_buffer = (uint8_t *) calloc(fast_size, sizeof(uint8_t));
    eprosima::fastcdr::FastBuffer fast_cdr_buffer((char *) fast_buffer, fast_size);
    eprosima::fastcdr::Cdr fast_cdr(fast_cdr_buffer);
    QUICK_RETURN(fast_callbacks->cdr_serialize(&msg, fast_cdr));

    QUICK_RETURN(xrce_cdr.offset == fast_cdr.getSerializedDataLength());

    bool equal_buffers = true;
    for(size_t i = 0; i < xrce_size && equal_buffers; ++i){
      equal_buffers &= xrce_buffer[i] == fast_buffer[i];
      if(!equal_buffers) {
        std::cout << "Error at position " << i << std::endl;
      }
    }
    QUICK_RETURN(equal_buffers);

    return true;
  }

  bool deserialize_with_fastcdr_and_compare(uint8_t * buff, size_t size, T & original_msg) {
    eprosima::fastcdr::FastBuffer input_buffer((char *) buff, size);
    eprosima::fastcdr::Cdr cdr(input_buffer);

    T out = {};
    this->introspection->init_function(&out, ROSIDL_RUNTIME_C_MSG_INIT_ALL);
    QUICK_RETURN(fast_callbacks->cdr_deserialize(cdr, &out));
    QUICK_RETURN(introspect_and_compare(&original_msg, &out));
    this->introspection->fini_function(&out);

    return true;
  }

  bool deserialize_with_microcdr_and_compare(uint8_t * buff, size_t size, T & original_msg) {
    ucdrBuffer cdr;
    ucdr_init_buffer(&cdr, buff, size);

    T out = {};
    this->introspection->init_function(&out, ROSIDL_RUNTIME_C_MSG_INIT_ALL);
    QUICK_RETURN(xrce_callbacks->cdr_deserialize(&cdr, &out));
    QUICK_RETURN(introspect_and_compare(&original_msg, &out));
    this->introspection->fini_function(&out);

    return true;
  }

  using recursive_callback = std::function<bool(const void * msg1, const void * msg2, const rosidl_typesupport_introspection_c__MessageMember m, size_t level)>;

  bool introspect_and_execute(const void * msg1, const void * msg2, recursive_callback cb, const rosidl_typesupport_introspection_c__MessageMembers * members = nullptr, size_t rec = 0) {
    members = (nullptr == members) ? introspection : members;
    bool return_value = true;

    for (uint32_t m = 0; m < members->member_count_; m++) {
      bool member_check = true;

      auto member = members->members_[m];
      bool is_array = member.is_array_;

      if(member.type_id_ == rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE){
        // Nested type
        const rosidl_message_type_support_t * aux_introspection_typesupport = get_message_typesupport_handle(member.members_, rosidl_typesupport_introspection_c__identifier);
        const rosidl_typesupport_introspection_c__MessageMembers * introspection_members = (rosidl_typesupport_introspection_c__MessageMembers*) aux_introspection_typesupport->data;
        size_t count = (is_array) ? member.array_size_ : 1;
        member_check &= cb(msg1 + member.offset_, msg2 + member.offset_, member, rec);
        for (size_t i = 0; i < count; i++) {
          member_check &= introspect_and_execute(msg1 + member.offset_ + i * introspection_members->size_of_, msg2 + member.offset_ + i * introspection_members->size_of_, cb, introspection_members, rec + 1);
        }
      } else {
        // Basic type
        member_check &= cb(msg1 + member.offset_, msg2 + member.offset_, member, rec);
      }

      return_value &= member_check;
    }

    return return_value;
  }

  void print_type_info() {
    char * names[20] = {};
    names[1] = "rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT";
    names[2] = "rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE";
    names[3] = "rosidl_typesupport_introspection_c__ROS_TYPE_LONG_DOUBLE";
    names[4] = "rosidl_typesupport_introspection_c__ROS_TYPE_CHAR";
    names[5] = "rosidl_typesupport_introspection_c__ROS_TYPE_WCHAR";
    names[6] = "rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN";
    names[7] = "rosidl_typesupport_introspection_c__ROS_TYPE_OCTET";
    names[8] = "rosidl_typesupport_introspection_c__ROS_TYPE_UINT8";
    names[9] = "rosidl_typesupport_introspection_c__ROS_TYPE_INT8";
    names[10] = "rosidl_typesupport_introspection_c__ROS_TYPE_UINT16";
    names[11] = "rosidl_typesupport_introspection_c__ROS_TYPE_INT16";
    names[12] = "rosidl_typesupport_introspection_c__ROS_TYPE_UINT32";
    names[13] = "rosidl_typesupport_introspection_c__ROS_TYPE_INT32";
    names[14] = "rosidl_typesupport_introspection_c__ROS_TYPE_UINT64";
    names[15] = "rosidl_typesupport_introspection_c__ROS_TYPE_INT64";
    names[16] = "rosidl_typesupport_introspection_c__ROS_TYPE_STRING";
    names[17] = "rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING";
    names[18] = "rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE";

    introspect_and_execute(nullptr, nullptr, [&](const void * /* msg1 */, const void * /* msg2 */, const rosidl_typesupport_introspection_c__MessageMember m, size_t level) -> bool {
      char tabs[100] = "";
      for (size_t i = 0; i < level; i++){tabs[i] = '\t';}

      std::cout << tabs << "member " << m.name_ << " [" << names[m.type_id_] << "]" << std::endl;
      return true;
    });
  }

  static bool compare_basic_type_field(const void * field1, const void * field2, const uint32_t type, const size_t count) {
    uint8_t size = 0;
    switch (type)
    {
    case rosidl_typesupport_introspection_c__ROS_TYPE_INT8:
    case rosidl_typesupport_introspection_c__ROS_TYPE_UINT8:
    case rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN:
    case rosidl_typesupport_introspection_c__ROS_TYPE_CHAR:
    case rosidl_typesupport_introspection_c__ROS_TYPE_OCTET:
      size = 1;
      break;
    case rosidl_typesupport_introspection_c__ROS_TYPE_UINT16:
    case rosidl_typesupport_introspection_c__ROS_TYPE_INT16:
      size = 2;
      break;
    case rosidl_typesupport_introspection_c__ROS_TYPE_UINT32:
    case rosidl_typesupport_introspection_c__ROS_TYPE_INT32:
    case rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT:
      size = 4;
      break;
    case rosidl_typesupport_introspection_c__ROS_TYPE_UINT64:
    case rosidl_typesupport_introspection_c__ROS_TYPE_INT64:
    case rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE:
      size = 8;
      break;
    case rosidl_typesupport_introspection_c__ROS_TYPE_LONG_DOUBLE:
    case rosidl_typesupport_introspection_c__ROS_TYPE_WCHAR:
    case rosidl_typesupport_introspection_c__ROS_TYPE_STRING:
    case rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING:
    default:
      break;
    }

    return (!size) ? false : memcmp(field1, field2, size * count) == 0;
  }

  bool introspect_and_compare(const void * msg1, const void * msg2, const rosidl_typesupport_introspection_c__MessageMembers * members = nullptr, size_t rec = 0) {
    return introspect_and_execute(msg1, msg2, [&](const void * msg1, const void * msg2, const rosidl_typesupport_introspection_c__MessageMember m, size_t /* level */) -> bool {
        if (m.type_id_ != rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE && !m.is_array_) {
          size_t count = (m.is_array_) ? m.array_size_ : 1;
          bool com = compare_basic_type_field(msg1, msg2, m.type_id_, count);
          if (!com) {
            std::cout << "compare_basic_type_field failed for member " << m.name_ << " of type " << m.type_id_ << std::endl;
          }
          return com;
        }
        return true;
    });
  }

  uint8_t * xrce_buffer;
  uint8_t * fast_buffer;

  size_t xrce_size;
  size_t fast_size;

  const rosidl_message_type_support_t * xrce_typesupport;
  const rosidl_message_type_support_t * fast_typesupport;
  const rosidl_message_type_support_t * introspection_typesupport;

  const message_type_support_callbacks_xrce_t * xrce_callbacks;
  const message_type_support_callbacks_fast_t * fast_callbacks;

  rosidl_typesupport_introspection_c__MessageMembers * introspection;
};

using MyTypes = ::testing::Types<char, int, rosidl_typesupport_microxrcedds_test_msg__msg__Regression10>;
TYPED_TEST_SUITE(SerDesTests, MyTypes);

TEST(SerDesTests, Example) {

  CrossSerialization<rosidl_typesupport_microxrcedds_test_msg__msg__Regression10> serdes(ROSIDL_GET_MSG_TYPE_SUPPORT(rosidl_typesupport_microxrcedds_test_msg, msg, Regression10));

  ASSERT_TRUE(serdes.check());
  rosidl_typesupport_microxrcedds_test_msg__msg__Regression10 msg = {};
  serdes.introspection->init_function(&msg, ROSIDL_RUNTIME_C_MSG_INIT_ALL);
  ASSERT_TRUE(serdes.serialize_and_compare_buffers(msg));
  ASSERT_TRUE(serdes.deserialize_with_fastcdr_and_compare(serdes.fast_buffer, serdes.fast_size, msg));
  ASSERT_TRUE(serdes.deserialize_with_microcdr_and_compare(serdes.xrce_buffer, serdes.xrce_size, msg));
  ASSERT_TRUE(serdes.deserialize_with_fastcdr_and_compare(serdes.xrce_buffer, serdes.xrce_size, msg));
  ASSERT_TRUE(serdes.deserialize_with_microcdr_and_compare(serdes.fast_buffer, serdes.fast_size, msg));
  serdes.introspection->fini_function(&msg);

  rosidl_typesupport_microxrcedds_test_msg__msg__Regression10 t1 = {};
  rosidl_typesupport_microxrcedds_test_msg__msg__Regression10 t2 = {};

  t1.b = 1;
  t1.c = 2;

  ASSERT_TRUE(serdes.introspect_and_compare(&t1, &t2));
}