/*
 * interface-management.c
 *
 *  Created on: Oct 22, 2014
 *      Author: javier
 */

#include <alljoyn.h>

int main(int argc, char **argv)
{
    AJ_InterfaceDescription *array;
    char **data;
    char ***iter;
    int x, y;

    array = AJ_InterfacesCreate();

    data = AJ_InterfaceDescriptionCreate("org.alljoyn.Bus.sample");
    if (!data)
        return 1;
    data = AJ_InterfaceDescriptionAdd(data, "?Dummy foo<i");
    data = AJ_InterfaceDescriptionAdd(data, "?Dummy2 fee<i");
    data = AJ_InterfaceDescriptionAdd(data, "?cat inStr1<s inStr2<s outStr>s");
    array = AJ_InterfacesAdd(array, data);

    data = AJ_InterfaceDescriptionCreate("org.alljoyn.Bus");
    if (!data)
        return 1;
    data = AJ_InterfaceDescriptionAdd(data, "!SessionLost >u");
    data = AJ_InterfaceDescriptionAdd(data, "!FoundAdvertisedName >s >q >s");
    data = AJ_InterfaceDescriptionAdd(data, "!LostAdvertisedName >s >q >s");
    data = AJ_InterfaceDescriptionAdd(data, "!MPSessionChanged >u >s >b");
    data = AJ_InterfaceDescriptionAdd(data, "?AdvertiseName <s <q >u");
    data = AJ_InterfaceDescriptionAdd(data, "?CancelAdvertiseName <s <q >u");
    data = AJ_InterfaceDescriptionAdd(data, "?FindAdvertisedName <s >u");
    data = AJ_InterfaceDescriptionAdd(data, "?CancelFindAdvertisedName <s");
    data = AJ_InterfaceDescriptionAdd(data, "?BindSessionPort <q <a{sv} >u >q");
    data = AJ_InterfaceDescriptionAdd(data, "?UnbindSessionPort <q >u");
    data = AJ_InterfaceDescriptionAdd(data, "?JoinSession <s <q <a{sv} >u >u >a{sv}");
    data = AJ_InterfaceDescriptionAdd(data, "?LeaveSession <u >u");
    data = AJ_InterfaceDescriptionAdd(data, "?CancelSessionlessMessage <u >u");
    data = AJ_InterfaceDescriptionAdd(data, "?FindAdvertisedNameByTransport <s <q >u");
    data = AJ_InterfaceDescriptionAdd(data, "?CancelFindAdvertisedNameByTransport <s <q >u");
    data = AJ_InterfaceDescriptionAdd(data, "?SetLinkTimeout <u <u >u >u");
    data = AJ_InterfaceDescriptionAdd(data, "?RemoveSessionMember <us >u");
    data = AJ_InterfaceDescriptionAdd(data, "!SessionLostWithReason >u >u");
    data = AJ_InterfaceDescriptionAdd(data, "?Ping <s <u >u");
    array = AJ_InterfacesAdd(array, data);

    data = AJ_InterfaceDescriptionCreate("org.freedesktop.DBus");
    data = AJ_InterfaceDescriptionAdd(data, "?Hello >s");
    data = AJ_InterfaceDescriptionAdd(data, "!NameOwnerChanged >s >s >s");
    data = AJ_InterfaceDescriptionAdd(data, "!NameAcquired >s");
    data = AJ_InterfaceDescriptionAdd(data, "!NameLost >s");
    data = AJ_InterfaceDescriptionAdd(data, "!PropertiesChanged >s >a{sv} >as");
    data = AJ_InterfaceDescriptionAdd(data, "?RequestName <s <u >u");
    data = AJ_InterfaceDescriptionAdd(data, "?AddMatch <s");
    data = AJ_InterfaceDescriptionAdd(data, "?RemoveMatch <s");
    data = AJ_InterfaceDescriptionAdd(data, "?ReleaseName <s >u");
    array = AJ_InterfacesAdd(array, data);

    data = AJ_InterfaceDescriptionCreate("#org.freedesktop.DBus.Properties");
    data = AJ_InterfaceDescriptionAdd(data, "?Get <s <s >v");
    data = AJ_InterfaceDescriptionAdd(data, "?Set <s <s <v");
    data = AJ_InterfaceDescriptionAdd(data, "?GetAll <s >a{sv}");
    array = AJ_InterfacesAdd(array, data);

    data = AJ_InterfaceDescriptionCreate("org.alljoyn.Daemon");
    data = AJ_InterfaceDescriptionAdd(data, "!ProbeReq");
    data = AJ_InterfaceDescriptionAdd(data, "!ProbeAck");
    array = AJ_InterfacesAdd(array, data);

    for (iter = (char ***) array, x = 0; array && iter[x]; x++) {
        for (y = 0; iter[x][y]; y++) {
            if (y)
                printf("\t");
            printf("%s\n", iter[x][y]);
        }

    }

    AJ_InterfacesDelete(array);

    printf("Deleting stuff\n");

    for (iter = (char ***) array, x = 0; array && iter[x]; x++) {
        for (y = 0; iter[x][y]; y++) {
            if (y)
                printf("\t");
            printf("%s\n", iter[x][y]);
        }
    }

    return 0;
}
