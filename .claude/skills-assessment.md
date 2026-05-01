# DAW-500: Skills Assessment & 3D Integration Roadmap

## Context

DAW-500 is a production Qt 5.15/C++20 DAW with an existing but incomplete 3D integration layer built on a custom engine (DGE). The team is evaluating whether to continue with DGE or replace it with a simpler 3D backend (Qt3D or Three.js). Before committing to a direction, a clear-eyed skills assessment and investigation is needed to establish what exists, what's missing, and what the path forward requires.

---

## What Was Found in the Codebase

### Working / Well-Implemented
- **Audio engine** — AudioEngine, Mixer, PortAudio I/O, lockless threading, VST/VST3 via JUCE. Solid.
- **Timeline system** — BeatBar, TimePos (bar_t/tick_t), frame-based positioning. Well-structured.
- **Automation curves** — CPoint/CLine with curve_n/curve_m parameters. Functional.
- **Undo system** — JSON-serialized undo operations + QML stack (Ctrl+Z/Y). Basic but works.
- **KeyFramesItem UI** — Stores XYZ position per keyframe in `QMap<qint64, double>` per axis. Connected to timeline.
- **DGE SDK** — CubicBezier, PathController, AnimationComponent, 30+ easing types all present in headers.

### Partial / Incomplete
- **ObjectCreator::currentTimeChanged()** — Declared in `EngineHelper.h`, not implemented. This is the main forward-sync gap (timeline time → 3D position update).
- **3D→Timeline reverse sync** — Signals defined (`sigObjectMovedX/Y/Z`), gizmo handling partial.
- **Bezier bug** — `CubicBezier.h` line 103: `ABC = AB * s + CD * t` should be `AB * s + BC * t` (De Casteljau typo in DGE SDK).

### Not Present
- No Qt3D modules
- No Three.js / WebGL / QtWebEngine integration
- No particle system
- No import/export for animation curves

---

## Required Skills for This Project

### Must Have (Core DAW work)
| Skill | Why Needed |
|---|---|
| **C++20** (concepts, structured bindings, ranges) | Codebase uses `/std:c++20`; AudioEngine threading patterns |
| **Qt 5.15** (Quick, QML, OpenGL painting) | All UI is Qt Quick painted items + QML |
| **qmake build system** | `.pro` + `.pri` modular build; no CMake for main project |
| **JUCE 7** (audio graph, plugin hosting, UndoManager) | VST/VST3, audio threading, data structures |
| **Real-time audio programming** (lockless queues, thread safety) | `LocklessAllocator`, `AudioEngineWorkerThread` patterns |
| **DAW concepts** (timeline, bars/ticks, automation, clips) | Domain model throughout |

### Must Have (3D Integration — regardless of backend)
| Skill | Why Needed |
|---|---|
| **3D math** (vectors, matrices, quaternions, Bezier curves) | Position sync, interpolation, path control |
| **Keyframe animation systems** (easing, interpolation, curve editing) | Central feature |
| **Two-way data binding / observer patterns** | Timeline↔3D sync architecture |
| **OpenGL or scene graph fundamentals** | Whatever 3D backend is chosen builds on these |

### Needed for 3D Backend Decision (choose one path)
| Option | Additional Skills |
|---|---|
| **Keep DGE** | DGE SDK internals, PathController/AnimationComponent APIs, fix existing sync gaps |
| **Qt3D** | Qt3D Extras/Animation/Input modules, QML-integrated scene graph, Qt 5.15 Qt3D limitations |
| **Three.js via QtWebEngine** | JavaScript/TypeScript, Three.js API, Qt↔JS bridge (`QWebChannel`), async boundary management |

### Particle Effects (any backend)
- Instanced rendering concepts
- GPU particle system design or Three.js `Points`/`BufferGeometry`
- Timeline-driven emission rates and envelopes

---

## 3D Backend Trade-off Summary

| Factor | DGE | Qt3D | Three.js (WebEngine) |
|---|---|---|---|
| Already integrated | Yes (partial) | No | No |
| Complexity | High (custom SDK, limited docs) | Medium | Low–Medium |
| Real-time audio sync | Custom signals | Native Qt signals | QWebChannel async |
| Particle system | Needs custom work | Needs custom work | Built-in primitives |
| Debuggability | Low (SDK is precompiled) | High (Qt source available) | High (browser DevTools) |
| 3D feature richness | High (full engine) | Medium (scene graph) | High (large ecosystem) |
| Recommended for this team | Only if DGE docs available | Good middle ground | Best for rapid prototyping |

---

## Recommended Next Steps (Step-by-Step)

### Step 1 — Stabilize the DAW-side interfaces (backend-agnostic)
These are needed regardless of 3D choice and should be done first:
- Define a clean `I3DPositionSync` interface in `EngineHelper.h`: `setEntityPosition(id, x, y, z)` and `onEntityMoved(id, x, y, z)`
- Wire `ObjectCreator::currentTimeChanged()` to call this interface
- Ensure undo operations wrap position changes (already partially in KeyFramesItem)

**Files:** `daw-3-SampleEditor/EngineHelper.h/cpp`, `daw-3-SampleEditor/animationeditor.h/cpp`

### Step 2 — Prototype 3D backend (parallel, time-boxed)
Build a minimal spike for each candidate:
- **Qt3D spike**: Simple scene with one movable entity, driven by a timeline slider. ~1 day.
- **Three.js spike**: `QWebEngineView` + Three.js scene, QWebChannel bridge, same slider test. ~1 day.

Evaluate: latency of sync, debugging experience, particle support, build complexity.

### Step 3 — Commit to backend, wire to Step 1 interface
Implement the chosen backend's adapter for `I3DPositionSync`. The DAW code from Step 1 stays unchanged.

### Step 4 — Re-enable Bezier interpolation
- If staying DGE: fix the typo in `CubicBezier.h:103` and instantiate PathControllers in `AnimationEditor::addKeyFrame()`
- If switching: implement equivalent cubic Bezier in the new backend

### Step 5 — Particle system & audio-reactive features
Once the sync foundation is stable, add particle emitters driven by automation lanes (volume/frequency envelopes already exist in the automation system).

---

## Verification Approach

- **Build check**: `qmake && nmake` (VS2019) — confirms no regressions
- **Timeline sync test**: Add a keyframe at t=0 (pos 0,0,0) and t=4bars (pos 1,1,1). Scrub timeline — verify entity interpolates smoothly.
- **Undo test**: Add keyframe, move entity in 3D, undo — verify position reverts in both DAW and 3D.
- **Bezier test**: Compare CubicBezier output at t=0.5 against reference cubic formula.
