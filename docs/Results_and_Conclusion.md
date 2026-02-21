# Results and Conclusion

## Results and Discussions

### Real-time 3D Mapping and Localization
Initially the virtual world is launched and the necessary connection between ROS and ArduCopter is established.

**Figure 4.1: Virtual World in Gazebo**
![Virtual World in Gazebo](../assets/figures/Figure%204.1.png)

**Figure 4.2: Integrated Simulation Setup**
![Simulation Setup](../assets/figures/Figure%204.2.svg)

---

### LiDAR-SLAM Integration
The blue laser rays from the drone shown below represent the LiDAR scanning pattern. The system maintains a safe distance of **3 meters** from all detected obstacles.

**Figure 4.3: LiDAR Integrated Drone**
![LiDAR Integrated Drone](../assets/figures/Figure%204.3.png)

---

### YOLOv8-Enabled Human Detection
The perception system distinguishes between real and false positive detections based on a **70% confidence threshold**:
- **Left (73% confidence)**: Above threshold → correctly identified as a real person → **autonomous landing triggered**
- **Right (48% confidence)**: Below threshold → identified as a false positive → **drone continues searching**

**Figure 4.4: YOLOv8 Confidence Output**
![YOLO Output](../assets/figures/Figure%204.4.svg)

---

### Performance Metrics

| Metric | Result |
|---|---|
| YOLOv8 Detection (mAP@0.5) | **91.3%** |
| Inference Speed | **60 FPS** (real-time) |
| Localization Drift Reduction | **38%** vs. single-sensor odometry |
| Obstacle Detection Range (LiDAR) | **3m** safety threshold |

---

## Conclusion and Future Work

### Conclusion
The Autonomous Disaster Response Drone Simulator successfully demonstrates that fusing geometric localization (LiDAR-SLAM) with semantic AI perception (YOLOv8) creates a significantly more robust and intelligent autonomous system. The framework achieved strong results in simulated disaster environments, validating the core technical approach as a foundation for real-world deployment.

### Future Work
- **Hardware Deployment**: Transitioning the validated algorithms to physical UAV platforms
- **Enhanced Datasets**: Training the perception model on specialized disaster-scenario datasets
- **GPU-Accelerated Inference**: Further reducing latency for even faster real-time detection
- **Higher Fidelity Worlds**: Advanced Gazebo environments with dynamic weather and multi-agent simulation
