Mac/802_11 set bandwidth_ [lindex $argv 0]
set error_rate [lindex $argv 1]
set packet_size [lindex $argv 2]

set MESSAGE_PORT 42
set BROADCAST_ADDR -1
set X 1200
set Y 800
set rate 200.0Kb

#set val(chan)           Channel/WirelessChannel    ;#Channel Type
set val(prop)           Propagation/TwoRayGround   ;# radio-propagation model
set val(netif)          Phy/WirelessPhy            ;# network interface type



set val(mac)            Mac/802_11                 ;# MAC type
#set val(mac)            Mac                 ;# MAC type
#set val(mac)		Mac/Simple


set val(ifq)            Queue/DropTail/PriQueue    ;# interface queue type
set val(ll)             LL                         ;# link layer type
set val(ant)            Antenna/OmniAntenna        ;# antenna model
set val(ifqlen)         32768                      ;# max packet in ifq


set val(rp) AODV


set ns [new Simulator]

set f [open wireless.tr w]
$ns trace-all $f
$ns eventtrace-all
set nf [open wireless.nam w]
$ns namtrace-all $nf
$ns namtrace-all-wireless $nf $X $Y

# set up topography object
set topo       [new Topography]

$topo load_flatgrid $X $Y

#
# Create God
#
create-god 9


#set mac0 [new Mac/802_11]

$ns node-config -adhocRouting $val(rp) \
                -llType $val(ll) \
                -macType $val(mac) \
                -ifqType $val(ifq) \
                -ifqLen $val(ifqlen) \
                -antType $val(ant) \
                -propType $val(prop) \
                -phyType $val(netif) \
                -IncomingErrProc UniformErr \
                -OutgoingErrProc UniformErr \
                -channelType Channel/WirelessChannel \
                -topoInstance $topo \
                -agentTrace ON \
                -routerTrace OFF \
                -macTrace ON \
                -movementTrace OFF

# Error model for simulating error with a set rate
proc UniformErr {} {
    global error_rate
    set error_model [new ErrorModel]
    $error_model unit pkt
    $error_model set rate_ $error_rate
    $error_model ranvar [new RandomVariable/Uniform]
    return $error_model
}


# Create nodes
# A : 0, B : 1, C : 2
# D : 3, E : 4, F : 5
# G : 6, H : 7, L : 8
for {set i 0} {$i < 9} {incr i} {
	set node_($i) [$ns node]
	$node_($i) random-motion 0
}


# Color nodes black
for {set i 0} {$i < 9} {incr i} {
    $node_($i) color black
}

# Set nodes positions
$ns at 0.0 "set_pos"


# Create UDP agents and attach them to their respective nodes

# Nodes A and D are senders
set udp_agent_0 [new Agent/UDP]
set udp_agent_3 [new Agent/UDP]

# CBR is used to transfer the packets
set cbr_08 [new Application/Traffic/CBR]
set cbr_07 [new Application/Traffic/CBR]
set cbr_3 [new Application/Traffic/CBR]

# Null agents
set null_0 [new Agent/Null]
set null_3 [new Agent/Null]

# Attach node A to L
$ns attach-agent $node_(8) $null_0
$ns attach-agent $node_(0) $udp_agent_0
$ns connect $udp_agent_0 $null_0
# Set up CBR for the connection
$cbr_08 attach-agent $udp_agent_0
$cbr_08 set packetSize_ $packet_size
$cbr_08 set rate_ $rate
$cbr_08 set random_ null



# Attach node D to H
$ns attach-agent $node_(7) $null_3
$ns attach-agent $node_(3) $udp_agent_3
$ns connect $udp_agent_3 $null_3
# Set up CBR for the connection
$cbr_3 attach-agent $udp_agent_3
$cbr_3 set packetSize_ $packet_size
$cbr_3 set rate_ $rate
$cbr_3 set random_ null

# Define some events for the simulation

$ns at 0.1 "$cbr_08 start"
$ns at 100.0 "$cbr_08 stop"

$ns at 0.1 "$cbr_3 start"
$ns at 100.0 "$cbr_3 stop"

# End sim
$ns at 100.0 "finish"
$ns at 100.1 "puts \"NS EXITING...\"; $ns halt"

#INSERT ANNOTATIONS HERE

proc set_pos {} {
    global node_
    global ns

    # A
    $node_(0) set X_ 250.0
    $node_(0) set Y_ 50.0
    $node_(0) set Z_ 0.0
    $ns initial_node_pos $node_(0) 30

    # B
    $node_(1) set X_ 100.0
    $node_(1) set Y_ 250.0
    $node_(1) set Z_ 0.0
    $ns initial_node_pos $node_(1) 30

    # C
    $node_(2) set X_ 400.0
    $node_(2) set Y_ 100.0
    $node_(2) set Z_ 0.0
    $ns initial_node_pos $node_(2) 30

    # D
    $node_(3) set X_ 250.0
    $node_(3) set Y_ 400.0
    $node_(3) set Z_ 0.0
    $ns initial_node_pos $node_(3) 30

    # E
    $node_(4) set X_ 400.0
    $node_(4) set Y_ 350.0
    $node_(4) set Z_ 0.0
    $ns initial_node_pos $node_(4) 30

    # F
    $node_(5) set X_ 500.0
    $node_(5) set Y_ 250.0
    $node_(5) set Z_ 0.0
    $ns initial_node_pos $node_(5) 30

    # G
    $node_(6) set X_ 500.0
    $node_(6) set Y_ 150.0
    $node_(6) set Z_ 0.0
    $ns initial_node_pos $node_(6) 30

    # H
    $node_(7) set X_ 650.0
    $node_(7) set Y_ 100.0
    $node_(7) set Z_ 0.0
    $ns initial_node_pos $node_(7) 30

    # L
    $node_(8) set X_ 650.0
    $node_(8) set Y_ 350.0
    $node_(8) set Z_ 0.0
    $ns initial_node_pos $node_(8) 30
}

proc finish {} {
    global ns f nf val
    $ns flush-trace
    close $f
    close $nf
    # exec nam wireless.nam &
    exit 0
}

puts "Starting Simulation..."

$ns run
