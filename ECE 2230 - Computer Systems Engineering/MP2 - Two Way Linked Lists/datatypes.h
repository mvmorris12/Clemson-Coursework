/* datatypes.h 
 * Michael Morris
 * Mvmorri
 * ECE 2230 Fall 2023
 * MP2
 *
 * Propose: The data type that is stored in the list ADT is defined here.  We
 * define a single mapping that allows the list ADT to be defined in terms of a
 * generic mydata_t.
 *
 * Assumptions: This data should not altered
 *
 * mydata_t: The type of data that we want to store in the list
 *
 * Bugs: None..
 */

typedef struct ap_info_tag {
    int mobile_count;	// Number mobles associated with this AP 
    int eth_address;	// AP's Ethernet address 
    int ip_address;	// AP's IP address 
    int location_code;	// AP's location code 
    int authenticated;	// true or false 
    int privacy;	// mode 0 for none, 1 for WEP, 2 for WPA, 3 for WPA2
    int standard_letter;// a, b, e, g, h, n, or s. 
                        //   Convert letter to integer with a=1, b=2, etc. 
    float band;		// 2.4 or 5.0 for the ISM frequency bands (in GHz) 
    int channel;	// 1-11 for 2.4 GHz and 1-24 for 5 GHz 
    float data_rate;	// 1, 2, 5.5, 11 for 2.4 GHz and 
                        // 6, 9, 12, 18, 24, 36, 48, 54 for 5 GHz 
    int time_received;	// time in seconds that information last updated
} ap_info_t;

/* the list ADT works on packet data of this type */
typedef ap_info_t mydata_t;

/* commands for vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
