# Project Introduction

## Overview
Unmanned Aerial Vehicles (UAVs), often known as drones, have experienced significant advancements in recent years, establishing themselves as a vital part of contemporary technological frameworks. From environmental monitoring and agricultural oversight to logistics, search-and-rescue operations, and defense uses, UAVs are revolutionizing the interaction of autonomous systems with the physical environment.

Attaining genuine autonomy necessitates overcoming several obstacles — most prominently accurate localization, real-time perception, and adaptive decision-making in uncertain or dynamic settings.

Traditional UAV navigation predominantly depends on GPS for localization. However, GPS can become unreliable or entirely unavailable in GPS-denied settings, such as indoors, in densely populated urban areas, or beneath canopy cover. To mitigate this, researchers have investigated Visual Odometry (VO) and LiDAR Odometry (LO) techniques.

The integration of YOLO with LOFF presents a promising avenue for perception-driven odometry, where semantic understanding enhances geometric localization. This combination allows UAVs to dynamically improve trajectory decisions — for instance, when YOLO detects obstacles, the data directly impacts LOFF trajectory calculations for safer and more efficient navigation.

---

## Objectives
- **Develop a Unified Simulation Platform**: A single, modular simulation environment (ROS 2 / Gazebo / ArduPilot SITL) for development and testing without physical hardware.
- **Achieve Robust Localization**: Implement sensor fusion (LiDAR-SLAM + Optical Flow via Factor Graph Optimization) for stable, high-accuracy, drift-reduced odometry in GPS-denied scenarios.
- **Integrate Real-Time Semantic Perception**: Incorporate YOLOv8 for real-time detection of people, vehicles, and debris at high frame rates.
- **Enable Perception-Driven Navigation**: Use semantic data from YOLOv8 to make intelligent flight decisions (obstacle avoidance, confidence-based autonomous landing).
- **Validate System Robustness**: Test in disaster-oriented simulated scenarios to prove efficiency, scalability, and safe navigation under challenging, dynamic conditions.

---

## Background and Literature Survey
The advancement of autonomous aerial robotics has progressed rapidly due to improvements in localization, perception, and multimodal sensor fusion. Conventional GPS-based navigation is unreliable in indoor or densely populated areas, which led early research to rely on Visual Odometry (VO). Although efficient, VO is hindered by lighting variations and motion blur. This redirected attention towards LiDAR-based methods such as LOAM (Zhang & Singh), which offers low-drift geometric estimation.

Multimodal fusion techniques like **LOFF** merge the precision of LiDAR with dense optical flow to ensure robustness in compromised visual conditions. These systems employ Factor Graph Optimization (FGO) with solvers like g2o and GTSAM. Simultaneously, deep learning perception has advanced through the **YOLO** series, offering single-shot inference for real-time UAV operations.
