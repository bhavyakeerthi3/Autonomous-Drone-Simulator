# System Architecture and Overview

## Proposed System
The proposed system presents a fully simulated autonomous drone framework that integrates LiDAR-based odometry, optical flow-derived motion cues, and deep learning perception for environment-aware navigation. The complete pipeline is executed in ROS 2 and Gazebo, facilitating controlled testing without physical hardware.

### Core Architecture Components
| # | Component | Technology |
|---|---|---|
| 1 | **Sensor Simulation** | Gazebo (LiDAR + RGB Camera) |
| 2 | **Optical Flow Computation** | Visual motion vectors between frames |
| 3 | **Multi-Sensor Fusion** | Factor Graph Optimization (FGO) |
| 4 | **Object Detection** | YOLOv8 (Darknet ROS integration) |
| 5 | **Scenario Validation** | Disaster-mimicking Gazebo worlds |

---

## Software Stack

| Category | Tools & Frameworks |
|---|---|
| **OS** | Ubuntu 18.04 / 20.04 / 22.04 LTS · WSL2 |
| **Middleware** | ROS 2 Humble |
| **Simulation** | Gazebo Garden · ArduPilot SITL |
| **Perception** | YOLOv8 · OpenCV · PyTorch |
| **Localization** | LiDAR-SLAM · Google Cartographer |
| **Languages** | Python 3 · C++17 |
| **Standards** | IEEE 802.15.4 · ISO 12100 · ROS REP |

---

## Working Methodology

### Sensor Simulation Setup
The Gazebo environment hosts a virtual UAV equipped with LiDAR and an RGB camera. All sensor data are published via ROS topics (`/scan`, `/camera/image_raw`), with strict temporal synchronization.

### Optical Flow Estimation
Motion information is derived from consecutive image frames using visual motion constraints, yielding an instantaneous velocity field representing drone motion.

### Sensor Fusion via Factor Graph Optimization (LOFF)
The system mimics LOFF behavior by fusing LiDAR odometry (point cloud alignment) with optical motion estimation within a Factor Graph Optimization framework. This minimizes overall error across accumulated constraints, yielding a robust, consistent fused pose estimate.

### Object Detection via YOLOv8
YOLOv8 processes camera frames to detect and classify objects (humans, vehicles). Bounding box outputs are integrated with the localization system to implement perception-driven feedback — adjusting trajectory to avoid collisions and triggering intelligent landing behavior.

---

## Compliance Standards
- **Communication & Data Transfer**: IEEE 802.15.4, IEEE 1722, ROS REP Standards
- **Safety & Compliance**: ISO 12100, ASTM F3201, IEEE 829
- **Technical Precision**: IEEE 754 (floating-point precision)
- **Simulation Standards**: Gazebo SDF & ArduPilot SITL modeling
