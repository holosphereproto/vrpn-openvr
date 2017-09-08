#pragma once

#include <string>
#include <vrpn_Analog.h>
#include <vrpn_Button.h>
#include <openvr.h>
#include "vrpn_Tracker_OpenVR.h"

class vrpn_Tracker_OpenVR_Tracker :
	public vrpn_Tracker_OpenVR,
	public vrpn_Button_Filter
{
public:
	vrpn_Tracker_OpenVR_Tracker() = delete;
	vrpn_Tracker_OpenVR_Tracker(const std::string& name, vrpn_Connection* connection, vr::IVRSystem * vr);
	void mainloop() override;
	void updateTracker(vr::TrackedDeviceIndex_t unTrackedDevice);
private:
    vr::VRControllerState_t pTrackerState;
};
