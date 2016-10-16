// File:    zbr.h
// Mode:    C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t

// Copyright (c) 2003-2004 Samsung Advanced Institute of Technology and
// The City University of New York. All rights reserved.

// ZigBee Routing (ZBR) is based on ZBR and Cluster-Tree.
// The ZBR part presented here is from Carnegie Mellon University's ns2
// ZBR simulation code (see the following copyright notice)


/*
Copyright (c) 1997, 1998 Carnegie Mellon University.  All Rights
Reserved. 

Permission to use, copy, modify, and distribute this
software and its documentation is hereby granted (including for
commercial or for-profit use), provided that both the copyright notice and this permission notice appear in all copies of the software, derivative works, or modified versions, and any portions thereof, and that both notices appear in supporting documentation, and that credit is given to Carnegie Mellon University in all publications reporting on direct or indirect use of this code or its derivatives.

ALL CODE, SOFTWARE, PROTOCOLS, AND ARCHITECTURES DEVELOPED BY THE CMU
MONARCH PROJECT ARE EXPERIMENTAL AND ARE KNOWN TO HAVE BUGS, SOME OF
WHICH MAY HAVE SERIOUS CONSEQUENCES. CARNEGIE MELLON PROVIDES THIS
SOFTWARE OR OTHER INTELLECTUAL PROPERTY IN ITS ``AS IS'' CONDITION,
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR
INTELLECTUAL PROPERTY, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

Carnegie Mellon encourages (but does not require) users of this
software or intellectual property to return any improvements or
extensions that they make, and to grant Carnegie Mellon the rights to redistribute these changes without encumbrance.

The ZBR code developed by the CMU/MONARCH group was optimized and tuned by Samir Das and Mahesh Marina, University of Cincinnati. The work was partially done in Sun Microsystems.

*/

#ifndef __zbr_h__
#define __zbr_h__

#include <cmu-trace.h>
#include <priqueue.h>
#include <zbr/zbr_rtable.h>
#include <zbr/zbr_rqueue.h>
#include <classifier/classifier-port.h>

// Allows local repair of routes 
#define ZBR_LOCAL_REPAIR

// Allows ZBR to use link-layer feedback in determining when links are up/down.
#define ZBR_LINK_LAYER_DETECTION

/*
  Causes ZBR to apply a "smoothing" function to the link layer feedback
  that is generated by 802.11/802.15.4.  In essence, it requires that RT_MAX_ERROR
  errors occurs within a window of RT_MAX_ERROR_TIME before the link
  is considered bad.
*/
#define ZBR_USE_LL_METRIC

/*
  Only applies if ABR_USE_LL_METRIC is defined.
  Causes ZBR to apply omniscient knowledge to the feedback received
  from 802.11/802.15.4.  This may be flawed, because it does not account for
  congestion.
*/
//#define ZBR_USE_GOD_FEEDBACK


class ZBR;

#define ZBR_MY_ROUTE_TIMEOUT        10                  // 100 seconds
#define ZBR_ACTIVE_ROUTE_TIMEOUT    10			// 50 seconds
#define ZBR_REV_ROUTE_LIFE          6			// 5  seconds
#define ZBR_BCAST_ID_SAVE           6			// 3 seconds


// No. of times to do network-wide search before timing out for 
// MAX_RREQ_TIMEOUT sec. 
#define ZBR_RREQ_RETRIES            3
// timeout after doing network-wide search RREQ_RETRIES times
#define ZBR_MAX_RREQ_TIMEOUT	10.0 			//sec

/* Various constants used for the expanding ring search */
#define ZBR_TTL_START     5
#define ZBR_TTL_THRESHOLD 7
#define ZBR_TTL_INCREMENT 2 

// This should be somewhat related to arp timeout
#define ZBR_NODE_TRAVERSAL_TIME     0.03		// 30 ms
#define ZBR_LOCAL_REPAIR_WAIT_TIME  0.15 		//sec

// Should be set by the user using best guess (conservative) 
#define ZBR_NETWORK_DIAMETER        30			// 30 hops

// Must be larger than the time difference between a node propagates a route 
// request and gets the route reply back.

