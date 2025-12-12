// Auto-generated. Do not edit!

// (in-package sentry_chassis_controller.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class WheelState {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.steering_angle = null;
      this.wheel_speed = null;
      this.wheel_velocity = null;
      this.wheel_torque = null;
      this.wheel_current = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('steering_angle')) {
        this.steering_angle = initObj.steering_angle
      }
      else {
        this.steering_angle = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('wheel_speed')) {
        this.wheel_speed = initObj.wheel_speed
      }
      else {
        this.wheel_speed = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('wheel_velocity')) {
        this.wheel_velocity = initObj.wheel_velocity
      }
      else {
        this.wheel_velocity = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('wheel_torque')) {
        this.wheel_torque = initObj.wheel_torque
      }
      else {
        this.wheel_torque = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('wheel_current')) {
        this.wheel_current = initObj.wheel_current
      }
      else {
        this.wheel_current = new Array(4).fill(0);
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type WheelState
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Check that the constant length array field [steering_angle] has the right length
    if (obj.steering_angle.length !== 4) {
      throw new Error('Unable to serialize array field steering_angle - length must be 4')
    }
    // Serialize message field [steering_angle]
    bufferOffset = _arraySerializer.float64(obj.steering_angle, buffer, bufferOffset, 4);
    // Check that the constant length array field [wheel_speed] has the right length
    if (obj.wheel_speed.length !== 4) {
      throw new Error('Unable to serialize array field wheel_speed - length must be 4')
    }
    // Serialize message field [wheel_speed]
    bufferOffset = _arraySerializer.float64(obj.wheel_speed, buffer, bufferOffset, 4);
    // Check that the constant length array field [wheel_velocity] has the right length
    if (obj.wheel_velocity.length !== 4) {
      throw new Error('Unable to serialize array field wheel_velocity - length must be 4')
    }
    // Serialize message field [wheel_velocity]
    bufferOffset = _arraySerializer.float64(obj.wheel_velocity, buffer, bufferOffset, 4);
    // Check that the constant length array field [wheel_torque] has the right length
    if (obj.wheel_torque.length !== 4) {
      throw new Error('Unable to serialize array field wheel_torque - length must be 4')
    }
    // Serialize message field [wheel_torque]
    bufferOffset = _arraySerializer.float64(obj.wheel_torque, buffer, bufferOffset, 4);
    // Check that the constant length array field [wheel_current] has the right length
    if (obj.wheel_current.length !== 4) {
      throw new Error('Unable to serialize array field wheel_current - length must be 4')
    }
    // Serialize message field [wheel_current]
    bufferOffset = _arraySerializer.float64(obj.wheel_current, buffer, bufferOffset, 4);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type WheelState
    let len;
    let data = new WheelState(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [steering_angle]
    data.steering_angle = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [wheel_speed]
    data.wheel_speed = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [wheel_velocity]
    data.wheel_velocity = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [wheel_torque]
    data.wheel_torque = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [wheel_current]
    data.wheel_current = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    return length + 160;
  }

  static datatype() {
    // Returns string type for a message object
    return 'sentry_chassis_controller/WheelState';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '1968715a3f1131762c929bdab65e2993';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    
    std_msgs/Header header
    float64[4] steering_angle   # 四个轮子的转向角度（弧度）
    float64[4] wheel_speed      # 四个轮子的转速(rad/s)
    float64[4] wheel_velocity   # 四个轮子的线速度(m/s)
    float64[4] wheel_torque     # 四个轮子的输出扭矩(Nm)
    float64[4] wheel_current    # 四个轮子的电流(A)
    
    ================================================================================
    MSG: std_msgs/Header
    # Standard metadata for higher-level stamped data types.
    # This is generally used to communicate timestamped data 
    # in a particular coordinate frame.
    # 
    # sequence ID: consecutively increasing ID 
    uint32 seq
    #Two-integer timestamp that is expressed as:
    # * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
    # * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
    # time-handling sugar is provided by the client library
    time stamp
    #Frame this data is associated with
    string frame_id
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new WheelState(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.steering_angle !== undefined) {
      resolved.steering_angle = msg.steering_angle;
    }
    else {
      resolved.steering_angle = new Array(4).fill(0)
    }

    if (msg.wheel_speed !== undefined) {
      resolved.wheel_speed = msg.wheel_speed;
    }
    else {
      resolved.wheel_speed = new Array(4).fill(0)
    }

    if (msg.wheel_velocity !== undefined) {
      resolved.wheel_velocity = msg.wheel_velocity;
    }
    else {
      resolved.wheel_velocity = new Array(4).fill(0)
    }

    if (msg.wheel_torque !== undefined) {
      resolved.wheel_torque = msg.wheel_torque;
    }
    else {
      resolved.wheel_torque = new Array(4).fill(0)
    }

    if (msg.wheel_current !== undefined) {
      resolved.wheel_current = msg.wheel_current;
    }
    else {
      resolved.wheel_current = new Array(4).fill(0)
    }

    return resolved;
    }
};

module.exports = WheelState;
