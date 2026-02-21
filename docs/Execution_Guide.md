# Installation and Execution Guide

This guide details the systematic configuration and execution of the unified multi-modal autonomous drone simulator using ROS 2 Humble, ArduPilot SITL, and Gazebo.

---

## Prerequisites
- Ubuntu 22.04 LTS (or WSL2)
- 8GB+ RAM recommended
- Internet access for package downloads

---

## Part 1: Environment Setup

### 1. Locale Configuration
```bash
locale  # check for UTF-8
sudo apt update && sudo apt install locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8
```
**Why:** UTF-8 encoding is essential for consistent text handling across ROS 2 and all tooling.

### 2. Enable Universe Repository
```bash
sudo apt install software-properties-common
sudo add-apt-repository universe
```

### 3. Add the ROS 2 Repository
```bash
sudo apt update && sudo apt install curl -y
export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F\" '{print $4}')
curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo ${UBUNTU_CODENAME:-${VERSION_CODENAME}})_all.deb"
sudo dpkg -i /tmp/ros2-apt-source.deb
```

### 4. Install ROS 2 Humble Desktop
```bash
sudo apt update && sudo apt upgrade
sudo apt install ros-humble-desktop
```

### 5. Configure ROS 2 in Bash
```bash
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
echo "export ROS_DOMAIN_ID=22" >> ~/.bashrc
echo "export ROS_LOCALHOST_ONLY=1" >> ~/.bashrc
source ~/.bashrc
```

---

## Part 2: Build ROS 2 Workspace

### 6. Create Workspace and Install Dependencies
```bash
mkdir -p ~/ardu_ws/src
cd ~/ardu_ws/src
git clone https://github.com/ros/ros_tutorials.git -b humble
cd ~/ardu_ws
rosdep install -i --from-path src --rosdistro humble -y
sudo apt install python3-colcon-common-extensions
colcon build
source install/local_setup.bash
```

### 7. Build Micro XRCE-DDS-Gen (Required for ArduPilot DDS)
```bash
sudo apt install default-jre
git clone --recurse-submodules https://github.com/ardupilot/Micro-XRCE-DDS-Gen.git ~/Micro-XRCE-DDS-Gen
cd ~/Micro-XRCE-DDS-Gen
./gradlew assemble
echo "export PATH=\$PATH:$HOME/Micro-XRCE-DDS-Gen/scripts" >> ~/.bashrc
source ~/.bashrc
```

---

## Part 3: ArduPilot SITL Setup

### 8. Install ArduPilot and Compiler Tools
```bash
sudo apt-get install git gitk git-gui gcc-arm-none-eabi
git clone https://github.com/ArduPilot/ardupilot.git ~/ardupilot
cd ~/ardupilot
Tools/environment_install/install-prereqs-ubuntu.sh -y
./waf distclean
./waf configure --board sitl
./waf copter -v
```

### 9. Import ArduPilot ROS 2 Packages
```bash
cd ~/ardu_ws
vcs import --recursive --input https://raw.githubusercontent.com/ArduPilot/ardupilot/master/Tools/ros2/ros2.repos src
source /opt/ros/humble/setup.bash
rosdep update && rosdep install --from-paths src --ignore-src -r -y
colcon build --packages-up-to ardupilot_sitl
```

### 10. Install and Verify MAVProxy
```bash
sudo pip3 install MAVProxy
mavproxy.py --version
```

---

## Part 4: Gazebo Garden Integration

### 11. Install Gazebo Garden
```bash
sudo apt-get install lsb-release curl gnupg
sudo curl https://packages.osrfoundation.org/gazebo.gpg --output /usr/share/keyrings/pkgs-osrf-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/pkgs-osrf-archive-keyring.gpg] https://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null
sudo apt-get update && sudo apt-get install gz-garden
echo "export GZ_VERSION=garden" >> ~/.bashrc
source ~/.bashrc
```

### 12. Import and Build ArduPilot-Gazebo Bridge
```bash
cd ~/ardu_ws
vcs import --input https://raw.githubusercontent.com/ArduPilot/ardupilot_gz/main/ros2_gz.repos --recursive src
rosdep install --from-paths src --ignore-src -y
colcon build --packages-up-to ardupilot_gz_bringup
source install/setup.bash
```

---

## Part 5: Running the Simulation

### 13. Launch SITL Simulation

**Figure 3.1.1: SITL Simulation**
![SITL Simulation](../assets/figures/Figure%203.1.1.jpg)

```bash
cd ~/ardupilot/Tools/autotest
./sim_vehicle.py -v ArduCopter -f quadcopter --console --map --osd
```

### 14. Launch Gazebo + ROS 2 + ArduPilot

**Figure 3.1.2: Gazebo + ROS 2**
![Gazebo + ROS](../assets/figures/Figure%203.1.2.jpg)

```bash
source install/setup.bash
ros2 launch ardupilot_gz_bringup iris_maze.launch.py lidar_dim:=2
```

### 15. Activate YOLO Perception

**Figure 3.2: Gazebo + YOLO Perception**
![Gazebo + YOLO](../assets/figures/Figure%203.2.png)

```bash
source ~/ardu_ws/install/setup.bash
roslaunch darknet_ros darknet_ros.launch
```

### 16. Launch SLAM Mapping
```bash
ros2 launch ardupilot_cartographer cartographer.launch.py
```

---

## Part 6: Autonomous Mission Execution

### 17. Search and Rescue Mission
```bash
roslaunch iq_sim hills.launch   # or: roslaunch iq_sim lidar.launch
./startsitl.sh
source ~/ardu_ws/install/setup.bash
roslaunch iq_sim apm.launch
rosrun iq_gnc sr               # Search and land on victim detection
```

### 18. Obstacle Avoidance Mode
```bash
rosrun iq_gnc avoidance        # Dynamic avoidance with LiDAR + YOLO
```

### 19. Manual Control via MAVProxy

**Figure 3.1.3: MAVProxy Interface**
![MAVProxy](../assets/figures/Figure%203.1.3.jpg)

```bash
mavproxy.py --console --map --aircraft test --master=:14550
# In MAVProxy console:
mode guided
arm throttle
takeoff 2.5
velocity 1 0 0  # Move forward
```
