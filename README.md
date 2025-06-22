# ue5_box
# UE5 JSON Box Shooter Project

## Project Purpose

This Unreal Engine 5.6 C++ project implements a **fully dynamic, data-driven box shooter** using JSON for configuration. The key features include:

* Dynamic spawning of box actors based on JSON data.
* Shooting mechanic with line trace detection.
* Real-time score tracking and display through UI.

The goal is to demonstrate runtime content loading and gameplay logic fully controlled by external data, with all major functionality handled in C++.

---

## System Architecture

### 1. JSON Handling (`jsonhandler` class)

* Sends an HTTP GET request to retrieve JSON.
* Parses dynamic and nested JSON into:

  ```cpp
  TArray<TSharedPtr<FJsonObject>> ParsedJsonData;
  ```
* Provides helper functions:

  * `ProcessJson()`
  * `ProcessJsonValue()`
  * `GetTypeDataByName()`

Parsed data is then used to configure and spawn boxes.

---

### 2. Box Spawner

* Reads parsed JSON.
* Spawns cube actors dynamically:

  ```cpp
  SpawnCube(Type, Location, Rotation, Scale, Color, Health, Score);
  ```
* Each box is uniquely configured from JSON properties (color, size, health, score value).

---

### 3. Box Actor (`Aboxhandler`)

Each box actor includes:

* Static mesh with dynamic material.
* Properties:

  * `CubeType`
  * `CubeHealth`
  * `CubeScore`
  * Transform and color

Main functionality:

* `ApplyDamage()`

  * Reduces health on hit.
* `DestroyBox()`

  * Destroys actor when health reaches zero.
* `UpdateScore()`

  * Increments player score.

---

### 4. Player Shooting Mechanic

* Player fires line trace from the weapon muzzle:

  ```cpp
  FireLineTrace();
  ```
* Trace origin: `muzzle` component offset forward.
* On hit:

  * If target is an `Aboxhandler`, applies damage.
  * Draws visual trace line and impact point.

Player score is tracked:

```cpp
int playerscore;
```

---

### 5. Score UI (`UScoreWidget`)

A simple UMG widget displays current score:

* Bound property:

  ```cpp
  UTextBlock* ScoreText;
  ```
* Score updated with:

  ```cpp
  UpdateScoreOnUI(int32 NewScore);
  ```
* Widget initialized in player class:

  ```cpp
  InitializeScoreWidget();
  ```

---

## Challenges Faced

* Completely new experience with UE5.6 and its API.
* One-time crash on first attempt (unknown system/editor cause).
* Syncing HTTP response timing with gameplay.

---

## How to Test

1. Launch the project in **Unreal Engine 5.6**.
2. Press **Play in Editor (PIE)**.

**Controls:**

* `W A S D` → Player movement.
* `F` → Spawn new cubes.
* `Left Mouse Button (LMB)` → Fire and damage cubes.

**Expected behavior:**

* Cubes are spawned based on JSON.
* Player shoots and damages cubes.
* Score is updated on screen when cubes are destroyed.

---

## Debug / Logging Notes

There are several commented **UE\_LOG()** and **GEngine->AddOnScreenDebugMessage()** calls across the project:

* Useful for testing spawn flow, HTTP response, JSON parsing, hit detection, and scoring.
* You can enable these for deeper debugging or performance tracing.

**Suggestion:** Use this for validating game loop and detecting possible race conditions or spawn timing errors.

---

