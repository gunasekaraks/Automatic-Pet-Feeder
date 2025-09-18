# Automatic Pet Feeder

An automated pet feeder system using **ESP32** and **MQTT** for remote control, featuring stepper and servo motors to manage food, water, and cleaning.

---

## 🛠️ Components

- **Microcontroller:** ESP32  
- **Motors:**  
  - Stepper motor for precise food dispensing  
  - Servo motor for cleaning mechanism  
  - Water pump for dispensing water  
- **Sensors:** Weight sensor to monitor food levels  
- **Communication:** MQTT protocol for remote monitoring and control  
- **Power Supply:** 7.4 V DC powe supply

---

## 📌 Features

### 1. Food Dispensing
- The **stepper motor** precisely rotates to dispense the scheduled amount of food.  
- Portion sizes can be configured via the **Node-RED dashboard**.  
- Weight sensor ensures accurate measurement and prevents overfeeding.  

### 2. Water Dispensing
- **Water pump** automatically fills the pet's water bowl.  
- Controlled remotely via **MQTT**, allowing flexible scheduling or manual refills.  
- Safety checks ensure the pump stops if the bowl is full.  

### 3. Cleaning Mechanism
- **Servo motor** is used to clean the food and water containers.  
- Can be scheduled after feeding times to maintain hygiene.  
- Prevents buildup of leftover food or stagnant water.  

### 4. Monitoring
- **Weight sensor** tracks remaining food and ensures proper portions are dispensed.  
- All actions (food/water dispensing, cleaning) are monitored remotely through **MQTT and Node-RED dashboard**.  
- Provides real-time updates and alerts for low food or water levels.  

---

## 📂 Folder Structure

