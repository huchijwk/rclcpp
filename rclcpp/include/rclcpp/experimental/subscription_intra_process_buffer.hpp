// Copyright 2021 Open Source Robotics Foundation, Inc.
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

#ifndef RCLCPP__EXPERIMENTAL__SUBSCRIPTION_INTRA_PROCESS_BUFFER_HPP_
#define RCLCPP__EXPERIMENTAL__SUBSCRIPTION_INTRA_PROCESS_BUFFER_HPP_

#include <rmw/rmw.h>

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "rcl/error_handling.h"

#include "rclcpp/any_subscription_callback.hpp"
#include "rclcpp/experimental/buffers/intra_process_buffer.hpp"
#include "rclcpp/experimental/create_intra_process_buffer.hpp"
#include "rclcpp/experimental/subscription_intra_process_base.hpp"
#include "rclcpp/qos.hpp"
#include "rclcpp/type_support_decl.hpp"
#include "rclcpp/waitable.hpp"
#include "tracetools/tracetools.h"

namespace rclcpp
{
namespace experimental
{

template<
  typename MessageT,
  typename Alloc = std::allocator<void>,
  typename Deleter = std::default_delete<MessageT>
>
class SubscriptionIntraProcessBuffer : public SubscriptionIntraProcessBase
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(SubscriptionIntraProcessBuffer)

  using MessageAllocTraits = allocator::AllocRebind<MessageT, Alloc>;
  using MessageAlloc = typename MessageAllocTraits::allocator_type;
  using ConstMessageSharedPtr = std::shared_ptr<const MessageT>;
  using MessageUniquePtr = std::unique_ptr<MessageT, Deleter>;

  using BufferUniquePtr = typename rclcpp::experimental::buffers::IntraProcessBuffer<
    MessageT,
    Alloc,
    Deleter
    >::UniquePtr;

  SubscriptionIntraProcessBuffer(
    std::shared_ptr<Alloc> allocator,
    rclcpp::Context::SharedPtr context,
    const std::string & topic_name,
    const rclcpp::QoS & qos_profile,
    rclcpp::IntraProcessBufferType buffer_type)
  : SubscriptionIntraProcessBase(context, topic_name, qos_profile)
  {
    // Create the intra-process buffer.
    buffer_ = rclcpp::experimental::create_intra_process_buffer<MessageT, Alloc, Deleter>(
      buffer_type,
      qos_profile,
      allocator);
  }

  bool
  is_ready(rcl_wait_set_t * wait_set)
  {
    (void) wait_set;
    return buffer_->has_data();
  }

  void
  provide_intra_process_message(ConstMessageSharedPtr message)
  {
    buffer_->add_shared(std::move(message));
    trigger_guard_condition();
  }

  void
  provide_intra_process_message(MessageUniquePtr message)
  {
    buffer_->add_unique(std::move(message));
    trigger_guard_condition();
  }

  bool
  use_take_shared_method() const
  {
    return buffer_->use_take_shared_method();
  }

protected:
  void
  trigger_guard_condition()
  {
    gc_.trigger();
  }

  BufferUniquePtr buffer_;
};

}  // namespace experimental
}  // namespace rclcpp

#endif  // RCLCPP__EXPERIMENTAL__SUBSCRIPTION_INTRA_PROCESS_BUFFER_HPP_