//#define ZBR_RREP_WAIT_TIME     (3 * NODE_TRAVERSAL_TIME * NETWORK_DIAMETER)	// ms
//#define ZBR_RREP_WAIT_TIME     (2 * REV_ROUTE_LIFE)	// seconds
#define ZBR_RREP_WAIT_TIME         1.0			// seconds

#define ZBR_ID_NOT_FOUND    	0x00
#define ZBR_ID_FOUND        	0x01
//#define ZBR_INFINITY        	0xff

// The followings are used for the forward() function. Controls pacing.
#define ZBR_DELAY 1.0           // random delay
#define ZBR_NO_DELAY -1.0       // no delay 

// think it should be 30 ms
#define ZBR_ARP_DELAY 0.01      // fixed delay to keep arp happy

#define ZBR_HELLO_INTERVAL          1               // 1000 ms
#define ZBR_ALLOWED_HELLO_LOSS      3               // packets

//FIX: added by Yong on 11/13/03 for ZigBee routing
#define ZBR_FAIL_THRESHOLD	    3
//FIX END 	

// Zhu: the following are definitions for cluster tree

// Relationship constants 
const unsigned char PARENT	=		0;			// Relationship field setting marking a parent
const unsigned char CHILD	=		2;			// Relationship field setting marking a child
const unsigned char NEIGHBOR	=		3;			// Relationship field setting marking a neighbor 
const unsigned char UNSTABLE	=		1;			// Relationship field setting marking a neighbor 

// Address Type Definitions  
const int CID_NID	=		0;
const int IEEE		=		1;
const int UNUSED	=		2;

// cluster tree parameters
typedef struct
{
    unsigned int	BlockSize;
    unsigned char	Cm;
    unsigned char	Lm;
    unsigned char	defaultFreq;
    unsigned int	ticksPerFrame;
    unsigned int	framesPerUpdate;
    unsigned char	nameAddrPeriod;
    unsigned char	lowerRSSI;
    unsigned char	middleRSSI;
    unsigned char	upperRSSI;
} NET_SYSTEM_CONFIG;

/*
  Timers (Broadcast ID, Hello, Neighbor Cache, Route Cache)
*/
class ZBR_BroadcastTimer : public Handler {
public:
        ZBR_BroadcastTimer(ZBR* a) : agent(a) {}
        void	handle(Event*);
private:
        ZBR    *agent;
	Event	intr;
};

class ZBR_NeighborTimer : public Handler {
public:
        ZBR_NeighborTimer(ZBR* a) : agent(a) {}
        void	handle(Event*);
private:
        ZBR    *agent;
	Event	intr;
};

class ZBR_RouteCacheTimer : public Handler {
public:
        ZBR_RouteCacheTimer(ZBR* a) : agent(a) {}
        void	handle(Event*);
private:
        ZBR    *agent;
	Event	intr;
};

class ZBR_LocalRepairTimer : public Handler {
public:
        ZBR_LocalRepairTimer(ZBR* a) : agent(a) {}
        void	handle(Event*);
private:
        ZBR    *agent;
	Event	intr;
};


/*
  Broadcast ID Cache
*/
class ZBR_BroadcastID {
        friend class ZBR;
 public:
        ZBR_BroadcastID(nsaddr_t i, u_int32_t b) { src = i; id = b;  }
 protected:
        LIST_ENTRY(ZBR_BroadcastID) link;
        nsaddr_t        src;
        u_int32_t       id;
        double          expire;         // now + BCAST_ID_SAVE s
};

LIST_HEAD(zbr_bcache, ZBR_BroadcastID);


/*
  The Routing Agent
*/

class SSCS802_15_4;

class ZBR: public Agent {

  /*
   * make some friends first 
   */

        friend class zbr_rt_entry;
        friend class ZBR_BroadcastTimer;
        friend class ZBR_NeighborTimer;
        friend class ZBR_RouteCacheTimer;
        friend class ZBR_LocalRepairTimer;
        friend class SSCS802_15_4;
 public:
	static unsigned int Cskip_BSize(void);
	static unsigned int c_skip(unsigned int Li); 

        ZBR(nsaddr_t id);

        void		recv(Packet *p, Handler *);
        
        double		dRate;		//zheng: add

 protected:
        int             command(int, const char *const *);
        int             initialized() { return 1 && target_; }

