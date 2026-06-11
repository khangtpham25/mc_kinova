mc_kinova
==

mc_rtc robot module for Kinova Gen3 robot.
By default, only Kinova and KinovaFloatingBase are supported.
To attach different tools to the end effector, please install [mc_robot_tools](#dependencies).

### Full list of available robots:
- Kinova
- KinovaCamera *(optional)*
- KinovaGripper *(optional)*
- KinovaCameraGripper *(optional)*
- KinovaBota *(optional)*
- KinovaBotaDS4 *(optional)*
- KinovaBotaPlate *(optional)*
- KinovaBotaScrew *(optional)*

> **Note:** All robots have their floating base variation available via `<robot-name>FloatingBase`.

## Dependencies

- [ROS2](https://docs.ros.org/)
- [mc_rtc](https://jrl-umi3218.github.io/mc_rtc/)
- [kortex_description](https://github.com/Kinovarobotics/ros2_kortex)
- [mc_robot_tools](https://github.com/isri-aist/mc_robot_tools) (optional)
  - [robotiq_description](https://github.com/PickNikRobotics/ros2_robotiq_gripper/tree/main/robotiq_description)
  - [bota_driver](https://gitlab.com/botasys/drivers/bota_driver_ros2)

To install `kortex_description`, `robotiq_description`, and `bota_driver`, you can use the following command:

```sh
mkdir -p ros2_ws/src && cd ros2_ws/src

git clone https://github.com/Kinovarobotics/ros2_kortex.git
# Optional
git clone https://github.com/PickNikRobotics/ros2_robotiq_gripper.git
git clone https://gitlab.com/botasys/drivers/bota_driver_ros2.git

cd ../..
colcon build --symlink-install \
  --packages-select \
    kortex_description \
    robotiq_description \
    bota_driver \
  --cmake-args -Wno-dev

source "install/setup.bash"
```

## Build and Install

```sh
git clone https://github.com/isri-aist/mc_kinova.git
cd mc_kinova
mkdir -p build && cd build
cmake ..
make -j$(nproc)
sudo make install
cd ..
```

To test the module is installed correctly, run the following command, run the following command from the root of the repository:

```sh
mc_rtc_ticker -f etc/mc_rtc.yaml
```
<p align="center">
  <img src="etc/kinova.png" alt="KinovaG3 mc_rtc_ticker test" height="500">
</p>
