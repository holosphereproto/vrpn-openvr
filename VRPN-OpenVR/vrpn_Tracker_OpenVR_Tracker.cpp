#include <iostream>
#include "vrpn_Tracker_OpenVR_Tracker.h"

vrpn_Tracker_OpenVR_Tracker::vrpn_Tracker_OpenVR_Tracker(const std::string& name, vrpn_Connection* connection, vr::IVRSystem * vr) :
    vrpn_Tracker_OpenVR(name.c_str(), connection, vr),
    vrpn_Button_Filter(name.c_str(), connection)
{
    // Initialize the vrpn_Button_Filter
    vrpn_Button_Filter::num_buttons = vr::k_EButton_Max;
    for (auto i = 0; i < vrpn_Button_Filter::num_buttons; i++) {
        vrpn_Button_Filter::buttons[i] = vrpn_Button_Filter::lastbuttons[i] = 0;
    }
}

void vrpn_Tracker_OpenVR_Tracker::mainloop() {
    vrpn_gettimeofday( &(vrpn_Tracker_OpenVR::timestamp), nullptr );
    vrpn_Tracker_OpenVR::mainloop();

    vrpn_gettimeofday( &(vrpn_Button_Filter::timestamp), nullptr );
    vrpn_Button_Filter::report_changes();
}

void vrpn_Tracker_OpenVR_Tracker::updateTracker(vr::TrackedDeviceIndex_t unTrackedDevice) {
    // Analog & Buttons
    if (vr->GetTrackedDeviceClass(unTrackedDevice) == vr::TrackedDeviceClass_GenericTracker) {
        vr->GetControllerState(unTrackedDevice, &pTrackerState, sizeof(vr::VRControllerState_t));

        for (unsigned int buttonId = 0; buttonId < vr::k_EButton_Max; ++buttonId) {
            uint64_t mask = vr::ButtonMaskFromId(static_cast<vr::EVRButtonId>(buttonId));
            vrpn_Button_Filter::buttons[buttonId] =
                static_cast<unsigned char>(((mask & pTrackerState.ulButtonTouched) == mask)
                                           ? 2
                                           : 0);
            vrpn_Button_Filter::buttons[buttonId] =
                static_cast<unsigned char>(((mask & pTrackerState.ulButtonPressed) == mask)
                                           ? 1
                                           : vrpn_Button_Filter::buttons[buttonId]);
        }
    }
}
