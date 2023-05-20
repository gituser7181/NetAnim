/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0



using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;
    uint32_t nCsma = 3;

    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);

    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
 
    NodeContainer host,router,host1;
    host.Create(2);
    router.Create(4);
    NodeContainer subnet1;
    subnet1.Add(host.Get(0));
    subnet1.Add(router.Get(0));
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay",StringValue("2ms"));
    NetDeviceContainer subnet1Devices;
    subnet1Devices = p2p.Install(subnet1);
    InternetStackHelper stack;
    stack.Install(router);
    stack.Install(host);
    Ipv4AddressHelper add1,add2,add3,add4,add5,add6;
    add1.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer subnet1Interfaces;
    subnet1Interfaces = add1.Assign(subnet1Devices);
    
    NodeContainer subnet2;
    subnet2.Add(host.Get(0));
    subnet2.Add(router.Get(1));
    NetDeviceContainer subnet2Devices;
    subnet2Devices = p2p.Install(subnet2);
    add2.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer subnet2Interfaces;
    subnet2Interfaces = add2.Assign(subnet2Devices);
    
    NodeContainer subnet3;
    subnet3.Add(host.Get(1));
    subnet3.Add(router.Get(2));
    NetDeviceContainer subnet3Devices;
    subnet3Devices = p2p.Install(subnet3);
    add3.SetBase("10.1.3.0","255.255.255.0");
    Ipv4InterfaceContainer subnet3Interfaces;
    subnet3Interfaces = add3.Assign(subnet3Devices);
    
    NodeContainer subnet4;
    subnet4.Add(host.Get(1));
    subnet4.Add(router.Get(3));
    NetDeviceContainer subnet4Devices;
    subnet4Devices = p2p.Install(subnet4);
    add4.SetBase("10.1.4.0","255.255.255.0");
    Ipv4InterfaceContainer subnet4Interfaces;
    subnet4Interfaces = add4.Assign(subnet4Devices);
    
    NodeContainer subnet5;
    subnet5.Add(host.Get(1));
    subnet5.Add(router.Get(2));
    NetDeviceContainer subnet5Devices;
    subnet5Devices = p2p.Install(subnet5);
    add5.SetBase("10.1.5.0","255.255.255.0");
    Ipv4InterfaceContainer subnet5Interfaces;
    subnet5Interfaces = add5.Assign(subnet5Devices);
    
    UdpEchoServerHelper echoServer(9);
    
    ApplicationContainer serverApps = echoServer.Install(subnet5.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));
    
    UdpEchoClientHelper echoClient(subnet5Interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    
    ApplicationContainer clientApps = echoClient.Install(subnet1.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    AnimationInterface anim("hybirdtopology.xml");
    anim.SetConstantPosition(host.Get(0),10.0,20.0);
    anim.SetConstantPosition(host.Get(1),20.0,30.0);
    
    //tcpdump -nn -tt -r second-1-0.pcap
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
