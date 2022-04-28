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

#include "rosidl_typesupport_microxrcedds_test_msg/msg/regression10.h"

bool compare_basic_type_field(const void * field1, const void * field2, const uint32_t type, const size_t count) {

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

bool introspect_and_compare_inner(const void * msg1, const void * msg2, const rosidl_typesupport_introspection_c__MessageMembers * members, size_t rec) {
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

  char tabs[100] = "";
  for (size_t i = 0; i < rec; i++)
  {
    tabs[i] = '\t';
  }




  bool return_value = true;

  for (uint32_t m = 0; m < members->member_count_; m++) {
    bool member_check = true;

    auto member = members->members_[m];
    std::cout << tabs << "member " << m << ": " << member.name_ << " [" << names[member.type_id_] << "]";
    bool is_array = member.is_array_;

    if(member.type_id_ == rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE){
      // Nested type
      std::cout << " (nested type)" << std::endl;
      const rosidl_message_type_support_t * introspection_typesupport = get_message_typesupport_handle(member.members_, rosidl_typesupport_introspection_c__identifier);
      const rosidl_typesupport_introspection_c__MessageMembers * introspection_members = (rosidl_typesupport_introspection_c__MessageMembers*) introspection_typesupport->data;
      size_t count = (is_array) ? member.array_size_ : 1;
      for (size_t i = 0; i < count; i++) {
        std::cout << tabs << ((count > 0) ? "----" : "") << std::endl;
        member_check &= introspect_and_compare_inner(msg1 + member.offset_ + i * introspection_members->size_of_, msg2 + member.offset_ + i * introspection_members->size_of_, introspection_members, rec + 1);
      }
    } else {
      // Basic type
      member_check &= compare_basic_type_field(msg1 + member.offset_, msg2 + member.offset_, member.type_id_, (is_array) ? member.array_size_ : 1);
      std::cout << " -  " << ((member_check) ? "OK" : "FAIL") << std::endl;
    }

    return_value &= member_check;
  }

  return return_value;
}

bool introspect_and_compare(const void * msg1, const void * msg2, const rosidl_typesupport_introspection_c__MessageMembers * members) {
  return introspect_and_compare_inner(msg1, msg2, members, 0);
}

TEST(asdfasdf, asdfasdf){
  const rosidl_message_type_support_t * type_support_c = ROSIDL_GET_MSG_TYPE_SUPPORT(rosidl_typesupport_microxrcedds_test_msg, msg, Regression10);
  ASSERT_NE(type_support_c, nullptr);

  const rosidl_message_type_support_t * xrce_typesupport = get_message_typesupport_handle(type_support_c, rosidl_typesupport_microxrcedds_c__identifier);
  const rosidl_message_type_support_t * fast_typesupport = get_message_typesupport_handle(type_support_c, rosidl_typesupport_fastrtps_c__identifier);
  const rosidl_message_type_support_t * introspection_typesupport = get_message_typesupport_handle(type_support_c, rosidl_typesupport_introspection_c__identifier);

  ASSERT_NE(xrce_typesupport, nullptr);
  ASSERT_NE(fast_typesupport, nullptr);
  ASSERT_NE(introspection_typesupport, nullptr);

  const message_type_support_callbacks_xrce_t * xrce_callbacks = static_cast<const message_type_support_callbacks_xrce_t *>(xrce_typesupport->data);
  const message_type_support_callbacks_fast_t * fast_callbacks = static_cast<const message_type_support_callbacks_fast_t *>(fast_typesupport->data);

  ASSERT_NE(xrce_callbacks, nullptr);
  ASSERT_NE(fast_callbacks, nullptr);

  const rosidl_typesupport_introspection_c__MessageMembers * introspection = (rosidl_typesupport_introspection_c__MessageMembers*) introspection_typesupport->data;

  // ---- SERIALIZE AND COMPARE BUFFERS ----
  rosidl_typesupport_microxrcedds_test_msg__msg__Regression10 msg = {};
  introspection->init_function(&msg, ROSIDL_RUNTIME_C_MSG_INIT_ALL);

  size_t xrce_size = xrce_callbacks->get_serialized_size(&msg);
  size_t fast_size = fast_callbacks->get_serialized_size(&msg);
  EXPECT_EQ(xrce_size, fast_size);

  // size_t xrce_max_size = xrce_callbacks->max_serialized_size();
  // bool full_bounded;
  // size_t fast_max_size = fast_callbacks->max_serialized_size(full_bounded);
  // EXPECT_EQ(xrce_max_size, fast_max_size);

  uint8_t * xrce_buffer = (uint8_t *) calloc(xrce_size, sizeof(uint8_t));
  ucdrBuffer xrce_cdr;
  ucdr_init_buffer(&xrce_cdr, xrce_buffer, xrce_size);
  ASSERT_TRUE(xrce_callbacks->cdr_serialize(&msg, &xrce_cdr));

  uint8_t * fast_buffer = (uint8_t *) calloc(fast_size, sizeof(uint8_t));
  eprosima::fastcdr::FastBuffer fast_cdr_buffer((char *) fast_buffer, fast_size);
  eprosima::fastcdr::Cdr fast_cdr(fast_cdr_buffer);
  ASSERT_TRUE(fast_callbacks->cdr_serialize(&msg, fast_cdr));

  ASSERT_EQ(xrce_cdr.offset, fast_cdr.getSerializedDataLength());

  bool equal_buffers = true;
  for(size_t i = 0; i < xrce_size && equal_buffers; ++i){
    equal_buffers &= xrce_buffer[i] == fast_buffer[i];
    if(!equal_buffers) {
      std::cout << "Error at position " << i << std::endl;
    }
  }
  ASSERT_TRUE(equal_buffers);

  // ---- CROSS DESERIALIZE AND COMPARE ----
  // micro-ROS -> ROS 2
  {
    eprosima::fastcdr::FastBuffer input_buffer((char *) xrce_buffer, xrce_size);
    eprosima::fastcdr::Cdr cdr(fast_cdr_buffer);

    rosidl_typesupport_microxrcedds_test_msg__msg__Regression10 out = {};
    memset(&out, 0xFF, sizeof(out));
    ASSERT_TRUE(fast_callbacks->cdr_deserialize(cdr, &out));
    ASSERT_TRUE(introspect_and_compare(&msg, &out, introspection));
  }

  // ROS 2 -> micro-ROS
  {
    ucdrBuffer cdr;
    ucdr_init_buffer(&cdr, fast_buffer, fast_size);

    rosidl_typesupport_microxrcedds_test_msg__msg__Regression10 out = {};
    memset(&out, 0xFF, sizeof(out));
    ASSERT_TRUE(xrce_callbacks->cdr_deserialize(&cdr, &out));
    ASSERT_TRUE(introspect_and_compare(&msg, &out, introspection));
  }

  {
    eprosima::fastcdr::FastBuffer input_buffer((char *) fast_buffer, fast_size);
    eprosima::fastcdr::Cdr cdr(fast_cdr_buffer);

    rosidl_typesupport_microxrcedds_test_msg__msg__Regression10 out = {};
    memset(&out, 0xFF, sizeof(out));
    ASSERT_TRUE(fast_callbacks->cdr_deserialize(cdr, &out));
    ASSERT_TRUE(introspect_and_compare(&msg, &out, introspection));
  }

  // ROS 2 -> micro-ROS
  {
    ucdrBuffer cdr;
    ucdr_init_buffer(&cdr, xrce_buffer, xrce_size);

    rosidl_typesupport_microxrcedds_test_msg__msg__Regression10 out = {};
    memset(&out, 0xFF, sizeof(out));
    ASSERT_TRUE(xrce_callbacks->cdr_deserialize(&cdr, &out));
    ASSERT_TRUE(introspect_and_compare(&msg, &out, introspection));
  }

  free(xrce_buffer);
  free(fast_buffer);
  introspection->fini_function(&msg);

  // EXPECT_EQ(cdr.offset, 166UL);
}