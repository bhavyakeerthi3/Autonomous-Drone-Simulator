#include <ros/ros.h>
#include <darknet_ros_msgs/BoundingBoxes.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/TwistStamped.h> // Needed for sending velocity commands
#include <gnc_functions.hpp> // Assumed to contain init_publisher_subscriber, takeoff, set_destination, etc.

// --- GLOBAL STATE AND VARIABLES ---
// mode_g denotes the flight opperations
// 0 - Search (Waypoint Following)
// 1 - Rescue (Landing/Mission End)
// 2 - Avoidance (LiDAR triggered)
int mode_g = 0; 
sensor_msgs::LaserScan latest_lidar_data;

// ROS Publisher for direct velocity control (used for avoidance)
ros::Publisher velocity_pub;

// --- UTILITY FUNCTIONS FOR AVOIDANCE ---

/**
 * @brief Publishes a velocity command using geometry_msgs/TwistStamped.
 */
void set_vel_global_frame(float vx, float vy, float vz)
{
    geometry_msgs::TwistStamped velocity_cmd;
    velocity_cmd.header.stamp = ros::Time::now();
    velocity_cmd.twist.linear.x = vx;
    velocity_cmd.twist.linear.y = vy;
    velocity_cmd.twist.linear.z = vz;
    velocity_pub.publish(velocity_cmd);
}

/**
 * @brief Calculates an avoidance velocity based on the closest detected obstacle.
 */
void calculate_avoidance_velocity(float& avoidance_vel_x, float& avoidance_vel_y)
{
    avoidance_vel_x = 0.0;
    avoidance_vel_y = 0.0;

    // Parameters
    const float OBSTACLE_THRESHOLD = 3.0; // Distance (meters) to start avoiding
    const float SAFE_DISTANCE = 1.5;      // Closest safe approach distance
    const float MAX_AVOID_SPEED = 0.5;    // Max speed for avoidance (m/s)

    if (latest_lidar_data.ranges.empty())
    {
        return; // No data to process
    }

    // Find the minimum distance and its angle (index)
    float min_dist = OBSTACLE_THRESHOLD + 1.0;
    int min_idx = -1;

    for (size_t i = 0; i < latest_lidar_data.ranges.size(); ++i)
    {
        float range = latest_lidar_data.ranges[i];
        if (std::isfinite(range) && range < min_dist)
        {
            min_dist = range;
            min_idx = i;
        }
    }

    // If an obstacle is detected within the threshold
    if (min_dist < OBSTACLE_THRESHOLD)
    {
        // Set mode to Avoidance
        mode_g = 2; 

        float angle_increment = latest_lidar_data.angle_increment;
        float angle_min = latest_lidar_data.angle_min;
        float obstacle_angle = angle_min + min_idx * angle_increment;

        // Calculate the repulsive force magnitude
        float repulsion_magnitude = 0.0;
        if (min_dist < SAFE_DISTANCE)
        {
            repulsion_magnitude = MAX_AVOID_SPEED;
        }
        else if (min_dist < OBSTACLE_THRESHOLD)
        {
            repulsion_magnitude = MAX_AVOID_SPEED * (OBSTACLE_THRESHOLD - min_dist) / (OBSTACLE_THRESHOLD - SAFE_DISTANCE);
        }

        // Avoidance direction is 180 degrees away from the obstacle
        float avoidance_angle = obstacle_angle + M_PI; 

        // Convert polar repulsion force to Cartesian velocity components
        avoidance_vel_x = cos(avoidance_angle) * repulsion_magnitude;
        avoidance_vel_y = sin(avoidance_angle) * repulsion_magnitude;

        ROS_INFO_STREAM("AVOIDANCE: Obstacle at " << min_dist << "m. Repulsion: (" << avoidance_vel_x << ", " << avoidance_vel_y << ")");
    }
    else if (mode_g == 2)
    {
        // If avoidance was active, but obstacle is cleared, switch back to search
        mode_g = 0;
        ROS_INFO("AVOIDANCE: Obstacle cleared, resuming search.");
    }
}

// --- CALLBACK FUNCTIONS ---

void detection_cb(const darknet_ros_msgs::BoundingBoxes::ConstPtr& msg)
{
    for( int i=0; i < msg->bounding_boxes.size(); i++)
    {
        ROS_INFO("%s detected", msg->bounding_boxes[i].Class.c_str());  
        if(msg->bounding_boxes[i].Class == "person" && mode_g != 1)
        {
            // Rescue takes precedence over search/avoidance
            mode_g = 1;
            ROS_INFO("Person found. Starting Rescue Operation (Mode 1)");
            return;
        }
    }   
}

