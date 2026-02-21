# Source Code Overview

This section provides a high-level overview of the two core source files.

For the full implementations, refer to the source files in the [`src/`](../src/) directory.

---

## SLAM-LIDAR Navigation Model
**File**: [`src/SLAM_LIDAR_Model.cpp`](../src/SLAM_LIDAR_Model.cpp)

This module implements:
- LiDAR point cloud processing and obstacle detection
- Factor Graph Optimization for pose estimation
- Dynamic waypoint generation for search patterns
- Confidence-weighted sensor fusion (LiDAR + Optical Flow)
- Autonomous landing and avoidance state machine

---

## YOLO Object Detector (ROS Integration)
**File**: [`src/Yolo_Object_Detector.cpp`](../src/Yolo_Object_Detector.cpp)

This module implements:
- ROS 2 subscriber for camera feed (`/camera/image_raw`)
- Darknet YOLO inference pipeline integration
- Confidence-threshold filtering (>70% = real detection)
- ROS publisher for detection events to the GNC controller
- OpenCV image conversion via `cv_bridge`
