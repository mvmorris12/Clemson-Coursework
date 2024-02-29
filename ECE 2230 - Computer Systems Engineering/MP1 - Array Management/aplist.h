/* aplist.h 
 * Michael Morris
 * mvmorri 
 * ECE 2230-001 Fall 2023 
 * MP1
 *
 * Purpose: A template for MP1 to manage WiFi Access Points
 *
 * Assumptions: structure definitions and public functions as defined for
 *      assignmnet.  
 *
 * Bugs: None
 */

struct ap_list_t {
    int apl_array_size;     // size of array given by user
    int apl_entries;        // current number of records in access point list
    struct ap_info_t **apl_ptr;
};

struct ap_info_t {
    int mobile_count;	    // Number mobles associated with this AP 
    int eth_address;	    // AP's Ethernet address 
    int ip_address;	        // AP's IP address 
    int location_code;	    // AP's location code 
    int authenticated;	    // true or false 
    int privacy;	        // mode 0 for none, 1 for WEP, 2 for WPA, 3 for WPA2
    int standard_letter;    // a, b, e, g, h, n, or s. 
                            //   Convert letter to integer with a=1, b=2, etc. 
    float band;		        // 2.4 or 5.0 for the ISM frequency bands (in GHz) 
    int channel;	        // 1-11 for 2.4 GHz and 1-24 for 5 GHz 
    float data_rate;	    // 1, 2, 5.5, 11 for 2.4 GHz and 
                            // 6, 9, 12, 18, 24, 36, 48, 54 for 5 GHz 
    int time_received;	    // time in seconds that information last updated
};

//  The sequential list ADT must have the following interface:

struct ap_list_t *aplist_construct(int);
void aplist_destruct(struct ap_list_t *);
int aplist_add(struct ap_list_t *, struct ap_info_t *);
struct ap_info_t *aplist_find(struct ap_list_t *, int);
struct ap_info_t *aplist_access(struct ap_list_t *, int);
struct ap_info_t *aplist_remove(struct ap_list_t *, int);
int aplist_inc(struct ap_list_t *, int);
int aplist_dec(struct ap_list_t *, int);
int aplist_number_entries(struct ap_list_t *);


