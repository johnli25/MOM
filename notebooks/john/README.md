# John's Lab Notebook
## Table of Contents
---
- [2022-09-13: Meeting Notes](README.md#2022-09-13-meeting-notes)
- [2022-09-14: Hardware Components Research](README.md#2022-09-14-Hardware-Components-research)
- [2022-09-16: Meeting with Jack Blevins](README.md#2022-09-14-Meeting-With-Jack-Kilby)
- [2022-09-20: Meeting-With-TA](README.md#2022-09-20-Meeting-With-TA)
- [2022-09-25: Finished-First-Draft-Of-Design-Doc](README.md#2022-09-25-Finished-First-Draft-Of-Design-Doc)
- [2022-10-04: Design-Document-review-and-presentation](README.md#2022-09-25-Design-Document-review-and-presentation)
- [2022-10-07: Finalizing-and-Ordering-Parts](README.md#2022-10-07-Finalizing-and-Ordering-Parts)
- [2022-10-18: More Findings about our Project](README.md#2022-10-18-More-Findings-about-our-Project)
- [2022-10-25: Implementation details + Assigning locker](README.md#2022-10-25-Implementation-details-and-Assigning-locker)
- [2022-11-04: Developing the Front End of the Web App](README.md#2022-11-04-Developing-the-Front-End-of-the-Web App)
- [2022-11-15: Mock Demo](README.md#2022-11-15-Mock-Demo)
- [2022-11-24: Thanksgiving Break](README.md#2022-11-24-Happy-Thanksgiving!)
- [2022-11-28: Finishing project for the final demo](README.md#2022-11-28-Finishing-project-for-the-final-demo!)
- [2022-12-4: Preparing-for-final-presentation](README.md#2022-12-4-Preparing-for-final-presentation)
- [2022-12-6: Final-presentation](README.md#2022-12-4-Final-presentation)

## 2022-09-13: Meeting Notes
Discussed with TA regarding our project proposal and our system and subsystem designs. Some things and suggestions to take note of:
- Add subcomponents for the cloud server for block diagram.
- Label necessary voltages at each power line for block diagram.
- Expand high-level requirement involving power to include 24-hour availability while the board is plugged in.
- Don't bother making your own Wi-Fi module; just buy one since budget is $150.

## 2022-09-14: Hardware Components Research
Since we have a limited $150 budget, all of our purchases will most likely be limited to Amazon, Digikey, and Mouser. Parts we are leaning toward using:

**Control Unit and Radio Scanner Suite**
  - [ESP32-S3-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-2_datasheet_en.pdf)
- This ESP32 is versatile, not super expensive, offers various functionalities, and is used in many IoT projects, both hobbyist and professional.
- Furthermore, this microcontroller has been proven to be compatible with AWS. 
- Currently, we aren't planning on integrating the 5 GHz frequency into the Radio Scanner Suite due to time constraints. The primary issue is that if we were to also scan 5 GHz, we would need to purchase a separate Wi-Fi module and write the drivers for it, which would take too much time and effort to integrate. So we plan on settling for 2.4 GHz frequency for our Wi-Fi module for now
- [USB-to-UART Bridge](http://esp32.net/usb-uart/): We need to make use of this bridge and attach it to our ESP-32. 

**Power Supply**
- LiPo battery: safe, effective, efficient, and cheap. Moreover, the "ease and form factor" of LiPo batteries are a plus.
- [AP2112 Voltage Regulator](https://www.digikey.com/en/products/detail/diodes-incorporated/AP2112M-3-3TRG1/5305555) (3.3V): The ESP32 series of SoCs all use 3.3 volts as their operating voltage. Since all USB types generally operating at or above 5 volts, we need to drop the voltage down. 3.3 V is a very common voltage for these sort of scenarios
- [MCP73831T Battery Charge Controller](https://www.digikey.com/en/products/detail/microchip-technology/MCP73831T-2DCI-OT/1979804): mainly used to prevent overcharging

## 2022-09-16: Meeting With Jack Blevins
We met with Jack Blevins, a UIUC alum whom has earned one of the first EECS degrees
back in 1970 and has a 50+-year track record spanning all kinds of
projects in industry (recommended to use by Professor Lumetta), and will essentially serve as another, "informal" mentor for our 445 project. Our team gave an overview of our project and followed up with some questions and comments from Jack:

- Jack essentially solidified our design decision from earlier and agreed that trying to implement and incorporate 5 GHz frequency into our Wi-Fi module would be too impractical. 
- For the Web programming side of things, we discussed which applications, languages, databases, etc. we were going to utilize. Since we established that we would be using AWS for our server side of things, we were leaning toward programming in Python and using a NoSQL Amazon database like DynamoDB. We were also open to using SQL or MongoDB from our discussions.
- Exchanged contact information so we can contact Jack whenever if we had any technical queries regarding our project.

## 2022-09-20: Meeting-With-TA

TA also sent back feedback for our project proposal. Some suggestions + improvements for our it are:

- Add range (+/- 1 minute) to occupancy updating time
- Update visual aid to add a few more explicit labels ("Occupancy data", etc.)
- Change "AWS IoT" in block diagram to "AWS server"
- More comments can be found on project proposal document that TA sent back to us 

We also discussed design document details. For example, creating a system/subsystem requirement | verification table. 

## 2022-09-25: Finished-First-Draft-Of-Design-Doc

- Finished first draft of Design Doc
- Main work we did was circuit schematics and requirements | verifications table for all the system & subsystem requirements

## 2022-10-04: Design-Document-review-and-presentation

- Prior to the Design document review on Monday (10/3/2022), our team made a couple revisions to some of the requirements and verifications, a block diagram, and the visual aid
- Design document presentation on Tuesday (10/4/2022) went well! Lots of great and constructive feedback from TAs, Professor Fliflet, and student peer reviews
- Judging from the feedback, the biggest functionality/implementation challenge we will most likely face is accurately estimating the Wi-Fi and Bluetooth signal strength/RSSI values while accurately pruning out "extra" or multiple signals and connected devices belonging to a single person.

## 2022-10-07: Finalizing-and-Ordering-Parts

- Our team finished finalizing and ordering all the necessary components for our project. Now we are just waiting for everything to arrive.

## 2022-10-18: More Findings about our Project 

- Our team finished finalizing and ordering all the necessary components for our project (PCB, etc.). Now we are just waiting for everything to arrive.

## 2022-10-25: Implementation details and Assigning locker

- We started writing the hardware arduino code for our project (Franklin started this) and cloud/AWS side of our project (Vish and I started looking and writing implementations for this)
- We also got a locker assigned to us, but it currently does not work.

## 2022-11-04: Developing-the-Front-End-of-the-Web-App

- The past couple works, I've been in charge of developing the front end side of the web app. 
- I used the Flask Python module to both design the front end and set up the pertinent connections and endpoints to our backend DynamoDB.
- I followed a Bootstrap + Jinja template for our front end design. The advantages for these web templates are 1) relatively simple learning curve with ample guides online 2) reactive/responsive 3) Provides a variety of designs, colors, graphs, charts, etc.
- The front end design is written in several HTML files so far: 'index.html' and 'pie-siebel.html'

## 2022-11-15: Mock-Demo
- Our group just performed a mock demo, and it went pretty well in my opinion. TAs said the demo went smoothly and we covered many of the requirements and verifications. We also received some very constructive feedback on what we need to address.
- Overall as a group, we need to run more tests on probing the number of devices/people in a closed room. Afterward, we will, together as a group, add more functionalities to ensure our occupancy probing and identifying algorithms and microcontroller sensor data collections are more smooth and consistent in both accuracy and latency
- On the web app side, I have to slightly update 1) the DynamoDB database structure/organizationd and 2) web app functionality in order to pull the most recent data from each room since we made a slight modification/optimization in that regard.
- This is optional but highly recommend. Change the front end design so there's one "home page" which lists all the rooms available on the web app. Then you can "click" on each room in order to navigate to that room and its occupancy metrics. 
- Also, if I have more time, prettify the web app by adding some more reactive widgets/features and colors. This part shouldn't be too hard

## 2022-11-24: Happy-Thanksgiving!
- Grind the points mentioned above and the rest of the MOM project over Thanksgiving break. Halfway-mostly done with the listed points in the entry above. Be ready for the real demo
- Happy Thanksgiving!

## 2022-11-28: Finishing-project-for-the-final-demo
- Finished all the technical/implementation aspects of the project --> ready to demo
- Web application is up, running, and deployed properly, DynamoDB is finished and presents and organizes data in a clean manner, and MOM device successfully reads WiFi signals and uploads to the AWS cloud successfully.
- Prep for demo by reviewing what Franklin, Vish, and I will be presenting which aspects of the project

## 2022-11-29: Demo
- Demo went great! TAs, peer reviewers, and professor liked our project!

## 2022-12-4: Preparing-for-final-presentation
- Prepared slides presentation and what Franklin, Vish, and I will be saying at the presentation
- Try on suits
- Prepared for our final presentation by doing mock presentation 
- also created a video the other day demonstrating our project/MOM device in action

## 2022-12-6: Final-presentation
- Final presentation went great!
- Some intriguing questions by the peer reviewers-if we had the time, their questions/suggestion are 100% worth considering!
- Done with project! Had a lot of fun working on it with my team. Lots of onerous, hard work and very stressful at times throughout the semester, but we got through it!






