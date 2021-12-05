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

#ifndef RCLCPP_LIFECYCLE__LIFECYCLE_SERVICE_HPP_
#define RCLCPP_LIFECYCLE__LIFECYCLE_SERVICE_HPP_

#include <memory>
#include <string>

#include "rclcpp/any_service_callback.hpp"
#include "rclcpp/logging.hpp"
#include "rclcpp/service.hpp"

namespace rclcpp_lifecycle
{
/// base class with only
/**
 * pure virtual functions. A managed
 * node can then deactivate or activate
 * the service handling.
 * It is more a convenient interface class
 * than a necessary base class.
 */
class LifecycleServiceInterface
{
public:
  virtual ~LifecycleServiceInterface() {}
  virtual void on_activate() = 0;
  virtual void on_deactivate() = 0;
  virtual bool is_activated() = 0;
};

/// brief child class of rclcpp Service class.
/**
 * Overrides all service functions to check for enabled/disabled state.
 */
template<typename ServiceT>
class LifecycleService : public LifecycleServiceInterface, public rclcpp::Service<ServiceT>
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(LifecycleService)

  /// Default constructor.
  /**
   * The constructor for a LifecycleService is almost never called directly.
   * Instead, services should be instantiated through the function
   * rclcpp::create_service().
   *
   * \param[in] node_handle NodeBaseInterface pointer that is used in part of
   * the setup. \param[in] service_name Name of the topic to publish to.
   * \param[in] any_callback User defined callback to call when a client request
   * is received. \param[in] service_options options for the subscription.
   */
  LifecycleService(
    std::shared_ptr<rcl_node_t> node_handle, const std::string & service_name,
    rclcpp::AnyServiceCallback<ServiceT> any_callback, rcl_service_options_t & service_options)
  : rclcpp::Service<ServiceT>(node_handle, service_name, any_callback, service_options),
    enabled_(false),
    should_log_(true),
    logger_(rclcpp::get_logger("LifecycleService"))
  {
  }

  /// Default constructor.
  /**
   * The constructor for a LifecycleService is almost never called directly.
   * Instead, services should be instantiated through the function
   * rclcpp::create_service().
   *
   * \param[in] node_handle NodeBaseInterface pointer that is used in part of
   * the setup. \param[in] service_handle service handle. \param[in]
   * any_callback User defined callback to call when a client request is
   * received.
   */
  LifecycleService(
    std::shared_ptr<rcl_node_t> node_handle, std::shared_ptr<rcl_service_t> service_handle,
    rclcpp::AnyServiceCallback<ServiceT> any_callback)
  : rclcpp::Service<ServiceT>(node_handle, service_handle, any_callback),
    enabled_(false),
    should_log_(true),
    logger_(rclcpp::get_logger("LifecycleService"))
  {
  }

  /// Default constructor.
  /**
   * The constructor for a LifecycleService is almost never called directly.
   * Instead, services should be instantiated through the function
   * rclcpp::create_service().
   *
   * \param[in] node_handle NodeBaseInterface pointer that is used in part of
   * the setup. \param[in] service_handle service handle. \param[in]
   * any_callback User defined callback to call when a client request is
   * received.
   */
  LifecycleService(
    std::shared_ptr<rcl_node_t> node_handle, rcl_service_t * service_handle,
    rclcpp::AnyServiceCallback<ServiceT> any_callback)
  : rclcpp::Service<ServiceT>(node_handle, service_handle, any_callback),
    enabled_(false),
    should_log_(true),
    logger_(rclcpp::get_logger("LifecycleService"))
  {
  }

  LifecycleService() = delete;

  virtual ~LifecycleService() {}

  /// add to wait set
  /**
   * The function checks whether the communication
   * was enabled or disabled and forwards the add_to_wait_set
   * request to the actual rclcpp Service base class
   */
  bool add_to_wait_set(rcl_wait_set_t * wait_set) override
  {
    if (!enabled_) {
      log_service_not_enabled();
      return true;
    }

    return rclcpp::Service<ServiceT>::add_to_wait_set(wait_set);
  }

  void on_activate() override {enabled_ = true;}

  void on_deactivate() override
  {
    enabled_ = false;
    should_log_ = true;
  }

  bool is_activated() override {return enabled_;}

private:
  void log_service_not_enabled()
  {
    // Nothing to do if we are not meant to log
    if (!should_log_) {
      return;
    }

    // Log the message
    RCLCPP_WARN(
      logger_,
      "Trying to wait for the service '%s', but the service "
      "is not activated",
      this->get_service_name());

    // We stop logging until the flag gets enabled again
    should_log_ = false;
  }

  bool enabled_ = false;
  bool should_log_ = true;
  rclcpp::Logger logger_;
};

}  // namespace rclcpp_lifecycle

#endif  // RCLCPP_LIFECYCLE__LIFECYCLE_SERVICE_HPP_
