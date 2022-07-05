#pragma once

namespace sdk {
	typedef enum
	{
		NS_CLIENT = 0,
		NS_SERVER,
		NS_COUNT,
	} netsrc_t;
	typedef struct netchan_s
	{
        netsrc_t		sock;		// NS_SERVER or NS_CLIENT, depending on channel.
        netadr_t		remote_address;	// address this channel is talking to.  
        int		qport;		// qport value to write when transmitting

        int		compress;		// enable huffman compression

        double		last_received;	// for timeouts
        double		last_sent;	// for retransmits		

        double		rate;		// bandwidth choke. bytes per second
        double		cleartime;	// if realtime > cleartime, free to send next packet
        double		connect_time;	// Usage: host.realtime - netchan.connect_time

        int		drop_count;	// dropped packets, cleared each level
        int		good_count;	// cleared each level

                            // Sequencing variables
        int		incoming_sequence;		// increasing count of sequence numbers               
        int		incoming_acknowledged;	// # of last outgoing message that has been ack'd.          
        int		incoming_reliable_acknowledged;	// toggles T/F as reliable messages are received.	
        int		incoming_reliable_sequence;	// single bit, maintained local	    
        int		outgoing_sequence;		// message we are sending to remote              
        int		reliable_sequence;		// whether the message contains reliable payload, single bit
        int		last_reliable_sequence; // outgoing sequence number of last send that had reliable data
	}netchan_t;
};