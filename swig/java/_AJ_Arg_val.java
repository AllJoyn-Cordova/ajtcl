/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package alljoyn;

public class _AJ_Arg_val {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected _AJ_Arg_val(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(_AJ_Arg_val obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        alljoynJNI.delete__AJ_Arg_val(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setV_byte(SWIGTYPE_p_uint8_t value) {
    alljoynJNI._AJ_Arg_val_v_byte_set(swigCPtr, this, SWIGTYPE_p_uint8_t.getCPtr(value));
  }

  public SWIGTYPE_p_uint8_t getV_byte() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_byte_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_uint8_t(cPtr, false);
  }

  public void setV_int16(SWIGTYPE_p_int16_t value) {
    alljoynJNI._AJ_Arg_val_v_int16_set(swigCPtr, this, SWIGTYPE_p_int16_t.getCPtr(value));
  }

  public SWIGTYPE_p_int16_t getV_int16() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_int16_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_int16_t(cPtr, false);
  }

  public void setV_uint16(SWIGTYPE_p_uint16_t value) {
    alljoynJNI._AJ_Arg_val_v_uint16_set(swigCPtr, this, SWIGTYPE_p_uint16_t.getCPtr(value));
  }

  public SWIGTYPE_p_uint16_t getV_uint16() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_uint16_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_uint16_t(cPtr, false);
  }

  public void setV_bool(SWIGTYPE_p_uint32_t value) {
    alljoynJNI._AJ_Arg_val_v_bool_set(swigCPtr, this, SWIGTYPE_p_uint32_t.getCPtr(value));
  }

  public SWIGTYPE_p_uint32_t getV_bool() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_bool_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_uint32_t(cPtr, false);
  }

  public void setV_uint32(SWIGTYPE_p_uint32_t value) {
    alljoynJNI._AJ_Arg_val_v_uint32_set(swigCPtr, this, SWIGTYPE_p_uint32_t.getCPtr(value));
  }

  public SWIGTYPE_p_uint32_t getV_uint32() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_uint32_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_uint32_t(cPtr, false);
  }

  public void setV_int32(SWIGTYPE_p_int32_t value) {
    alljoynJNI._AJ_Arg_val_v_int32_set(swigCPtr, this, SWIGTYPE_p_int32_t.getCPtr(value));
  }

  public SWIGTYPE_p_int32_t getV_int32() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_int32_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_int32_t(cPtr, false);
  }

  public void setV_int64(SWIGTYPE_p_int64_t value) {
    alljoynJNI._AJ_Arg_val_v_int64_set(swigCPtr, this, SWIGTYPE_p_int64_t.getCPtr(value));
  }

  public SWIGTYPE_p_int64_t getV_int64() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_int64_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_int64_t(cPtr, false);
  }

  public void setV_uint64(SWIGTYPE_p_uint64_t value) {
    alljoynJNI._AJ_Arg_val_v_uint64_set(swigCPtr, this, SWIGTYPE_p_uint64_t.getCPtr(value));
  }

  public SWIGTYPE_p_uint64_t getV_uint64() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_uint64_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_uint64_t(cPtr, false);
  }

  public void setV_double(SWIGTYPE_p_double value) {
    alljoynJNI._AJ_Arg_val_v_double_set(swigCPtr, this, SWIGTYPE_p_double.getCPtr(value));
  }

  public SWIGTYPE_p_double getV_double() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_double_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_double(cPtr, false);
  }

  public void setV_string(String value) {
    alljoynJNI._AJ_Arg_val_v_string_set(swigCPtr, this, value);
  }

  public String getV_string() {
    return alljoynJNI._AJ_Arg_val_v_string_get(swigCPtr, this);
  }

  public void setV_objPath(String value) {
    alljoynJNI._AJ_Arg_val_v_objPath_set(swigCPtr, this, value);
  }

  public String getV_objPath() {
    return alljoynJNI._AJ_Arg_val_v_objPath_get(swigCPtr, this);
  }

  public void setV_signature(String value) {
    alljoynJNI._AJ_Arg_val_v_signature_set(swigCPtr, this, value);
  }

  public String getV_signature() {
    return alljoynJNI._AJ_Arg_val_v_signature_get(swigCPtr, this);
  }

  public void setV_data(SWIGTYPE_p_void value) {
    alljoynJNI._AJ_Arg_val_v_data_set(swigCPtr, this, SWIGTYPE_p_void.getCPtr(value));
  }

  public SWIGTYPE_p_void getV_data() {
    long cPtr = alljoynJNI._AJ_Arg_val_v_data_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_void(cPtr, false);
  }

  public _AJ_Arg_val() {
    this(alljoynJNI.new__AJ_Arg_val(), true);
  }

}