void lidar_cb(const sensor_msgs::LaserScan::ConstPtr& msg)
{
    latest_lidar_data = *msg;
    // We process the LiDAR data in the main loop for avoidance logic
}


int main(int argc, char **argv)
{
    //initialize ros
    ros::init(argc, argv, "optimal_search_gnc_node");
    ros::NodeHandle n;

    // ROS Subscribers
    ros::Subscriber yolo_sub = n.subscribe("/darknet_ros/bounding_boxes", 1, detection_cb);
    ros::Subscriber lidar_sub = n.subscribe("/spur/laser/scan", 10, lidar_cb);

    // Initialize the high-level GNC publisher/subscribers
    init_publisher_subscriber(n);

    // Initialize the avoidance velocity publisher
    velocity_pub = n.advertise<geometry_msgs::TwistStamped>("/mavros/setpoint_velocity/cmd_vel", 10);

    // wait for FCU connection
    wait4connect();

    //wait for used to switch to mode GUIDED
    wait4start();

    //create local reference frame
    initialize_local_frame();

    //request takeoff to 10m
    takeoff(10);

    // specify the lawnmower search pattern
    std::vector<gnc_api_waypoint> waypointList;
    gnc_api_waypoint nextWayPoint;
    float range = 50.0;
    float spacing = 10.0;
    
    // Generate the search pattern (Lawnmower)
    for(int i=0; i<5; i++)
    {
        float current_x = i * spacing;

        // 1. Move to start of row (y=0) at constant x
        nextWayPoint.x = current_x;
        nextWayPoint.y = 0;
        nextWayPoint.z = 10;
        nextWayPoint.psi = 0;
        waypointList.push_back(nextWayPoint);   
        
        // 2. Fly across the range
        nextWayPoint.x = current_x;
        nextWayPoint.y = range;
        nextWayPoint.z = 10;
        nextWayPoint.psi = 0;
        waypointList.push_back(nextWayPoint);   
        
        // 3. Move across to the next lane
        nextWayPoint.x = (i+1) * spacing;
        nextWayPoint.y = range;
        nextWayPoint.z = 10;
        nextWayPoint.psi = 0;
        waypointList.push_back(nextWayPoint);

        // 4. Fly back to the start side
        nextWayPoint.x = (i+1) * spacing;
        nextWayPoint.y = 0;
        nextWayPoint.z = 10;
        nextWayPoint.psi = 0;
        waypointList.push_back(nextWayPoint);   
    }
    
    //specify control loop rate. 
    ros::Rate rate(10.0); // Increased rate to 10Hz for smoother avoidance/control
    int counter = 0;
    float current_altitude = 10.0; // Target altitude

    // Command the first waypoint
    if (waypointList.size() > 0)
    {
        set_destination(waypointList[counter].x, waypointList[counter].y, waypointList[counter].z, waypointList[counter].psi);
        counter++;
    }


    while(ros::ok())
    {
        // --- 1. AVOIDANCE CHECK (Highest Priority) ---
        float avoid_vx, avoid_vy;
        calculate_avoidance_velocity(avoid_vx, avoid_vy); // This function sets mode_g=2 if avoidance is needed

        if(mode_g == 2)
        {
            // Send velocity command to override navigation
            set_vel_global_frame(avoid_vx, avoid_vy, 0.0); // 0.0 for Z to maintain altitude
        }
        // --- 2. RESCUE CHECK (High Priority) ---
        else if(mode_g == 1)
        {
            land();
            ROS_INFO("RESCUE: Person found. Landing Started (Mode 1)");
            break;
        }
        // --- 3. SEARCH (Default Mode) ---
        else // mode_g == 0 (Search)
        {
            if(check_waypoint_reached(.3) == 1)
            {
                if (counter < waypointList.size())
                {
                    // Command next waypoint
                    set_destination(waypointList[counter].x,waypointList[counter].y,waypointList[counter].z, waypointList[counter].psi);
                    counter++;  
                } else {
                    // Land after all waypoints are reached
                    land();
                    ROS_INFO("SEARCH: All waypoints complete. Landing.");
                    break;
                }   
            }
            // Keep sending the current waypoint command for robust control
            else if (counter > 0)
            {
                int current_wp_idx = counter - 1;
                set_destination(waypointList[current_wp_idx].x, waypointList[current_wp_idx].y, waypointList[current_wp_idx].z, waypointList[current_wp_idx].psi);
            }
        }
        
        // Process sensor data and update state
        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}
