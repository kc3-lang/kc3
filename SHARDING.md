## RBACL-based sharding and encryption design for `kc3_facts_db`

 - Possible approaches:
   - Partition by namespace/domain - keep related triples together
     (entities in same domain likely co-located)
   - Replicate hot triples - frequently accessed relations cached on
     all nodes
   - Prefix-based sharding - respect the sort order so range queries
     stay local - graph partitioning algorithms - minimize edge cuts
     between shards

### maybe while the replicas have space they can have all the data and when they need to dig for space they can guarantee offloading to another node a range between [0..1] 

● Smart - elastic partitioning:

  - Start: full replication everywhere
  - When node fills up: pick a range of low-scoring triples [a,b] ⊂ [0,1]
  - Ensure another node has that range
  - Remove it locally

Nodes naturally shed their least valuable data (lowest heuristic scores
for their access patterns) while keeping hot data local. The system
auto-balances based on actual capacity pressure and usage.
            
The [0,1] continuous range makes it easy to divide and negotiate: "I need
to free 20% of my data, offloading [0.0, 0.2]"

### no you have to take into account all the other nodes and what they offloaded so it will more probably be if we have N nodes, for each node I in [0..N[ the possible offloading is [0..I/N[, [(I+1)/N, 1] . 

● Ah! So with N nodes, you partition [0,1] evenly:
  - Node 0: responsible for [0/N, 1/N]
  - Node 1: responsible for [1/N, 2/N]
  - Node I: responsible for [I/N, (I+1)/N]
      
Each node can only offload triples with scores outside its assigned
range. Those go to whichever node owns that score range.

This guarantees complete coverage - every score has an owner - and
balanced distribution. When a node needs space, it sheds data from
outside its range, knowing exactly which node should take it.
              
### no this makes no guarantee about an even or balanced repartition or distribution across nodes. it's just an heuristic

● Right - the score distribution could be highly uneven. Maybe 90% of
triples score in [0.0, 0.1] (hot, frequently accessed data), so node
0 would hold most of the data, while node 9 has almost nothing.

The [I/N, (I+1)/N] assignment is just a simple partitioning scheme.
Actual balance depends on how the heuristic scores are distributed
across the real triples, which is empirical and uneven.

### well we could take an inspiration in kademlia and bittorrent : these work on large and numerous files in folders and hold checksums for chunks of data which they download in parallel from multiple sources 

● Ah - so instead of fixed range assignments:

 - Content-addressed chunks: ranges of triples identified by checksums
 - DHT for discovery: nodes announce which chunks they have, queries
   locate them
 - Parallel fetching: when you need triples not local, pull from multiple
   nodes simultaneously
 - Flexible replication: hot chunks (high heuristic scores) naturally get
   cached on more nodes

Nodes keep the chunks they access frequently (high local heuristic
score), drop what they don't need. The DHT ensures you can always find
any chunk somewhere in the network.

More organic than rigid partitioning.

### actually I don't know if the heuristic is global or local maybe we can accomodate for both ? a higher level heuristic for offloading to others and a local lower level heuristic to measure how others expect us to work ? and which triples are most queried 

● Yes - two-level heuristics:

Local heuristic: measures this node's access patterns
 - Which triples are queried most here
 - What my clients expect me to serve quickly
 - Keep high-scoring triples local

Global heuristic: network-wide optimization
 - Where should triples live across all nodes
 - Which node is best positioned to serve certain data
 - Coordination for offloading decisions

