#include "kinova.h"

#include <mc_rbdyn/RobotLoader.h>
#include <mc_rbdyn/RobotModuleMacros.h>
#include <mc_rtc/logging.h>

namespace
{

static std::vector<std::string> registered_names;

static std::string camera_name = "RealSenseD435";
static std::string camera_base_frame = "realsense_d435_base_link";
static std::string camera_wrench_frame = "realsense_d435_wrench_link";
static std::string gripper_name = "Robotiq2f85Gripper";
static std::string gripper_base_frame = "robotiq_85_base_link";
static std::string bota_name = "BFT_SENS_ECAT_M8";
static std::string bota_base_frame = "bft_sens_ecat_m8_mounting";
static std::string bota_wrench_frame = "bft_sens_ecat_m8_wrench";

/** Check if a robot module is available (without loading it) */
bool isAvailable(const std::string & module_name, std::vector<std::string> & available_robots)
{
  return std::find(available_robots.begin(), available_robots.end(), module_name) != available_robots.end();
}

static const std::vector<std::string> kinova_collision_links = {"base_link", "shoulder_link", "half_arm_1_link",
                                                                "half_arm_2_link"};

static constexpr double COL_I = 0.03;
static constexpr double COL_S = 0.015;
static constexpr double COL_D = 0.0;

void addToolCollisions(mc_rbdyn::RobotModule & module, const std::vector<std::string> & tool_links)
{
  for(const auto & kinova_link : kinova_collision_links)
  {
    for(const auto & tool_link : tool_links)
    {
      module._minimalSelfCollisions.push_back({kinova_link, tool_link, COL_I, COL_S, COL_D});
    }
  }
  module._commonSelfCollisions = module._minimalSelfCollisions;
}

} // namespace

