# Ajsfx MetaSound Auditioner

**Version:** 0.1.0 (Beta) &nbsp;|&nbsp; **Engine:** Unreal Engine 5.7 &nbsp;|&nbsp; **Category:** Audio

A dockable editor panel for auditioning sounds through a MetaSound graph. Select any MetaSound asset and the panel automatically discovers all its public inputs — float, int, bool, trigger, and wave asset types — giving you controls to tweak parameters and play back audio without leaving the editor.

---

## Features

- **Auto-discovery** — reads every public input from the selected MetaSound and builds the UI dynamically; no manual wiring required
- **Per-type controls** — float/int spin boxes, bool checkboxes, trigger fire buttons, and wave asset pickers, each appropriate to their type
- **WaveAsset Single & Pool mode** — pick one wave, or define a pool for random selection on each play
- **Re-randomize on Loop** — when looping, choose whether to replay the same wave or pick a new one from the pool each cycle
- **WaveAsset Array inputs** — pass a full array to the MetaSound graph; the graph handles internal selection
- **Drag-and-drop** — drag one or more sound waves from the Content Browser onto any wave input; dropping multiple waves automatically switches to Pool mode
- **Live parameter tweaking** — scalar changes (float, int, bool) apply immediately to the playing audio component
- **Dockable panel** — floats or docks anywhere in the editor layout like any native panel

---

## Requirements

- Unreal Engine 5.7
- MetaSound plugin enabled (included with UE 5.7 by default)

---

## Installation

1. Copy the `AjsfxMetaSoundAuditioner` folder into your project's `Plugins` directory:
   ```
   YourProject/
   └── Plugins/
       └── AjsfxMetaSoundAuditioner/   ← place it here
   ```

2. Right-click your `.uproject` file and choose **Generate Visual Studio project files**.

3. Open the solution in Visual Studio and **build** the project (Development Editor configuration).

4. Launch Unreal Editor, then go to **Edit → Plugins**, search for **MetaSound Auditioner**, enable it, and restart the editor when prompted.

---

## Usage

### Opening the panel

Go to **Tools → MetaSound Auditioner**. The panel opens as a floating window and can be docked anywhere in the editor.

### Auditioning a MetaSound

1. Click the **MetaSound** asset picker at the top of the panel and select a MetaSound source asset.
2. The panel populates with a row for every public input the MetaSound exposes.
3. Configure each input with the provided control (spin box, checkbox, wave picker, etc.).
4. Click **Play** to start playback. Adjust parameters in real time while audio is running.
5. Use the **Loop** checkbox to loop playback. Click **Stop** to halt.

### Drag-and-drop waves

Drag one or more sound waves from the Content Browser directly onto a WaveAsset input:

- Dropping **one wave** respects the current mode (Single or Pool).
- Dropping **multiple waves** automatically switches the input to Pool mode and fills the pool.

---

## Input Types Reference

| Type | Control | Notes |
|---|---|---|
| Float | Spin box | Updates live during playback |
| Int32 | Spin box | Updates live during playback |
| Bool | Checkbox | Updates live during playback |
| Trigger | Button | Fires the trigger input immediately |
| WaveAsset | Single or Pool picker | Drag-drop supported; see Pool Mode below |
| WaveAsset Array | Multi-asset list | Full array passed to the graph; graph controls selection |
| String | Text box | Displayed for reference only — string params are not applied at runtime |

---

## WaveAsset Pool Mode

Each WaveAsset input has two modes, toggled per-input:

**Single mode** — one wave asset is selected and plays every time.

**Randomize Pool mode** — you define a list of waves. On each Play, one is chosen at random.

The **Re-randomize on Loop** checkbox (visible when both Loop and Pool mode are active) controls what happens when a loop cycle completes:

- **Unchecked** — the same wave that played in the previous cycle repeats.
- **Checked** — a new wave is picked from the pool at the start of each loop cycle.

---

## Known Limitations

- **String inputs** — the MetaSound runtime does not support string parameters via `FAudioParameter`; string input rows display for reference but have no effect on playback.
- **Beta status** — this plugin is under active development. APIs and UI may change between versions.

---

## Author

Made by **ajohnsonsfx**  
[github.com/ajohnsonsfx](https://github.com/ajohnsonsfx)
