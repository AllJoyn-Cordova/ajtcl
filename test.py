#!/usr/bin/python2
from __future__ import print_function

__author__ = 'javier'

import alljoyn

service_name = 'org.alljoyn.Bus.sample'
service_path = '/sample'
service_port = 25

sample_interfaces = alljoyn.AJ_InterfacesCreate()

sample_interface = alljoyn.AJ_InterfaceDescriptionCreate(
    'org.alljoyn.Bus.sample')
sample_interface = alljoyn.AJ_InterfaceDescriptionAdd(sample_interface,
                                                      '?Dummy foo<i')
sample_interface = alljoyn.AJ_InterfaceDescriptionAdd(sample_interface,
                                                      '?Dummy2 fee<i')
sample_interface = alljoyn.AJ_InterfaceDescriptionAdd(sample_interface,
                                                      '?cat inStr1<s inStr2<s outStr>s')
sample_interfaces = alljoyn.AJ_InterfacesAdd(sample_interfaces,
                                             sample_interface)

obj = alljoyn.AJ_Object()
obj.path = service_path
obj.interfaces = sample_interfaces

app_objects = alljoyn.AJ_ObjectsCreate()
app_objects = alljoyn.AJ_ObjectsAdd(app_objects, obj)

alljoyn.AJ_Initialize()
alljoyn.AJ_PrintXML(app_objects)
alljoyn.AJ_RegisterObjects(None,app_objects)
bus = alljoyn.AJ_BusAttachment()
status, sessionid = alljoyn.AJ_StartClient(bus,
                                   None,
                                   60 * 1000,
                                   alljoyn.FALSE,
                                   service_name,
                                   service_port,
                                   None)

msg1 = alljoyn._AJ_Message()
basic_client_cat = alljoyn.AJ_PRX_MESSAGE_ID(0,0,2)

print('Checkpoint')
status = alljoyn.AJ_MarshalMethodCall(bus, msg1, basic_client_cat,
                                      service_name, sessionid, 0, (1000))
print('Checkpoint')
alljoyn.AJ_MarshalArgs(msg1, 'ss', 'Hello ', 'World!')
print('Checkpoint')
alljoyn.AJ_DeliverMsg(msg1)

print('Checkpoint')

msg2 = alljoyn._AJ_Message()
status = alljoyn.AJ_UnmarshalMsg(bus, msg2, 5000)
if msg2.msgId == alljoyn.AJ_REPLY_ID(basic_client_cat):
    arg = alljoyn._AJ_Arg()
    alljoyn.AJ_UnmarshalArg(msg2, arg)
    print(service_name+'.cat', "(path='%s')".format(service_path), 'returned',
          "'%s'".format(arg.val.v_string))