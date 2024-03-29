# Franklin's Lab Notebook
## Table of Contents
- [2022-09-13: Post-Meeting Notes](README.md#2022-09-13-post-meeting-notes)
- [2022-09-14: Parts Research](README.md#2022-09-14-parts-research)
- [2022-09-14: Quick note about our traffic monitoring method](README.md#2022-09-14-quick-note-about-our-traffic-monitoring-method)
- [2022-09-15: AWS Research](README.md#2022-09-15-aws-research)
- [2022-09-24: Full Bill of Materials for MOM Node Prototype 1](README.md#2022-09-24-full-bill-of-materials-for-mom-node-prototype-1)
- [2022-10-02: Calculations and Design Approaches for the Schematic](README.md#2022-10-02-calculations-and-design-approaches-for-the-schematic)
- [2022-10-11: Concerns that arose while doing early testing](README.md#2022-10-11-concerns-that-arose-while-doing-early-testing)
- [2022-10-21: Thinking about implementation](README.md#2022-10-21-thinking-about-implementation)
- [2022-10-26: Experimenting with Probe Requests](README.md#2022-10-26-experimenting-with-probe-requests)
- [2022-11-04: Integrating AWS IoT](README.md#2022-11-04-integrating-aws-iot)
- [2022-11-11: Device Enclosure Preparation with Machine Shop](README.md#2022-11-11-device-enclosure-preparation-with-machine-shop)
- [2022-11-17: Tinkering with the Occupancy Estimation Algorithm](README.md#2022-11-17-tinkering-with-the-occupancy-estimation-algorithm)
- [2022-11-25: Fall Break](README.md#2022-11-25-fall-break)
- [2022-12-02: The Occupancy Contribution/Probability Model](README.md#2022-12-02-the-occupancy-contributionprobability-model)

## 2022-09-13: Post-Meeting Notes 
We just talked with our mentor TA about our initial block diagram, high-level and subsystem requirements, and about some of the parts that we are planning on using for the project. She had some great suggestions for us that we think would benefit our project, especially since she had a very similar project when she took the course. \
Here's the list of suggestions that she mentioned:
- Add the subcomponents for the cloud server in the block diagram.
- Put required voltages at each power line in the block diagram.
- Expand the high-level requirement involving power to include 24-hour availability while the board is plugged in.
- Don't bother making your own Wi-Fi module; just buy one since the budget is $150.

## 2022-09-14: Hardware Parts Research
Now that we kind of have our project idea fleshed out, we need to create an initial list of parts to create our prototype. All research will be done with the fact that the budget can only be used on Amazon, Digi-Key, and Mouser. Below is a list of all parts and components, sorted into subsystems, I have researched that should be purchased from the aforementioned sites, along with my thought process behind picking those parts.
- **Control Unit and Radio Scanner Suite**
  - [ESP32-S3-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-s3-mini-1_mini-1u_datasheet_en.pdf)
    - It's no question that the ESP32 family has a good track record with IoT projects. It's very inexpensive and it has a decently-sized community around it to provide support and answers should someone need it.
    - It's also approved and supported by AWS, which is a huge plus should we need help in the programming side of the project.
    - To make this easier for us to quickly build the prototype, we need an ESP32 SoC that has an integrated Bluetooth module as well as a Wi-Fi module to quickly integrate the ***Radio Scanner Suite***. This means we need to get an ESP32 with antennas.
    - As of right now, we aren't planning on integrating the 5 GHz frequency into the ***Radio Scanner Suite*** due to time constraints. The main issue is that if we were to also scan 5 GHz, we would need to purchase a separate Wi-Fi module and write the drivers for it, which would take too much time to integrate.
      - However, the ESP32 that we are getting has native USB support, so if we have the time to integrate the device drivers for the 5 GHz band, we would be able to do so.
      - On the bright side, Espressif is currently developing the ESP32-C5, which will have dual-band and Bluetooth LE capability. This would be the easiest upgrade to scan all Wi-Fi and Bluetooth frequencies, but the release date for this SoC is not currently known (as of 2022-09-14).
  - ~~[USB-to-UART Bridge](http://esp32.net/usb-uart/)~~
    - ~~We need a USB-to-UART bridge IC to actually program our ESP32. The link goes to a web page with a list of USB-to-UART ICs used in other ESP32 development boards, so we know picking one of those should work.~~
    - ~~We are also planning on powering the board through USB when it is plugged in.~~
    - The USB-to-UART Bridge is not necessary for the ESP32-S3 because it has native USB CDC and JTAG support.
      - I go over this in more detail in the notebook entry [2022-09-24: Full Bill of Materials for MOM Node Prototype 1](README.md#2022-09-24-full-bill-of-materials-for-mom-node-prototype-1).
- **Power Supply**
  - LiPo battery
    - 18650 batteries are probably the better choice over choosing a LiPo pack when it comes to the price-to-capacity ratio. However, the ease and form factor of LiPo batteries are more suitable for our design since it's easier to tuck and keep hidden away.
  - [AP2112 Voltage Regulator](https://www.digikey.com/en/products/detail/diodes-incorporated/AP2112M-3-3TRG1/5305555) (3.3V)
    - The ESP32 series of SoCs all use 3.3 volts as their operating voltage. Since all USB types generally operating at or above 5 volts, we need to drop the voltage down. The AP2112 is a tried and true voltage regulator for 3.3 volts, so we'll use it in our design, too.
  - [MCP73831T Battery Charge Controller](https://www.digikey.com/en/products/detail/microchip-technology/MCP73831T-2DCI-OT/1979804)
    - It's very important to keep the battery from overcharging. This battery charge controller appears to be very popular with other boards that are based on ESP32.

## 2022-09-14: Quick note about our traffic monitoring method
So when our team initially talked about this idea, we were certain that we would need to purchase a separate Wi-Fi module to scan both bands of the 802.11 spectrum (2.4 GHz and 5 GHz). However, after a bit of research, and because of a hardware limitation of the ESP32 platform (as of 2022-09-14), we found that only scanning the 2.4 GHz frequency may be sufficient. This is because most Wi-Fi devices send probe requests after a certain event or a certain amount of time, which is dependent on the operating system used by the probing device. These probe requests are sent on both the 2.4 GHz and 5 GHz bands, since not all Wi-Fi networks utilize the 5 GHz band. 

The discovery phase of a Wi-Fi connection can be broken down to the following steps:
1. Wi-Fi client sends a (periodic) probe request to its general viscinity.
2. All Wi-Fi access points (APs) that have read the probe request frame generate and send a probe response to the client (unless the AP is configured otherwise).
3. Wi-Fi client received the probe response from the responding APs to generate the list of available Wi-Fi connections.  
  
Here are some good references to take a look at to learn more:
- [What are Passive and Active Scanning? - Wi-Fi Alliance](https://www.wi-fi.org/knowledge-center/faq/what-are-passive-and-active-scanning)
- [802.11 MAC Series – Basics of MAC Architecture – Part 3 of 3 - CWNP](https://www.cwnp.com/802.11-mac-series-ndash-basics-mac-architecture-ndash-part-3/)
- [Oliveira et al. 2019](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=8747391)

## 2022-09-15: AWS Research
Here's an official AWS web page about how to use AWS for the connected home. This diagram outlines our potential flow with respect to how each device in our fleet will interact with AWS. [Link to diagram here](https://aws.amazon.com/iot/solutions/connected-home/).

## 2022-09-24: Full Bill of Materials for MOM Node Prototype 1
For me, this entire week was spent on finalizing the bill of materials for the first prototype of a MOM node. This list goes in-depth on all major ICs, receptacles, and connectors needed for each MOM node.
- [ESP32-S3-MINI-1-N8 SoC](https://www.digikey.com/en/products/detail/espressif-systems/ESP32-S3-MINI-1-N8/15295890?s=N4IgTCBcDaIKYGcAOBmMBaBL0FsCWAdniALoC%2BQA)
  - ESP32 is a powerful and popular family of SoCs that have microcontrollers, GPIO connectivity, and integrated Wi-Fi and Bluetooth transceivers. We picked the ESP32-S3 specifically because it does not require the use of a separate USB-to-UART bridge to communicate with it from a computer. This is because it contains a USB On-The-Go chip that also supports USB CDC and JTAG. [You can read more about it here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/establish-serial-connection.html).  
- [Molex USB 2.0 Type-C Receptacle](https://www.digikey.com/en/products/detail/molex/2137160001/13662558)
  - We plan on using USB for flashing software and for the power supply. Thus, we need a USB receptacle for the board.
- [AP2112 (3.3V) Linear Voltage Regulator](https://www.digikey.com/en/products/detail/diodes-incorporated/AP2112M-3-3TRG1/5305555)
  - The ESP32 has a normal operating voltage of 3.3V, and since both LiPo batteries and USB power supply higher voltages, we need to step it down using a voltage regulator. 
- [MCP73831-2ACI Battery Charge Controller](https://www.digikey.com/en/products/detail/microchip-technology/MCP73831T-2ACI-MC/7065590)
  - To ensure proper safety and health of the LiPo backup battery, we will use a battery charge controller. This battery charge controller is popular with other ESP32-based boards.
- [LC709204 Battery Fuel Gauge](https://www.digikey.com/en/products/detail/microchip-technology/MCP73831T-2ACI-MC/7065590)
  - We thought that it would be nice to receive a notification if a node is running on battery power so that we would know if a node's wall power supply needs to be checked out. This battery fuel gauge IC is easy to use and connects directly using I2C.
- [3.7V Li-Po Battery (JST-PH connector)](https://www.digikey.com/en/products/filter/batteries-rechargeable-secondary/91?s=N4IgTCBcDaIIYBM4DMBOBXAlgFxAXQF8g)
  - This will act as a battery backup should a node's wall power be disconnected for whatever reason.
- [2-pin JST-PH Header](https://www.digikey.com/en/products/detail/jst-sales-america-inc/S2B-PH-K-S-LF-SN/926626)
  - This is a connector for the LiPo battery.
- [DMG3415U MOSFET](https://www.digikey.com/en/products/detail/diodes-incorporated/DMG3415UFY4Q-7/5970445)
  - This is the Diode-OR circuit that will decide whether the wall charger or the battery will power the node.  
- [NRVB540MFS Diode Rectifier](https://www.mouser.com/ProductDetail/onsemi/NRVB540MFST1G?qs=xGcJQ%252BnsJwtnjlj0htu6yg%3D%3D)
  - This diode rectifier is used to keep the incoming power as steady as possible so that other components receive steady power.

## 2022-10-02: Calculations and Design Approaches for the Schematic
- **USB Type-C** 
  - Our decision to use USB for our device stems from the fact that our microcontroller needs to connect to our laptops somehow so that we can program them. Since all modern laptops have USB ports, it would make sense to have a USB interface with the microcontroller so that we can just plug in a USB cable.
  - As we would like to "future-proof" our devices, we decided to go with a USB Type-C receptacle instead of a Micro USB (USB OTG) receptacle.
  - USB is a standard which provides a bus to power devices and transfer data, which makes it a perfect all-in-one solution.
  - MOM devices will never act as USB hosts, as they are simply going to be powered by a USB power supply or will just be programmed by a host computer, so they are (electrically) treated as a sink device on the Configuration Channel (CC) pins.
    - All sink devices will need to have their Configuration Channel pins connected to 5.1K Ohm resistors that are connected to ground.
    - ![USB Type-C Configuration Channel Requirements](math/USB-C_Config_Channel_Resistance.png)  
    - A more in-depth review of the electrical aspects of USB Type-C can be seen in [this document provided by STMicroelectronics](https://www.st.com/resource/en/technical_article/dm00496853-overview-of-usb-type-c-and-power-delivery-technologies-stmicroelectronics.pdf).
- **Battery Charging Circuit**
  - Since we would like to have our backup battery charge while the MOM device is plugged into wall power, we will have a battery charging circuit on the MOM device.
  - This battery charging circuit (part of the Power Supply subsystem) is made up of two ICs: an MCP73831 battery charge controller and a MAX17048G battery fuel gauge.
  - The MCP73831 is a popular battery management controller for Lithium-Polymer batteries. As such, we chose it as our battery charge controller.
    - Because we want to have each MOM device to last for at least one hour while on battery power, we calculated that the minimum battery capacity required to do so is at least 500mAh.
    - For Lithium-Polymer batteries, it is unsafe to charge it at a current that is higher than its capacity divided by one hour. For our battery, this means that we should not charge it using a current higher than 500mA.
    - The MCP73831 datasheet provides a simple formula (seen below) that we can use to set its charge rate by using a programming resistor at its "PROG" pin.
    - ![MCP73831 charge rate formula from its datasheet](math/MCP73831_R_Prog_Formula.png)
    - From this formula, we can see that the resistance of the programming resistor required to set the current to no more than 500mA is ***2K Ohms***.
  - The MAX17048G is a battery fuel gauge that can calculate battery levels and communicate its findings over I2C. We plan on using this to display the battery level of each node so we can determine if there are any issues with the battery backup of that node.
    - Since we are going to just use it as a simple fuel gauge, we decided to use the typical application circuit shown below (which was also from the datasheet).
    - ![Simple MAX17048G fuel gauge circuit](math/MAX17048G_simple_fuel_gauge.png)
- **Battery Backup Switchover**
  - After doing some research, I found that the most efficient circuit for a (relatively) seamless battery backup switchover circuit involves a P-channel MOSFET. By having the voltage from USB as the gate, and the voltage from the battery as the source/drain, you can easily decide which power source to draw from.
  - In this setup, when USB is powering the device, the voltage drop between the gate and source (Vgs) will be above the turn-on voltage range, so no power from the battery will go through. When USB is disconnected from the device, Vgs will be within the range of turn-on voltage range, allowing power from the battery to go through.
  - There is a Schottky diode used to protect the USB power line from battery power. A Schottky diode was chosen due to its low turn-on/forward voltage, and the specific model being used was chosen due to its combination of high reverse voltage blocking and low reverse current leakage.
- **Voltage Regulator**
  - The AP2112 is a commonly used voltage regulator in microcontroller platforms that use 3.3 volts, so we picked it because of that. 

## 2022-10-11: Concerns that arose while doing early testing
Our parts came in earlier this week and we were able to pick them up. Among the parts that we ordered is a development board that we plan to use to code our microcontroller program while we wait for PCB orders to be fulfilled. As we started developing, we found out about an issue that would force us into making certain algorithm design decisions. This issue is how a device is able to randomize its Wi-Fi MAC address. For example, every time an iOS device sends a probe request, it will randomize its MAC address for each request. This would make it very difficult to have the true count of devices using just basic MAC address storage/lookup. 

We're thinking that one way of mitigating this would be connecting to IllinoisNet instead of the IllinoisNet_Guest network so that we could truly monitor just IllinoisNet. This would cause each device to just be mapped to a single MAC address since the MAC address cannot be changed during a connection. Since IllinoisNet requires a certificate, we could do something similar to how [someone at UMich connected their ESP32 to their university's network](https://www.youtube.com/watch?v=bABHeMea-P0).

Fortunately, one of the references in our RFA/design document proposes another way of mitigating this. You can read the proposed method in the [research publication on IEEE](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8747391).

## 2022-10-21: Thinking about implementation
So while we're waiting for our PCB to come in, we've been developing using the development board that we ordered. So far, we have had a bit of success with the current code that we have. Now, we're trying to look for ways to optimize the counting algorithm and make it as accurate as we can. This involves a bit more testing and some more math. We've also started researching how to implment the IoT Cloud subsystem on AWS and how to connect the development board to it. 

## 2022-10-26: Experimenting with Probe Requests
I spent a couple days this week trying to tune the parameters for Wi-Fi scanning. However, one of the problems that I found while I was experimenting was with respect to the frequency that devices send probe requests while already connected to a network. When a device sends a probe while already connected to a network, it sends significantly fewer probe requests than if it were not connected. 

One of my experiments this week involved finding the time intervals in which different computer operating systems send probe requests while already connected to a network. I put a MOM node next to the devices I was testing and let it collect data over a period of one hour. These devices consisted of my laptop running Windows 10/11, a smartphone running Android, a smartphone running iOS, and one of my roommate's laptops running macOS. My roommate and I were actively using our respective laptops, while both smartphones were left in a locked state.

During this one hour interval, I found that devices running Windows 10/11 send a probe request about every 10 minutes (which can be seen in the screenshots below). On one occasion, our MOM device caught traffic from the Windows device less than 2 minutes after its previous probe request, but this wouldn't have an effect on how we plan on measuring occupancy if this happened in the real world. 

![Windows 10 Probe Request detection 1](screenshots/win10+11_probe_log_1.png)
![Windows 10 Probe Request detection 2](screenshots/win10+11_probe_log_2.png)
![Windows 10 Probe Request detection 3](screenshots/win10+11_probe_log_3.png)

We also found that the probe request interval for MacBooks running macOS is also about every 10 minutes. However, we found that iOS rarely sends probe requests when it is locked. This could be a double-edged sword. On one hand, it helps mitigate over-counting people if they have multiple Wi-Fi devices. On the other hand, if a person isn't using their laptop (if they are taking a break from studying, for example) we could be under-counting because the MOM node would not count the actively used smartphone. In the end, I'll still need to figure out if (and how) we can keep the occupancy measurement as close to ground truth as possible.

## 2022-11-04: Integrating AWS IoT
I spent the bulk of this week adding the integration with AWS IoT into the microcontroller codebase. To publish the occupancy data to the database for the web application to use, the microcontroller will use the MQTT protocol to send messages to the AWS IoT Core. The AWS IoT Core can then forward the MQTT message it receives from the MOM device to a database via a forwarding rule. The web application can read from the database to update the occupancy data on the web page. This follows the official AWS IoT Core flow diagram below.

![Official AWS IoT flow diagram](https://d1.awsstatic.com/IoT/diagrams/AWS%20IoT%20Core%20-%20Connect%20and%20Manage.edb43e92d542f4053727eaeda267e3776382fd06.png)

Since there is only one Wi-Fi antenna on the ESP32-S3, it cannot be connected to the internet and scan for Wi-FI traffic simultaneously. To overcome this, I programmed the microcontroller to switch between the two modes. Depending on what part of the data collection flow the MOM node is currently on, it will set the Wi-Fi antenna to the corresponding mode. When it is scanning, the antenna is put into promiscuous mode. After a certain amount of time, promiscuous mode is turned off, and it starts to estimate the occupancy based on the scan data. After it does that, it will set the Wi-Fi antenna to station mode and connect to the internet and AWS. Once it publishes the data, it will disconnect from the internet and re-enable promiscuous mode to scan for probe requests again. See the flowchart and an example demo screenshot below.

![MOM Device Flowchart](screenshots/MOM_Device_Program_Flowchart.png)

![MOM IoT Core Demo](screenshots/IoT_Core_Demo.png)

## 2022-11-11: Device Enclosure Preparation with Machine Shop
I spent this week with more attempts of fine tuning the occupancy estimation algorithm. Unfortunately, I couldn't get too much progress with this, even after reading some more research papers on the topic. The primary issue with referencing most of the research papers on the topic of occupancy detection through probe requests is the fact that the scenarios that apply to the research publications don't apply to the scenario in which we are deploying MOM. I briefly touch on this in [2022-10-26: Experimenting with Probe Requests](README.md#2022-10-26-experimenting-with-probe-requests), but to elaborate, the research publications target scenarios when the devices are not already connected to a network, such as on public transportation or while out and about. 

In these scenarios, client devices send probe requests much more frequently, allowing the researchers to use algorithms to map patterns to estimate occupancy. Since we plan on deploying MOM devices in places such as a public library, the devices will already be connected to a network like IllinoisNet. This makes it harder for us to keep tabs on these devices in real-time since they don't send probe requests often, if at all, while already connected to the internet. I am trying to come up with an algorithm of my own to estimate occupancy in our situation, which I will be continuing work on next week.

Meanwhile, I was also working with Gregg from the Machine Shop this week to build an enclosure for our MOM device. We found a plastic enclosure that suits the size of our PCB and backup battery. A plastic enclosure should not affect any the signal strength parameters that the algorithm uses since plastic is practically invisible to the 2.4GHz waves that we are using. Gregg and the others from the Machine Shop will drill holes in the plastic enclosure so that the MOM board can be mounted to the case. Also, a hole will be made on one side of the enclosure for the USB-C port on the board, allowing for easy connection with a USB-C cable to program and power the board while it is in the case.

## 2022-11-17: Tinkering with the Occupancy Estimation Algorithm
At this point, the physical MOM device is ready and is packaged in an enclosure. I've noticed that the enclosure slightly interferes with the RSSI tolerance ranges that the algorithm uses, but it shouldn't have too much of an impact. However, as I was adjusting these tolerances, I found that the current algorithm does not do a good job of trying to mitigate things such as counting people who are simply passing by and the MAC address randomization used for probing by mobile devices, both of which would cause overcounting. 

Another concern is that not every Windows machine will send periodic probe requests while already connected to the internet. For example, my Windows laptop is equipped with an Intel Wi-Fi controller. When I was testing an earlier version of the algorithm a couple of weeks ago, I found that my laptop sent probe requests with a period of about 10 minutes. However, one of my teammate's laptops has a Wi-Fi controller manufactured by a different company. As these Wi-Fi controllers are made by different companies, the software drivers for them are also developed differently. As such, they may not behave exactly the same, and this is what I saw with my teammate's laptop, as it didn't probe at all when already connected to the internet during a 30-minute testing period.

Because of this and the possible overcounting, I decided to rethink what exactly occupancy would mean in our project, since we won't be able to reliably map the occupancy to just the number of MAC addresses we see from probe requests at a given point in time. For now, the best metric/model I could think of involves traffic flow and how busy the area seems. As someone walks through a building, their smartphone's connection to the access point it was previously connected to becomes weaker and weaker. It is at this point when the smartphone sends a probe request to try and connect to an access point with a stronger signal. If we can capture this traffic flow and then try to map it to the number of people in an area, then we can estimate occupancy that way. We also need to consider signal strength relative to the MOM device to try to see if the traffic is within the room or just passing by.

## 2022-11-25: Fall Break
This week was Fall Break, and since I was more or less done with building the physical MOM device and developing the microcontroller code for it, I was only performing the final verifications and recording our results. All of the requirements for the subsystems I was in charge of have been verified, and so was the high-level requirement of the device running for at least one hour while on battery power. However, because of the scarcity of sent probe requests by devices already connected to the internet, the high-level requirement of estimating sector occupancy with at least 80% accuracy is becoming increasingly more difficult through just measuring probe requests. If we used a much more intrusive method, this would not be too difficult. However, ethical reasons deter the usage of these intrusive methods.

## 2022-12-02: The Occupancy Contribution/Probability Model
We noticed that after monitoring a space for a long time, the MOM device eventually reads an explosive increase in detected devices when no new people enter the room, which in turn increases the estimated occupancy. To mitigate this, we instead calculate occupancy based off of the probability that the detected device represents an entire person. With this new probability model, we try to take into account factors such as the number of devices per person, the dimensions of the environment where the device is deployed, and extraneous traffic from devices from other areas (such as other floors and other rooms). The probability primarily focuses on the signal strength and timestamps of each captured probe request, where the probability decays over time if the device has not been seen for a while since the previous timestamp. We also tried to implement a way to detect explosive increases in detected devices within a short period of time. While we were able to detect and mitigate these, the algorithm we use would ignore explosive increases if it actually happened. For example, if a class is starting in a classroom, and many students enter the room at the same time, the algorithm currently thinks that mitigation actions are required even though the increase is the ground truth.

In general, the mitigation would not really matter too much for the areas where we would be primarily deploying MOM devices, such as Grainger Library and the ECEB Atrium, since those are large, open spaces with lots of movementand traffic flow. When we tested this algorithm in the Senior Design Lab, we found that it works relatively well. It would be rare to see explosive increases in occupancy in a short period in those spaces. However, the shortcoming of this mitigation approach is losing the accuracy in smaller rooms. Given more time to fine-tune the mitigation algorithm, I believe that we would be able to find an optimal way to identify when mitigation actions should be taken based on these explosive increases.