extern "C"
{

  ROBOT_MODULE_API void MC_RTC_ROBOT_MODULE(std::vector<std::string> & names)
  {
    names = {"Kinova",
             "KinovaFloatingBase",

             "KinovaCamera",
             "KinovaCameraFloatingBase",
             "KinovaGripper",
             "KinovaGripperFloatingBase",
             "KinovaCameraGripper",
             "KinovaCameraGripperFloatingBase",

             "KinovaBota",
             "KinovaBotaFloatingBase",
             "KinovaBotaDS4",
             "KinovaBotaDS4FloatingBase",
             "KinovaBotaDS4Callib",
             "KinovaBotaDS4CallibFloatingBase",
             "KinovaBotaPlate",
             "KinovaBotaPlateFloatingBase",
             "KinovaBotaPlateCallib",
             "KinovaBotaPlateCallibFloatingBase",
             "KinovaBotaScrew",
             "KinovaBotaScrewFloatingBase",
             "KinovaBotaScrewCallib",
             "KinovaBotaScrewCallibFloatingBase"};
  }

  ROBOT_MODULE_API void destroy(mc_rbdyn::RobotModule * ptr)
  {
    delete ptr;
  }

  ROBOT_MODULE_API mc_rbdyn::RobotModule * create(const std::string & n)
  {
    ROBOT_MODULE_CHECK_VERSION("Kinova")

    // Check Callib is supported
    bool is_callib = n.find("Callib") != std::string::npos;
    if(is_callib && n.find("Bota") == std::string::npos)
    {
      mc_rtc::log::error("KinovaRobotModule callib mode requires a Bota variant with a mounted end effector");
      return nullptr;
    }

    // Create base module
    bool is_floating_base = n.find("FloatingBase") == std::string::npos;
    auto kinova = mc_robots::KinovaRobotModule("kinova", is_callib, is_floating_base);
    if(n == "Kinova" || n == "KinovaFloatingBase")
    {
      return new mc_rbdyn::RobotModule(std::move(kinova));
    }

    // Check required modules is available
    auto available_robots = mc_rbdyn::RobotLoader::available_robots();
    if(n.find("Camera") != std::string::npos && !isAvailable(camera_name, available_robots))
    {
      mc_rtc::log::error("Kinova module cannot create an object of type {}", n);
      mc_rtc::log::error("Camera module is not available");
      return nullptr;
    }

    if(n.find("Gripper") != std::string::npos && !isAvailable(gripper_name, available_robots))
    {
      mc_rtc::log::error("Kinova module cannot create an object of type {}", n);
      mc_rtc::log::error("{} module is not available", gripper_name);
      return nullptr;
    }

    if(n.find("Bota") != std::string::npos && !isAvailable(bota_name, available_robots))
    {
      mc_rtc::log::error("Kinova module cannot create an object of type {}", n);
      mc_rtc::log::error("{} module is not available", bota_name);
      return nullptr;
    }

    if(n.find("DS4") != std::string::npos && !isAvailable("DS4", available_robots))
    {
      mc_rtc::log::error("Kinova module cannot create an object of type {}", n);
      mc_rtc::log::error("{} module is not available", bota_name);
      return nullptr;
    }

    if(n.find("Plate") != std::string::npos && !isAvailable("Plate", available_robots))
    {
      mc_rtc::log::error("Kinova module cannot create an object of type {}", n);
      mc_rtc::log::error("Plate module is not available");
      return nullptr;
    }

    if(n.find("Screw") != std::string::npos && !isAvailable("Screw", available_robots))
    {
      mc_rtc::log::error("Kinova module cannot create an object of type {}", n);
      mc_rtc::log::error("Screw module is not available");
      return nullptr;
    }

    // Variants with Camera and Gripper
    auto camera =
        (n.find("Camera") != std::string::npos) ? mc_rbdyn::RobotLoader::get_robot_module(camera_name) : nullptr;
    auto gripper =
        (n.find("Gripper") != std::string::npos) ? mc_rbdyn::RobotLoader::get_robot_module(gripper_name) : nullptr;
    if(n == "KinovaGripper")
    {
      auto kinova_gripper =
          kinova.connect(*gripper, "tool_frame", gripper_base_frame, "",
                         mc_rbdyn::RobotModule::ConnectionParameters{}.X_other_connection(sva::RotZ(M_PI)));
      addToolCollisions(kinova_gripper,
                        {"robotiq_85_base_link", "robotiq_85_left_knuckle_link", "robotiq_85_right_knuckle_link",
                         "robotiq_85_left_finger_link", "robotiq_85_right_finger_link",
                         "robotiq_85_left_finger_tip_link", "robotiq_85_right_finger_tip_link"});
      return new mc_rbdyn::RobotModule(std::move(kinova_gripper));
    }

    if(n.find("KinovaCamera") != std::string::npos)
    {
      auto kinova_camera =
          kinova.connect(*camera, "tool_frame", camera_base_frame, "",
                         mc_rbdyn::RobotModule::ConnectionParameters{}.X_other_connection(sva::RotZ(0.0)));
      addToolCollisions(kinova_camera, {"realsense_d435_bracket_link", "realsense_d435_camera_link"});
      if(n == "KinovaCameraGripper")
      {
        auto kinova_camera_gripper =
            kinova_camera.connect(*gripper, camera_wrench_frame, gripper_base_frame, "",
                                  mc_rbdyn::RobotModule::ConnectionParameters{}.X_other_connection(sva::RotZ(M_PI)));
        addToolCollisions(kinova_camera_gripper,
                          {"robotiq_85_base_link", "robotiq_85_left_knuckle_link", "robotiq_85_right_knuckle_link",
                           "robotiq_85_left_finger_link", "robotiq_85_right_finger_link",
                           "robotiq_85_left_finger_tip_link", "robotiq_85_right_finger_tip_link"});
        return new mc_rbdyn::RobotModule(std::move(kinova_camera_gripper));
      }
      return new mc_rbdyn::RobotModule(std::move(kinova_camera));
    }

    // Variants with Bota
    if(n.find("Bota") != std::string::npos)
    {
      auto bota = mc_rbdyn::RobotLoader::get_robot_module(bota_name);
      // if(!kinova._forceSensors.empty())
      // {
      //   kinova._forceSensors.pop_back();
      // }
      // if(!kinova._bodySensors.empty())
      // {
      //   kinova._bodySensors.pop_back();
      // }

      auto kinova_bota =
          kinova.connect(*bota, "tool_frame", bota_base_frame, "",
                         mc_rbdyn::RobotModule::ConnectionParameters{}.X_other_connection(sva::RotZ(M_PI)));
      addToolCollisions(kinova_bota,
                        {"bft_sens_ecat_m8_mounting_0", "bft_sens_ecat_m8_mounting_1", "bft_sens_ecat_m8_mounting_2"});
      if(n == "KinovaBota" || n == "KinovaBotaFloatingBase")
      {
        return new mc_rbdyn::RobotModule(std::move(kinova_bota));
      }

      if(n.find("DS4") != std::string::npos)
      {
        auto ds4 = mc_rbdyn::RobotLoader::get_robot_module("DS4");
        auto kinova_bota_ds4 =
            kinova_bota.connect(*ds4, bota_wrench_frame, "ds4_base_link", "",
                                mc_rbdyn::RobotModule::ConnectionParameters{}.X_other_connection(sva::RotZ(M_PI)));
        addToolCollisions(kinova_bota_ds4, {"ds4_adapter_link", "ds4_actual_controller_link"});
        return new mc_rbdyn::RobotModule(std::move(kinova_bota_ds4));
      }

      if(n.find("Plate") != std::string::npos)
      {
        auto plate = mc_rbdyn::RobotLoader::get_robot_module("Plate");
        auto kinova_bota_plate =
            kinova_bota.connect(*plate, bota_wrench_frame, "plate_base_link", "",
                                mc_rbdyn::RobotModule::ConnectionParameters{}.X_other_connection(sva::RotX(M_PI / 2)));
        addToolCollisions(kinova_bota_plate, {"plate_link"});
        return new mc_rbdyn::RobotModule(std::move(kinova_bota_plate));
      }

      if(n.find("Screw") != std::string::npos)
      {
        auto screw = mc_rbdyn::RobotLoader::get_robot_module("Screw");
        auto kinova_bota_screw =
            kinova_bota.connect(*screw, bota_wrench_frame, "screw_base_link", "",
                                mc_rbdyn::RobotModule::ConnectionParameters{}.X_other_connection(sva::RotZ(0.0)));
        addToolCollisions(kinova_bota_screw, {"screw_link"});
        return new mc_rbdyn::RobotModule(std::move(kinova_bota_screw));
      }
    }

    mc_rtc::log::error("Kinova module cannot create an object of type {}", n);
    return nullptr;
  }
}
