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

#ifndef RCLCPP_LIFECYCLE__LIFECYCLE_SUBSCRIPTION_HPP_
#define RCLCPP_LIFECYCLE__LIFECYCLE_SUBSCRIPTION_HPP_

namespace rclcpp_lifecycle
{
/// base class with only
/**
 * pure virtual functions. A managed
 * node can then deactivate or activate
 * the publishing.
 * It is more a convenient interface class
 * than a necessary base class.
 */
class LifecycleSubscriptionInterface
{
public:
  virtual ~LifecycleSubscriptionInterface() {}
  virtual void on_activate() = 0;
  virtual void on_deactivate() = 0;
  virtual bool is_activated() = 0;
};

}

#endif  // RCLCPP_LIFECYCLE__LIFECYCLE_SUBSCRIPTION_HPP_
