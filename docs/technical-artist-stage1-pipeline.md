# Stage 1 Art/Animation Pipeline — First-Person Weapon

**Scope:** one weapon model + matching first-person animation set (idle, fire, reload — minimum viable set). Placeholder quality is the target; this document is the repeatable process, not the asset itself.

**Owner:** technical-artist. Art direction calls (weapon silhouette, visual style) are not decided here — they go to studio-director as tradeoffs.

---

## 1. Recommended DCC tool(s) and why

| Tool | Use | Why |
|---|---|---|
| **Blender** (latest stable LTS) | Modeling, UV unwrap, first-pass rigging/animation | Free, no license cost for a pre-revenue indie project, has a mature FBX exporter, and its animation/rigging toolset is sufficient for a first-person weapon rig (small bone count, no deformation complexity). Avoids Maya's per-seat subscription cost during a stage where budget is unproven. |
| **UE5 Control Rig / Sequencer** | Final animation polish and in-engine preview, optionally re-timing | Keeps the "does it feel right in-engine" loop tight — first-person weapon feel is judged against the actual camera FOV and viewmodel offset, which Blender can't reproduce. |
| **Substance Painter (or Blender's own shader/bake tools if Substance isn't licensed)** | Texturing | Not required for Stage 1 placeholder quality — flat/basic PBR materials authored directly in Blender or UE5 are acceptable. Noted here so the next stage's texturing pipeline isn't undocumented. |

**Assumption to confirm with Daniel:** this plan assumes Blender is the DCC of record for modeling and animation, per the brief's guidance to default to standard indie UE5 tooling. If Daniel or a contractor already has a Maya seat and prefers it, the export/import steps below still apply (Maya's FBX exporter needs the same axis/scale/frame-rate settings called out in Section 2) — flag that preference back through studio-director before a contractor starts.

---

## 2. Model + animation import pipeline