        /*
         * Route Table Management
         */
        void            rt_resolve(Packet *p);
        void            rt_update(zbr_rt_entry *rt,
							u_int16_t metric, nsaddr_t nexthop,
		      				double expire_time);
        void            rt_down(zbr_rt_entry *rt);
        void            local_rt_repair(zbr_rt_entry *rt, Packet *p, bool retry = false);

	//// zhu: add
	unsigned int myIDType;		// Node ID type ((CID,NID) or IEEE)
	u_int32_t  myNodeID;		// Node ID. 
	unsigned int myDepth;		// 4-bit Depth. 
	unsigned int myFreq;		// 4-bit Frequency. 
	unsigned int myType;		// 8-bit Type of the node (DD, Root, network or gateway)
	u_int32_t  myParentNodeID;	// Parent NID

	//// end zhu
 		
	//NEW: Yong modified on 10/10/03 for ZigBee routing
 	nsaddr_t rt_clusterTree(nsaddr_t dst);
	//NEW END
 
 public:
        void            rt_ll_failed(Packet *p);
        void            handle_link_failure(nsaddr_t id);
 protected:
        void            rt_purge(void);

        void            enque(zbr_rt_entry *rt, Packet *p);
        Packet*         deque(zbr_rt_entry *rt);

        /*
         * Neighbor Management
         */
	void		sscs_nb_insert(nsaddr_t id, unsigned char Rel);		//zheng: add
        void            nb_insert(nsaddr_t id, unsigned char Rel = UNSTABLE);
        ZBR_Neighbor*  nb_lookup(nsaddr_t id);
        void            nb_delete(nsaddr_t id);
        void            nb_purge(void);

        /*
         * Broadcast ID Management
         */

        void            id_insert(nsaddr_t id, u_int32_t bid);
        bool	        id_lookup(nsaddr_t id, u_int32_t bid);
        void            id_purge(void);

        /*
         * Packet TX Routines
         */
        void            forward(zbr_rt_entry *rt, Packet *p, double delay);
        
        //NEW: Yong modified on 10/10/03 for ZigBee routing
        void            tree_forward(nsaddr_t tree_rt, Packet *p, double delay);
	//NEW END
        
        
        void            sendRequest(nsaddr_t dst);
                

        //NEW: Yong modified on 10/10/03 for ZigBee routing
	void            sendReply(nsaddr_t ipdst, u_int32_t hop_count,
                                  nsaddr_t rpdst, 
                                  u_int32_t lifetime, double timestamp, unsigned int allbrctflag, unsigned int
				  locreplflag, unsigned int grat);
        //NEW END                          
        /*
         * Packet RX Routines
         */
        void            recvZBR(Packet *p);
        void            recvRequest(Packet *p);
        void            recvReply(Packet *p);

	/*
	 * History management
	 */
	
	double 		PerHopTime(zbr_rt_entry *rt);


        nsaddr_t        index;                  // IP Address of this node
        int             bid;                    // Broadcast ID
        
        //NEW: Yong modified on 10/10/03 for ZigBee routing
        unsigned int   RNType;
        //NEW END
        

        zbr_rtable         rthead;                 // routing table
        zbr_ncache         nbhead;                 // Neighbor Cache
        zbr_bcache         bihead;                 // Broadcast ID Cache

        /*
         * Timers
         */
        ZBR_BroadcastTimer  btimer;
        ZBR_NeighborTimer   ntimer;
        ZBR_RouteCacheTimer rtimer;
        ZBR_LocalRepairTimer lrtimer;

        /*
         * Routing Table
         */
        zbr_rtable          rtable;
        /*
         *  A "drop-front" queue used by the routing layer to buffer
         *  packets to which it does not have a route.
         */
        zbr_rqueue         rqueue;

        /*
         * A mechanism for logging the contents of the routing
         * table.
         */
        Trace           *logtarget;

        /*
         * A pointer to the network interface queue that sits
         * between the "classifier" and the "link layer".
         */
        PriQueue        *ifqueue;

        /*
         * Logging stuff
         */
        void            log_link_del(nsaddr_t dst);
        void            log_link_broke(Packet *p);
        void            log_link_kept(nsaddr_t dst);

	/* for passing packets up to agents */
	PortClassifier *dmux_;
};

#endif 

//end of file: zbr.h

