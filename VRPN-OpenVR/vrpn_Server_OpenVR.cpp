#include <iostream>
#include <string>
#include <quat.h>
#include <vrpn_Connection.h>
#include "vrpn_Server_OpenVR.h"

vrpn_Server_OpenVR::vrpn_Server_OpenVR() {
    // Initialize OpenVR
    vr::EVRInitError eError = vr::VRInitError_None;
    static int max_tries = 10;

    std::cout << "VRPN Server OpenVR" << std::endl;

    // Retry some times because the base do not always give enough samples for the init process.
    while( max_tries-- > 0) {
        std::cout << "VR Runtime Path is: " << vr::VR_RuntimePath() << std::endl;
        vr = vr::VR_Init(&eError, vr::VRApplication_Utility);
        if (eError != vr::VRInitError_None) {
            vr = nullptr;
            std::cerr << "Unable to init VR runtime: " << vr::VR_GetVRInitErrorAsEnglishDescription(eError) << std::endl;
        } else{
            // Successfully initialized OpenVR, continue
            break;
        }

        if (max_tries == 0) {
          return;
        }

        vrpn_SleepMsecs(2000);
    }

    // Initialize VRPN Connection
    std::string connectionName = ":" + std::to_string(vrpn_DEFAULT_LISTEN_PORT_NO);
    connection = vrpn_create_server_connection(connectionName.c_str());
}


vrpn_Server_OpenVR::~vrpn_Server_OpenVR() {
    vr::VR_Shutdown();
    if (connection) {
        connection->removeReference();
        connection = nullptr;
    }
}

void vrpn_Server_OpenVR::mainloop() {
    // Get Tracking Information
    vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    vr->GetDeviceToAbsoluteTrackingPose(
        vr::TrackingUniverseStanding,
        0/*float fPredictedSecondsToPhotonsFromNow*/,
        m_rTrackedDevicePose,
        vr::k_unMaxTrackedDeviceCount
    );

    for (vr::TrackedDeviceIndex_t unTrackedDevice = 0;
         unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++) {
        // Forget about disconnected devices
        if (!m_rTrackedDevicePose[unTrackedDevice].bDeviceIsConnected) {
            continue;
        }

        // Treat different device types separately
        switch (vr->GetTrackedDeviceClass(unTrackedDevice)) {
            case vr::TrackedDeviceClass_HMD: {
                vrpn_Tracker_OpenVR_HMD *hmd{nullptr};
                auto search = hmds.find(unTrackedDevice);
                if (search == hmds.end()) {
                    auto newHMD = std::make_unique<vrpn_Tracker_OpenVR_HMD>(
                        "openvr/hmd/" + std::to_string(unTrackedDevice),
                        connection,
                        vr
                    );
                    hmd = newHMD.get();
                    hmds[unTrackedDevice] = std::move(newHMD);
                } else {
                    hmd = search->second.get();
                }
                hmd->updateTracking(&m_rTrackedDevicePose[unTrackedDevice]);
                hmd->mainloop();
                break;
            }
            case vr::TrackedDeviceClass_Controller: {
                vrpn_Tracker_OpenVR_Controller *controller{ nullptr };
                auto search = controllers.find(unTrackedDevice);
                if (search == controllers.end()) {
                    std::string device_name = std::to_string(unTrackedDevice);

                    char device_id[255];
                    uint32_t len = vr->GetStringTrackedDeviceProperty(unTrackedDevice, vr::Prop_SerialNumber_String, device_id, 255);
                    if (len > 0)
                        device_name = device_id;

                    auto newController = std::make_unique<vrpn_Tracker_OpenVR_Controller>(
                        "openvr/controller/" + device_name,
                        connection,
                        vr
                    );
                    controller = newController.get();
                    controllers[unTrackedDevice] = std::move(newController);
                }
                else {
                  controller = search->second.get();
                }
                controller->updateTracking(&m_rTrackedDevicePose[unTrackedDevice]);
                controller->updateController(unTrackedDevice);
                controller->mainloop();
            }
            {
                vr::ETrackedControllerRole role = vr->GetControllerRoleForTrackedDeviceIndex(unTrackedDevice);
                if (role == vr::TrackedControllerRole_Invalid)
                    break;

                vrpn_Tracker_OpenVR_Controller *controller{ nullptr };
                auto search = hands.find(role);
                if (search == hands.end()) {
                    std::string device_name;
                    if (role == vr::TrackedControllerRole_LeftHand)
                        device_name = "left";
                    else
                        device_name = "right";

                    auto newController = std::make_unique<vrpn_Tracker_OpenVR_Controller>
                      ("openvr/controller/" + device_name,
                       connection,
                       vr);
                    controller = newController.get();
                    hands[role] = std::move(newController);
                } else {
                    controller = search->second.get();
                }
                controller->updateTracking(&m_rTrackedDevicePose[unTrackedDevice]);
                controller->updateController(unTrackedDevice);
                controller->mainloop();

                break;
            }
            case vr::TrackedDeviceClass_GenericTracker: {
                vrpn_Tracker_OpenVR_Tracker *tracker{nullptr};
                auto search = trackers.find(unTrackedDevice);
                if (search == trackers.end()) {
                    std::string device_name = std::to_string(unTrackedDevice);

                    char device_id[255];
                    uint32_t len = vr->GetStringTrackedDeviceProperty(unTrackedDevice, vr::Prop_SerialNumber_String, device_id, 255);
                    if (len > 0)
                      device_name = device_id;

                  
                    auto newTracker = std::make_unique<vrpn_Tracker_OpenVR_Tracker>(
                        "trackerdata",
                        //"openvr/tracker/" + device_name,
                        connection,
                        vr
                    );
                    tracker = newTracker.get();
                    trackers[unTrackedDevice] = std::move(newTracker);
                } else {
                    tracker = search->second.get();
                }
                tracker->updateTracking(&m_rTrackedDevicePose[unTrackedDevice]);
                tracker->updateTracker(unTrackedDevice);
                tracker->mainloop();
                break;
            }
            default: {
                break;
            }
        }
    }

    // Send and receive all messages.
    connection->mainloop();

    // Bail if the connection is in trouble.
    if (!connection->doing_okay()) {
        std::cerr << "Connection is not doing ok. Should we bail?" << std::endl;
    }
}