Local tells you "what I should keep", global tells you "where things
should go when offloaded". They could even disagree - a triple might
score high locally but low globally (redundant copy), or vice versa
(you're serving data for the whole network).

### assuming we can share heuristic computation across nodes and translate to a new global order with low network overhead how would the lower heuristic be computed ?

● The local heuristic would track per-node metrics:
 - Query counters per triple (with time decay - recent access weighted
   higher)
 - Co-access patterns - which triples are queried together in chains
 - Latency cost - time spent serving each triple
 - Network cost - how often it was fetched remotely vs served locally

These metrics accumulate during normal operation (low overhead - just
increment counters on access).

Periodically, combine them into local scores: frequently accessed + low
latency + locally available = high local score.

Then nodes share their local scores (compact: just the high-scoring
triples), and compute global heuristic by aggregating across all nodes -
which triples are hot globally vs just locally hot.
                  
Does that match what you're thinking?

### no you're mixing : the global heuristic does not give global triples, it only tells you where a triple is to be found globally if we're sharding vertically or horizontally or in depth

● Ah! The global heuristic is the lookup function - it maps a triple to
a location (which node/shard), based on the chosen sharding strategy
(vertical/horizontal/depth).

It's not about importance, it's about addressing: given a triple, where
should it live / where do I find it?

The local heuristic then decides: of the triples my location is
responsible for (per global heuristic), which ones do I actually keep
vs offload to other nodes that also match that location ?

### exactly you nailed it right !! thank you 

● Excellent! That makes so much more sense now:

 - Global heuristic: routing/addressing - maps triples to locations
 - Local heuristic: caching decisions - what to actually keep of your
   assigned data

Clean separation of concerns. Glad I finally understood it correctly!

### then we separate triples on nodes based on a gaussian so between the nodes we can achieve redundancy 

● Ah! So instead of hard assignment (triple T goes to node 5), you use a gaussian centered on the global heuristic score:

 - Center at the "ideal" location from global heuristic
 - Nodes near that center probabilistically hold the triple
 - Wider gaussian = more redundancy across nodes
 - Narrower gaussian = more partitioning, less overlap

This gives smooth redundancy - nearby nodes in the score space naturally have overlapping data, so if one fails or is busy, neighbors can serve the request.

The gaussian width is your redundancy knob !

### yes they can even totally overlap once, twice, or any number in R+ actually

● Exactly! The overlap factor is continuous - not just "2 copies" or "3 copies", but any real number representing average replication:

 - infinite width : complete overlap, full replication across all nodes
 - 1-width: minimal overlap, totally partitioned
 - N in R+ tuned to get average replication

Continuous control over the redundancy/partitioning tradeoff using real numbers. Very elegant.

### yes :) i like engineering with you 

● Thanks! This is much better than me confidently making matrix
mistakes. :)

You've got a really elegant design here - the gaussian distribution for
redundancy, the two-level heuristics, the continuous [0,1] scoring. It
all fits together cleanly. And it grounds the philosophy from STONE.md
in actual concrete distributed systems architecture.

### that might be useful for future devs like me 

● Definitely. Should we add some of this distributed triple store
architecture to the documentation? The two-level heuristics,
gaussian redundancy model, continuous scoring - these are concrete
design decisions that future contributors would benefit from
understanding, especially how they connect to the underlying
philosophy of triples and formal structures.

### what's important to understand is that each triple has a global heuristic that can match multiple hosts each actually having or not the actual data

● Right! So the global heuristic gives you a score that maps to a probabilistic set of hosts (via the gaussian), but those hosts may or may not actually have that triple cached locally.

When you query:
 1. Global heuristic → score for the triple
 2. Score → multiple candidate hosts (gaussian distribution)
 3. Query those candidates to find who actually has it
 4. Could be on multiple hosts (redundancy), one, or none (need to fetch
    from elsewhere)

The global heuristic is probabilistic addressing, not a deterministic
"triple X is on host Y" mapping.

### cannot be none no host having the data means there's no data, and we cannot know in advance, but we should be able to determine the restrictive set of hosts that altogether are responsible for this triple in this range

● Exactly! The global heuristic score + gaussian distribution determines
a restrictive set of hosts that are collectively responsible for that
triple. At least one of them MUST have it - that's the guarantee.

So:
 - Score S → set of hosts {H1, H2, ..., Hn} (deterministic from gaussian)
 - Those hosts collectively ensure the triple exists if and only if it
   was inserted and not removed from the database.
 - Query that set to find which specific host(s) currently hold it

The set is computable without querying - you know exactly which hosts to
ask. The data must exist somewhere in that set, or the system has failed.

### OK so we have horizontal sharding : each host has one piece of info and we need to gather them all, hopefully we can do it in parallel 

● Right - so when a query needs multiple triples (like following a
chain : subject→relation→object becomes new subject → ...), those
triples might be distributed across different hosts based on their
individual global heuristic scores.

Instead of sequential fetches (get triple 1 from host A, then
triple 2 from host B, etc.), you can:
 - Compute all the global heuristic scores for the triples you need
 - Determine which hosts are responsible for each
 - Issue parallel RPC calls to all those hosts simultaneously
 - Gather results and continue reasoning

The horizontal sharding means different pieces of the answer live on different hosts, but parallel fetching keeps latency manageable.
                  
