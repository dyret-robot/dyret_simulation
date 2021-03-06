#pragma once

#include <memory>
#include <mutex>
#include <string>

#include <dyret_common/Configure.h>
#include <dyret_common/Pose.h>
#include <gazebo/common/Plugin.hh>
#include <gazebo/physics/physics.hh>
#include <ros/callback_queue.h>
#include <ros/ros.h>
#include <ros/spinner.h>
#include <std_srvs/SetBool.h>

namespace dyret {
	class GazeboDyretController: public gazebo::ModelPlugin {
		public:
			GazeboDyretController() {}
			~GazeboDyretController();

			void Load(gazebo::physics::ModelPtr model_, sdf::ElementPtr sdf_) override;

		protected:
			void PublishPose();
			void PublishState();
			// Load shared parameters
			bool LoadParameters();
			// Initialize ROS subsystem
			bool InitROS();
			// Reset joint targets
			bool Reset(const std_srvs::SetBool::Request& req, std_srvs::SetBool::Response& resp);
			// Internal method used to set joints to default values
			void Default(bool reset_prismatic = true);
			// Configure one or more servos
			bool Configure(const dyret_common::Configure::Request& req,
					dyret_common::Configure::Response& resp);
			// Set position of all joints
			void Pose(const dyret_common::Pose::ConstPtr& pose);

		private:
			// Pointer to connection event update from Gazebo
			gazebo::event::ConnectionPtr pose_conn;
			gazebo::event::ConnectionPtr state_conn;
			// Pointer to Dyret model
			gazebo::physics::ModelPtr model;
			// Pointer to joint controller
			gazebo::physics::JointControllerPtr ctrl;
			// Pointer to <plugin> sdf element
			sdf::ElementPtr sdf;
			// Pointer to Nodehandle
			std::unique_ptr<ros::NodeHandle> node;
			// Custom callback queue to handle ROS administration
			ros::CallbackQueue queue;
			// Asynchronous processing of callback queue
			std::unique_ptr<ros::AsyncSpinner> spin;
			// Last publish time
			gazebo::common::Time lastPublish;
			// Reset service
			ros::ServiceServer reset_service;
			// Configure servo service
			ros::ServiceServer config_service;
			// Publisher for Servo states
			ros::Publisher state_pub;
			ros::Publisher pose_pub;
			// Subscription for pose messages
			ros::Subscriber pose_sub;
			// Ensure that asynchronous callbacks are not interrupted by
			// callback from Gazebo
			std::mutex mutex;

			// Parameters for plugin
			double publishRate = 60.; // 60Hz default
			gazebo::common::PID mx64Pid;
			gazebo::common::PID mx106Pid;
			gazebo::common::PID extShortPid;
			gazebo::common::PID extLongPid;
	};
}
