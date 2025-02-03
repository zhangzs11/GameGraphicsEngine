# 🎮 Graphics Module in a Lightweight Game Engine

This repository contains the **graphics module** of a custom game engine.  
It features a modular **rendering framework**, multiple **rendering techniques**, and an **optimized asset pipeline**. 

📌 **Each feature is implemented in a separate branch.** Switch branches to explore specific functionalities.

🔗 **Technical Blog:** [Game Engine Blog](https://zhangzs11.github.io/categories/Game-Engine/)  

## ✨ Features Overview

### 1️⃣ Framebuffer Management & Dual-Threaded Rendering Framework  
🔹 **Effect & Mesh Submission System:**
🔹 **Optimized Framebuffer Structures:**
🔹 **Game Object & Camera Interfaces:** 

---

### 2️⃣ Rendering Pipeline  
🔹 **Forward & Deferred Rendering:** Supports multiple rendering pipelines with normal mapping, FXAA, and skybox.  
🔹 **Shadow Mapping Techniques:** Implemented Cascaded Shadow Maps (CSM), Percentage-Closer Filtering (PCF), and Moving Light Adjustments in Texel-Sized increments.  
🔹 **Optimized G-buffer Rendering:** Reduced storage overhead in deferred shading.  

#### 🌟 Rendering Showcase

<p align="center">
  <img src="./images/e85dd116538f0f95c0b15b2dc786f19.jpg" width="30%" alt="Normal Mapping">
  <img src="./images/1808ad49c882edb639ece129c8037a0.jpg" width="30%" alt="FXAA">
  <img src="./images/16fc8b19ef6339fc59fb4ea1f6ffd32.jpg" width="30%" alt="FXAA">
</p>
<p align="center">
  <img src="./images/0cb6c9ca5f84107edd3e04dcf15f1f9.jpg" width="45%" alt="Normal Mapping">
  <img src="./images/2fb0f2a2e9fad06f3ae4fa331d79d64.jpg" width="45%" alt="FXAA">
</p>

---

### 3️⃣ Mesh Asset Data Pipeline  
🔹 **Maya Export Plugin:** Converts 3D models into human-readable Lua files for efficient asset integration.  
🔹 **Mesh Builder Tool:** Converts Lua-based mesh data into **binary format**, supporting dynamic selection of **16-bit or 32-bit index data** based on performance needs.  
🔹 **Optimized for Performance & Memory Usage.**  

<p align="center">
  <img src="./images/b5bf94cf449a0d3e56e78f1c38e2fed.jpg" width="90%" alt="Normal Mapping">
</p>

## 📂 How to Explore and Run (Windows + Visual Studio 2019)

### 1️⃣ Clone the Repository
```sh
# Clone the repository
git clone https://github.com/zhangzs11/GameGraphicsEngine
cd GameGraphicsEngine
```

### 2️⃣ Switch to a Feature Branch
```sh
# Checkout a specific feature branch
git checkout Graphics_CascadeShadowMap # Example: Switch to CascadeShadowMap implementation
```

### 3️⃣ Open in Visual Studio 2019
Click Open a project or solution, then select the .sln file in the repository folder.
