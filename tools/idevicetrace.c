#include <libimobiledevice/libimobiledevice.h>

int main(int argc, char *argv[])
{
    int num = 0;
	idevice_info_t *devices = NULL;
	idevice_get_device_list_extended(&devices, &num);
	if (num == 0) {
		fprintf(stderr, "No device found.\n");
		return -1;
    }
    idevice_t * device = devices[0];
    lockdownd_client_t client = NULL;
    if (LOCKDOWN_E_SUCCESS == lockdownd_client_new_with_handshake(device, &client, "traceclient"))
    {
    	lockdownd_service_descriptor_t service = NULL;
        if(LOCKDOWN_E_SUCCESS == lockdownd_start_service(client, &service, "com.apple.os_trace_relay"))
        {
            plist_t dict = plist_new_dict();
    		plist_dict_set_item(dict, "Request", plist_new_string("PidList"));
            char * dictxml  = NULL;
            uint32_t xmllen = 0;
            plist_to_xml(dict, &dictxml, &xmllen);            
            if (S_OK == service_send(service, dictxml, xmllen))
            {
                char buffer[1024*1024];
                uint32_t received = 0;
                if (S_OK == service_receive(service, buf, 1, &received))
                {
                    received = 0;
                    service_error_t ret = service_receive(service, buf, 1024*1024, &received);
                    if (ret == OK || received > 0)
                    {
                        pritf(buf);
                    }
                }
            }
            plist_mem_free(dictxml);
            service_free(service);
        }
        lockdown_client_free(client);
    }
	idevice_device_list_extended_free(devices);
    return 0;
}