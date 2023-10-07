from pymavlink import mavutil
from dronekit import connect, VehicleMode, LocationGlobal, LocationGlobalRelative
import random
import math
import time
import os


class DronController:
    def __init__(self, takeoff_alt, velocity):
        self.alt = takeoff_alt
        self.drone_speed = velocity
        self.image_count = 0
        self.call_count = 0
        # if os.path.exists('~/Desktop/run_images'):
        #     os.system('rm -rf ~/Desktop/run_images')
        # else:
        #     os.system('mkdir ~/Desktop/run_images')
        self.vehicle = None

    def wait_for_altituide(self):
        while True:
            print('Altitude: {0}'.format(self.vehicle.location.global_relative_frame.alt))
            if self.vehicle.location.global_relative_frame.alt >= self.alt * 0.95:
                print('REACHED TARGET ALTITUDE')
                break
            time.sleep(1)

    def send_body_ned_velocity(self, velocity_x, velocity_y, velocity_z):
        self.call_count += 1
        msg = self.vehicle.message_factory.set_position_target_local_ned_encode(
            0,  # time_boot_ms (not used)
            0, 0,  # target system, target component
            mavutil.mavlink.MAV_FRAME_BODY_NED,  # frame Needs to be MAV_FRAME_BODY_NED for forward/back left/right control.
            0b0000111111000111,  # type_mask
            0, 0, 0,  # x, y, z positions
            velocity_x, velocity_y, velocity_z,  # m/s
            0, 0, 0,  # x, y, z acceleration
            0, 0)
        self.vehicle.send_mavlink(msg)
        time.sleep(7)
        if 2 == self.call_count:
            print('INFO  Fix the altitude deviation.')
            self.wait_for_altituide()
            self.call_count = 0

    def goto_position_target_local_ned(self, x, y, down):
        msg = self.vehicle.message_factory.set_position_target_local_ned_encode(
            0,  # time_boot_ms (not used)
            0, 0,  # target system, target component
            mavutil.mavlink.MAV_FRAME_BODY_FRD,  # frame
            0b0000111111111000,  # type_mask (only positions enabled)
            x, y, down,  # x, y, z positions (or North, East, Down in the MAV_FRAME_BODY_NED frame
            0, 0, 0,  # x, y, z velocity in m/s  (not used)
            0, 0, 0,  # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
            0, 0)  # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)
        # send command to vehicle
        self.vehicle.send_mavlink(msg)
        time.sleep(7)
        if 2 == self.call_count:
            print('INFO  Fix the altitude deviation.')
            self.wait_for_altituide()
            self.call_count = 0

    def get_location_metres(self, original_location, lat, lon):
        if type(original_location) is LocationGlobal:
            targetlocation = LocationGlobal(lat, lon, original_location.alt)
        elif type(original_location) is LocationGlobalRelative:
            targetlocation = LocationGlobalRelative(lat, lon, original_location.alt)
        else:
            raise Exception("Invalid Location object passed")
        return targetlocation

    def get_distance_metres(self, aLocation1, aLocation2):
        dlat = aLocation2.lat - aLocation1.lat
        dlong = aLocation2.lon - aLocation1.lon
        return math.sqrt((dlat * dlat) + (dlong * dlong)) * 1.113195e5

    def goto_position_target_global_int(self, aLocation):
        msg = self.vehicle.message_factory.set_position_target_global_int_encode(
            0,  # time_boot_ms (not used)
            0, 0,  # target system, target component
            mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT_INT,  # frame
            0b0000111111111000,  # type_mask (only speeds enabled)
            aLocation.lat * 1e7,  # lat_int - X Position in WGS84 frame in 1e7 * meters
            aLocation.lon * 1e7,  # lon_int - Y Position in WGS84 frame in 1e7 * meters
            aLocation.alt,
            0,  # X velocity in NED frame in m/s
            0,  # Y velocity in NED frame in m/s
            0,  # Z velocity in NED frame in m/s
            0, 0, 0,  # afx, afy, afz acceleration (not supported yet, ignored in GCS_Mavlink)
            0, 0)  # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)
        # send command to vehicle
        self.vehicle.send_mavlink(msg)

    def goto(self, lon, lat):
        currentLocation = self.vehicle.location.global_relative_frame
        targetLocation = self.get_location_metres(currentLocation, lat, lon)
        targetDistance = self.get_distance_metres(currentLocation, targetLocation)
        self.vehicle.simple_goto(targetLocation)

        while self.vehicle.mode.name == "GUIDED":  # Stop action if we are no longer in guided mode.
            # print "DEBUG: mode: %s" % vehicle.mode.name
            remainingDistance = self.get_distance_metres(self.vehicle.location.global_relative_frame, targetLocation)
            print("Distance to target: ", remainingDistance)
            if remainingDistance <= targetDistance * 0.05:  # Just below target, in case of undershoot.
                print("Reached target")
                break
            time.sleep(2)

    def set_servo(self, servo_number, pwm_value):
        pwm_value_int = int(pwm_value)
        msg = self.vehicle.message_factory.command_long_encode(
            0, 0,
            mavutil.mavlink.MAV_CMD_DO_SET_SERVO,
            0,
            servo_number,
            pwm_value_int,
            0, 0, 0, 0, 0
        )
        self.vehicle.send_mavlink(msg)
        time.sleep(1)

    def startup(self):
        self.vehicle = connect('/dev/serial0', baud=57600, wait_ready=True)
        while not self.vehicle.is_armable:
            time.sleep(1)
        self.vehicle.mode = VehicleMode("GUIDED")
        self.vehicle.armed = True
        while not self.vehicle.armed:
            print('Waiting for arming...')
            time.sleep(1)
        self.vehicle.simple_takeoff(self.alt)  # Take off to target altitude
        self.wait_for_altituide()

    def shot(self, i, j):
        os.system('raspistill -o ~/Desktop/run_images/image_{0}{1}_{2}.jpg'.format(i, j, self.image_count))
        print('INFO  Photo shoting was done successfully.')

    def missinon_end(self, mode='LAND'):
        self.vehicle.mode = VehicleMode(mode)
        self.wait_for_altituide()