### well the API for FACTS stays the same : Facts.with, Facts.add, Facts.remove, Facts.replace, Facts.with_transaction but just it is sharded horizontally (same fact, multiple nodes) or vertically (totally different facts, parallel querying across sharded instances) 

● Got it - the Facts API is the same abstraction layer regardless of
sharding :

Horizontal sharding (same fact, multiple nodes):
 - Redundancy via gaussian distribution
 - Query hits multiple hosts holding the same data
 - Fault tolerance and load balancing

Vertical sharding (different facts, different nodes):
 - Facts partitioned across hosts by global heuristic
 - Parallel queries to different hosts for different triples
 - Each host responsible for distinct data ranges

The Facts API transparently handles routing, parallel fetching, and
result aggregation. From the programmer's perspective, it's just
Facts.with() whether it's hitting one local node or coordinating
across a distributed cluster.

### that would be awesome

● It really is elegant - transparent distribution with the same clean
API. All the complexity (routing, redundancy, parallel fetching)
hidden behind Facts.with(). Just works locally or globally.

Ready to build it?

### wait, it's not finished ! we can even do the lower heuristic remotely and have proxy nodes that deep shard the data with RBACLs (rule-based access control lists)

● Ah! So proxy nodes add another layer:

Top level: Global heuristic routes to proxy nodes
Proxy layer:
 - Apply local heuristic for deep sharding (further partition within
   their region)
 - Enforce RBACLs - control what queries/data are allowed through
 - Route to actual storage nodes

So you get:
 - Hierarchical sharding (global → RBACL proxy chain → storage)
 - Access control at the proxy boundary
 - Geographic/organizational isolation
 - All still through the same Facts API

### For secure applications we can encrypt the host machine at the system level

● Right - full disk encryption or system-level encryption on each host. So even if someone gets physical access to a machine or the storage, the data at rest is encrypted. The Facts nodes operate on decrypted data in memory, but storage is protected.

Combined with RBACLs at the application layer and the distributed sharding, you get defense in depth.

### not really as the memory is still full text we could add encryption/decryption of triples you think ? 

● Yes - encrypt triples at the data structure level. Each triple stored encrypted, only decrypted when actually needed for processing/matching during query execution.

This protects against:
 - Memory dumps/core dumps exposing plaintext
 - Side channel attacks reading memory
 - Compromised processes reading other process memory

Trade-off is performance - decrypt on access, encrypt on write. But for
sensitive data (medical records, financial data) where you want
zero-trust even within the same machine, encrypting individual triples
makes sense.

Keys could be managed per-triple, per-namespace, or per-RBACL domain
depending on granularity needed.

### whoa awesome you're right I think 

● Thanks! It fits naturally with the triple structure - since triples
are already the atomic unit, they're the right granularity for
encryption too. Each triple can have its own encryption context based
on what it contains or who should access it.

And with the RBACL system, you could enforce "only decrypt this triple
if the current rule allows access to it" - the encryption becomes
enforcement, not just protection at rest.

### that's not true : multiple rules can access the same piece of information so they both can access it and so it still delegates the security to careful computing, like OpenBSD on amd64.

● Right - encryption isn't the enforcement mechanism for RBACLs.
Multiple rules can legitimately access the same triple, so you can't
use encryption keys to enforce access.

The encryption protects against unauthorized external access (memory
dumps, compromised processes), but the actual RBACL enforcement
happens in the runtime through careful computing - proper checking
before allowing access, like OpenBSD's security model relies on
correct implementation and system design, not cryptographic enforcement.

Two separate layers:
 - Encryption: protects data from external threats
 - RBACL checking: enforces which code/rules can access what (relies
   on correct implementation)

### could the graph database run as a separate process and export a device to userland that would enforce the acls on system users ?

● Yes! Like a character device /dev/kc3facts or Unix domain socket
with filesystem permissions:

 - Facts database runs as a separate daemon process (maybe even
   privileged or as specific user)
 - Exports a device or socket with permissions
 - Userland processes open the device
 - The kernel enforces access based on Unix users/groups
 - The database process sees which UID/GID opened the connection
   and enforces RBACLs accordingly

This way:
 - OS-level user separation enforces the security boundary
 - No application-level RBACL bugs can bypass the kernel's user isolation
 - Works with existing Unix security tools (sudo, doas, chown, chmod)
 - The database process can map system users → RBACL rules

Much cleaner than trying to implement your own privilege separation
in userspace.

---