### 2.1 File formats
- **Working file:** `.blend` (source of truth, kept in Git LFS — see Dependencies).
- **Interchange format:** **FBX 2020** (Blender's FBX exporter, "Apply Transform" on, forward axis `-Z`, up axis `Y` to match UE5 convention).
- **Static mesh export:** FBX, no animation.
- **Skeletal mesh + animation export:** FBX with armature + animation baked (Blender's "Bake Animation" export option on, all keyframes baked to avoid driver/constraint mismatches on import).

### 2.2 Naming convention
Applies to this weapon and is the template for every future weapon/character.

```
SK_<AssetName>_<Variant>          Skeletal mesh          e.g. SK_Rifle01
SM_<AssetName>_<Variant>          Static mesh (world/pickup version)  e.g. SM_Rifle01
A_<AssetName>_<ActionName>        Animation sequence      e.g. A_Rifle01_Idle, A_Rifle01_Fire, A_Rifle01_Reload
SKEL_<AssetName>                  Skeleton asset          e.g. SKEL_Rifle01
PHAT_<AssetName>                  Physics asset (if needed later)
T_<AssetName>_<MapType>           Texture                e.g. T_Rifle01_BaseColor
M_<AssetName>                     Material                e.g. M_Rifle01
```

Rules:
- All lowercase after the prefix except the asset's own name (Unreal convention, case-sensitive on some platforms — enforce consistently to avoid cross-platform build breaks).
- No spaces, no version numbers baked into the name (use Perforce/Git history for versioning, not `_v2` suffixes).
- Animation action names match the deliverable format exactly (`Idle`, `Fire`, `Reload`) so the Animation Blueprint state machine can bind to them by convention.

### 2.3 UE5 import settings
**Static/world mesh (`SM_Rifle01`):**
- Import as Static Mesh, generate lightmap UVs on (channel 1), collision generated from convex hull for Stage 1 (no hand-authored collision needed for a placeholder pickup).

**Skeletal mesh (`SK_Rifle01`) — first-person viewmodel:**
- Import as Skeletal Mesh, "Import Mesh" + "Import Rig" on Blender's baked FBX.
- Skeletal Mesh scale: verify 1 Unreal Unit = 1 cm matches Blender's export scale (Blender default is 1 unit = 1m; set the FBX export scale to 100 or apply a unit conversion in Blender's scene properties before export — mismatches here are the most common cause of a "giant/tiny gun" bug).
- Import normals/tangents: "Recompute Normals" off if Blender's exported normals are already correct; verify visually after import.
- Skeleton: create new skeleton on first import (`SKEL_Rifle01`), reuse it for all subsequent animation-only imports of this weapon.

**Animations (`A_Rifle01_Idle` / `_Fire` / `_Reload`):**
- Import Animation on, targeting `SKEL_Rifle01`.
- Frame rate: standardize on **30fps** for all Stage 1 weapon animations (matches Blender's default and avoids a resample step; document any deviation per-asset if a contractor's tooling defaults elsewhere).
- Import Uniform Scale = 1.0 once the mesh-level scale issue above is resolved at the source.
- Loop flags: set `Idle` to loop, `Fire`/`Reload` as one-shot (non-looping) in the Anim Sequence asset details after import — this isn't an FBX-level setting, it's set per-asset in UE5.

---

## 3. Rigging approach for first-person weapon animation

Stage 1 does not need a full character/IK rig — a first-person weapon uses a lightweight, hand-authored skeleton:

1. **Skeleton hierarchy:** a single root bone (`root`) parented to nothing, with a `weapon` bone as the mesh's animatable root, and optional child bones only where the model has moving parts that matter for Stage 1 readability (e.g. a `slide` or `bolt` bone if the weapon has a visible action — otherwise skip; don't rig parts that won't be seen or won't move). This keeps bone count minimal, which matters because first-person animation is authored by hand-keying transforms, not through IK solving.
2. **No IK, no physics-driven secondary motion for Stage 1.** Recoil, sway, and bob are handled in the Animation Blueprint (additive animation + curve-driven offsets) layered on top of the base clips, not baked into the FBX animations themselves — this keeps the exported clips reusable if recoil tuning changes later.
3. **Attachment:** the weapon skeleton's root is socketed to the first-person arms/camera rig via a UE5 socket (`hand_r_weapon` or equivalent on the arms skeletal mesh), not hard-parented in the DCC tool — this is what makes the weapon swappable without re-rigging, which is the repeatability requirement for the next weapon.
4. **Animation authoring:** all three clips (idle, fire, reload) are keyed on the same skeleton in Blender using Pose Mode, exported as separate FBX action clips (or split from one timeline via Blender's Action Editor) to keep the naming/import convention in 2.2–2.3 consistent.
5. **Next-weapon repeatability:** because the rig is a flat, minimal-bone skeleton and the socket attachment lives in UE5 rather than the DCC file, a second weapon only needs a new skeleton if its bone count differs (e.g. a two-handed reload with a visible magazine bone) — otherwise it can reuse `SKEL_Rifle01` directly, which should be evaluated before rigging each new weapon from scratch.

---

## 4. Deliverable format

```
ASSET         First-person weapon (SK_Rifle01) — Stage 1 placeholder model with a 3-clip
              animation set (A_Rifle01_Idle, A_Rifle01_Fire, A_Rifle01_Reload).

PIPELINE      Model + rig authored in Blender -> exported as baked FBX (30fps, -Z forward/
              Y up, scale verified at source) -> imported to UE5 as SK_Rifle01 against a
              shared SKEL_Rifle01 skeleton -> animations imported per-clip against the same
              skeleton -> loop flags set per-asset in UE5 -> socketed (not hard-parented) to
              the first-person arms rig so the weapon is swappable. Naming convention and
              skeleton-reuse rule in Sections 2.2 and 3.5 make this repeatable for the next
              weapon/character without re-deriving the process.

DEPENDENCIES  UE5 (project's locked engine version — confirm exact version with game-director,
              not independently pinned here), Blender (latest stable LTS), Git LFS for binary
              asset storage (.blend, .fbx, textures), a machine with the engine + DCC installed
              to actually execute this pipeline (not available in this sandbox — this document
              is the process only).

RISK          Scale/unit mismatch between Blender (1 unit = 1m) and UE5 (1 unit = 1cm) is the
              single most likely failure point on first import and produces an obviously wrong
              result (giant or tiny weapon) that's easy to catch but easy to lose time to if
              the export scale isn't checked before the first FBX export.

DONE WHEN     SK_Rifle01 imports into UE5 without errors or manual fix-up, all three animations
              play correctly on the shared skeleton in the UE5 animation preview at correct
              scale, and the weapon socket-attaches to the first-person arms rig and animates
              in-engine without retargeting issues.
```
